#include "editor/EditorDocument.h"
#include "model/ChannelType.h"
#include "editor/commands/PlaceNoteCommand.h"
#include "editor/commands/RemoveNoteCommand.h"
#include "editor/commands/DeleteNotesCommand.h"
#include "editor/commands/EditHeaderCommand.h"
#include "editor/commands/EditResourceCommand.h"
#include "editor/commands/TimeSelectionCommand.h"
#include "editor/commands/DeleteMeasureCommand.h"
#include "editor/commands/CopyMeasureCommand.h"
#include "editor/commands/PasteMeasureCommand.h"
#include "editor/commands/ConvertLongShortCommand.h"
#include "editor/commands/ConvertHiddenCommand.h"
#include "editor/commands/MirrorNotesCommand.h"
#include "editor/commands/StormCommand.h"
#include "editor/commands/ModifyLabelsCommand.h"
#include "editor/commands/TimeSelectionTools.h"
#include "editor/commands/SetMeasureLengthCommand.h"
#include "editor/EditorUtil.h"
#include "model/Base36.h"
#include "model/TimingMap.h"
#include <algorithm>
#include <QClipboard>
#include <QGuiApplication>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <cmath>
#include <utility>

namespace Editor {

} // namespace

namespace Editor {

EditorDocument::EditorDocument(QObject* parent)
    : QObject(parent) {
    connect(&m_undoStack, &QUndoStack::cleanChanged, this, &EditorDocument::onCleanChanged);
    connect(&m_undoStack, &QUndoStack::indexChanged, this, &EditorDocument::onIndexChanged);
}

void EditorDocument::setFilePath(const QString& path) {
    if (m_filePath == path) return;
    m_filePath = path;
    emit filePathChanged();
    emit selectedNoteInfoChanged();
}

void EditorDocument::resetDocument(Model::BmsDocument doc, const QString& filePath) {
    m_doc = std::move(doc);
    m_undoStack.clear();
    if (!filePath.isEmpty()) setFilePath(filePath);
    emit documentChanged();
    emit isDirtyChanged();
    emit canUndoChanged();
    emit canRedoChanged();
    emit selectedNoteInfoChanged();
}

void EditorDocument::markClean() {
    m_undoStack.setClean();
}

void EditorDocument::onCleanChanged(bool /*clean*/) {
    emit isDirtyChanged();
}

QString EditorDocument::headerValue(const QString& key) const {
    return QString::fromStdString(m_doc.header(key.toStdString()));
}

void EditorDocument::setHeaderValue(const QString& key, const QString& value) {
    m_undoStack.push(new Commands::EditHeaderCommand(this, key.toStdString(), value.toStdString()));
}

// Check if a note already exists at the same position, or if the position
// falls within an existing long note's range on the same channel.
static bool hasNoteAt(const Model::BmsDocument& doc, int channelIndex, int measureIndex, int beatNum, int beatDen) {
    // Compute absolute beat of the candidate position
    double candBeat = 0.0;
    for (int m = 0; m < measureIndex; ++m)
        candBeat += doc.measureLength(m) * 4.0;
    double measLen = doc.measureLength(measureIndex) * 4.0;
    candBeat += (static_cast<double>(beatNum) / beatDen) * measLen;

    for (const auto& n : doc.notes) {
        if (n.channelIndex != channelIndex) continue;
        // Compute absolute beat of existing note
        double noteBeat = 0.0;
        for (int m = 0; m < n.measureIndex; ++m)
            noteBeat += doc.measureLength(m) * 4.0;
        noteBeat += n.beat.toDouble() * doc.measureLength(n.measureIndex) * 4.0;

        // Exact position match
        if (std::abs(noteBeat - candBeat) < 1e-9)
            return true;
        // Candidate falls within a long note's range
        if (n.durationInBeats > 0.0) {
            double noteEnd = noteBeat + n.durationInBeats;
            if (candBeat > noteBeat + 1e-9 && candBeat < noteEnd - 1e-9)
                return true;
        }
    }
    return false;
}

void EditorDocument::placeNote(int channelIndex, int measureIndex, int beatNum, int beatDen, int value) {
    if (hasNoteAt(m_doc, channelIndex, measureIndex, beatNum, beatDen)) return;
    Model::Note note{};
    note.channelIndex = channelIndex;
    note.measureIndex = measureIndex;
    note.beat = {beatNum, beatDen};
    note.value = value;
    m_undoStack.push(new Commands::PlaceNoteCommand(this, note));
}

void EditorDocument::placeNoteWithDuration(int channelIndex, int measureIndex, int beatNum, int beatDen, int value, double durationInBeats) {
    if (hasNoteAt(m_doc, channelIndex, measureIndex, beatNum, beatDen)) return;
    Model::Note note{};
    note.channelIndex = channelIndex;
    note.measureIndex = measureIndex;
    note.beat = {beatNum, beatDen};
    note.value = value;
    note.durationInBeats = std::max(0.0, durationInBeats);
    m_undoStack.push(new Commands::PlaceNoteCommand(this, note));
}

void EditorDocument::deleteNote(int channelIndex, int measureIndex, int beatNum, int beatDen) {
    for (const auto& n : m_doc.notes) {
        if (n.channelIndex == channelIndex && n.measureIndex == measureIndex
            && n.beat.num == beatNum && n.beat.den == beatDen) {
            m_undoStack.push(new Commands::RemoveNoteCommand(this, n));
            break;
        }
    }
}

bool EditorDocument::copySelection() {
    std::vector<Model::Note> selected;
    selected.reserve(m_doc.notes.size());
    for (const auto& n : m_doc.notes) {
        if (n.selected) selected.push_back(n);
    }
    if (selected.empty()) return false;

    double minBeat = noteAbsoluteBeat(m_doc, selected.front());
    for (const auto& n : selected) {
        minBeat = std::min(minBeat, noteAbsoluteBeat(m_doc, n));
    }

    QJsonArray items;
    for (const auto& n : selected) {
        QJsonObject obj;
        obj["channel"] = n.channelIndex;
        obj["value"] = n.value;
        obj["duration"] = n.durationInBeats;
        obj["relativeBeat"] = noteAbsoluteBeat(m_doc, n) - minBeat;
        items.append(obj);
    }

    QJsonObject root;
    root["format"] = "ibmsc-note-clipboard-v1";
    root["notes"] = items;
    QString payload = QStringLiteral("IBMSC_NOTES_JSON\n") + QString::fromUtf8(QJsonDocument(root).toJson(QJsonDocument::Compact));
    QGuiApplication::clipboard()->setText(payload);
    return true;
}

bool EditorDocument::cutSelection() {
    std::vector<Model::Note> selected;
    selected.reserve(m_doc.notes.size());
    for (const auto& n : m_doc.notes) {
        if (n.selected) selected.push_back(n);
    }
    if (selected.empty()) return false;
    if (!copySelection()) return false;

    m_undoStack.push(new Commands::DeleteNotesCommand(this, std::move(selected)));
    return true;
}

bool EditorDocument::pasteFromClipboard(double anchorBeat) {
    QString payload = QGuiApplication::clipboard()->text();
    const QString prefix = QStringLiteral("IBMSC_NOTES_JSON\n");
    if (!payload.startsWith(prefix)) return false;

    QJsonParseError err{};
    QJsonDocument doc = QJsonDocument::fromJson(payload.mid(prefix.size()).toUtf8(), &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) return false;

    QJsonArray notes = doc.object().value("notes").toArray();
    if (notes.isEmpty()) return false;

    m_undoStack.beginMacro(tr("Paste Notes"));
    for (const auto& v : notes) {
        QJsonObject obj = v.toObject();
        Model::Note note{};
        note.channelIndex = obj.value("channel").toInt();
        note.value = obj.value("value").toInt(1);
        note.durationInBeats = std::max(0.0, obj.value("duration").toDouble(0.0));

        int measure = 0;
        Model::BeatFraction beat{};
        absoluteBeatToPosition(m_doc, anchorBeat + obj.value("relativeBeat").toDouble(0.0), measure, beat);
        note.measureIndex = measure;
        note.beat = beat;

        m_undoStack.push(new Commands::PlaceNoteCommand(this, note));
    }
    m_undoStack.endMacro();
    return true;
}

void EditorDocument::insertEmptyMeasure(int measureIndex, int count) {
    if (count <= 0) return;
    m_undoStack.push(new Commands::TimeSelectionCommand(this, std::max(0, measureIndex), count));
}

void EditorDocument::deleteMeasure(int measureIndex, int count) {
    if (count <= 0) return;
    m_undoStack.push(new Commands::DeleteMeasureCommand(this, std::max(0, measureIndex), count));
}

void EditorDocument::copyMeasure(int measureIndex, int count) {
    if (count <= 0) return;
    m_undoStack.push(new Commands::CopyMeasureCommand(this, std::max(0, measureIndex), count));
}

void EditorDocument::pasteMeasure(int measureIndex) {
    if (m_measureClipboardSpan <= 0 || m_measureClipboardNotes.empty()) return;
    m_undoStack.push(new Commands::PasteMeasureCommand(this, std::max(0, measureIndex)));
}

void EditorDocument::setMeasureClipboard(std::vector<Model::Note> notes, int span) {
    m_measureClipboardNotes = std::move(notes);
    m_measureClipboardSpan = std::max(0, span);
}

QVariantList EditorDocument::timingEvents() const {
    using namespace Model;
    QVariantList result;
    for (const auto& note : m_doc.notes) {
        ChannelType ct = classifyChannel(note.channelIndex);
        if (ct != ChannelType::BPM && ct != ChannelType::BpmExtended && ct != ChannelType::Stop)
            continue;

        double beat = note.measureIndex * 4.0
                    + note.beat.toDouble() * 4.0 * m_doc.measureLength(note.measureIndex);

        QString label;
        bool isStop = false;
        if (ct == ChannelType::BPM) {
            label = QString("BPM %1").arg(note.value);
        } else if (ct == ChannelType::BpmExtended) {
            double bpm = (note.value >= 1 && note.value < kMaxSlots)
                         ? m_doc.bpmTable[note.value].bpm : 0.0;
            label = QString("BPM %1").arg(bpm, 0, 'f', 0);
        } else {
            double beats = (note.value >= 1 && note.value < kMaxSlots)
                           ? m_doc.stopTable[note.value].stopBeats : 0.0;
            label = QString("STOP %1b").arg(beats, 0, 'f', 1);
            isStop = true;
        }

        QVariantMap ev;
        ev["beat"]   = beat;
        ev["label"]  = label;
        ev["isStop"] = isStop;
        result.append(ev);
    }
    return result;
}

void EditorDocument::setWavFilename(int slot, const QString& filename) {
    m_undoStack.push(new Commands::EditResourceCommand(
        this, Commands::ResourceType::WAV, slot, filename.toStdString()));
}

void EditorDocument::setBmpFilename(int slot, const QString& filename) {
    m_undoStack.push(new Commands::EditResourceCommand(
        this, Commands::ResourceType::BMP, slot, filename.toStdString()));
}

void EditorDocument::onIndexChanged() {
    emit canUndoChanged();
    emit canRedoChanged();
    emit documentChanged();
    emit selectedNoteInfoChanged();
}

QVariantMap EditorDocument::selectedNoteInfo() const {
    QVariantMap info;
    int selectedCount = 0;
    const Model::Note* selected = nullptr;
    for (const auto& note : m_doc.notes) {
        if (!note.selected) continue;
        ++selectedCount;
        if (!selected) selected = &note;
    }

    info["hasSelection"] = (selected != nullptr);
    info["selectedCount"] = selectedCount;
    if (!selected) return info;

    const auto& note = *selected;
    const double measureBeats = m_doc.measureLength(note.measureIndex) * 4.0;
    const double absoluteBeat = noteAbsoluteBeat(m_doc, note);
    Model::TimingMap timing(m_doc);

    int duplicateCount = 0;
    for (const auto& other : m_doc.notes) {
        if (other.channelIndex == note.channelIndex &&
            other.measureIndex == note.measureIndex &&
            other.beat == note.beat) {
            ++duplicateCount;
        }
    }

    const bool hasSoundSlot = note.value >= 1 && note.value < Model::kMaxSlots;
    QString resolvedFilename;
    if (hasSoundSlot)
        resolvedFilename = QString::fromStdString(m_doc.wavTable[note.value]);

    info["channelIndex"] = note.channelIndex;
    info["measureIndex"] = note.measureIndex;
    info["beatNumerator"] = note.beat.num;
    info["beatDenominator"] = note.beat.den;
    info["absoluteBeat"] = absoluteBeat;
    info["seconds"] = timing.beatToSeconds(note.measureIndex, note.beat.toDouble() * m_doc.measureLength(note.measureIndex));
    info["measureBeatString"] = QString("%1 / %2").arg(note.beat.num).arg(note.beat.den);
    info["measureLengthBeats"] = measureBeats;
    info["value"] = note.value;
    info["slotIndex"] = hasSoundSlot ? note.value : 0;
    info["slotLabel"] = hasSoundSlot ? QString::fromStdString(Model::intToBase36(note.value)) : QString();
    info["filename"] = resolvedFilename;
    info["displayFilename"] = resolvedFilename.isEmpty() ? QStringLiteral("\u2014") : QFileInfo(resolvedFilename).fileName();
    info["durationInBeats"] = note.durationInBeats;
    info["isLong"] = note.durationInBeats > 0.0;
    info["isHidden"] = note.hidden;
    info["isLandmine"] = note.landmine;
    info["hasError"] = duplicateCount > 1;
    return info;
}

void EditorDocument::notifySelectionChanged() {
    emit documentChanged();
    emit selectedNoteInfoChanged();
}

// ── Note conversion commands ─────────────────────────────────────────────────

QVariantMap EditorDocument::noteStatistics() const {
    int total = 0, bpm = 0, stop = 0, bgm = 0;
    int aShort = 0, aLong = 0, aHidden = 0, aMine = 0;
    int dShort = 0, dLong = 0, dHidden = 0, dMine = 0;
    for (const auto& n : m_doc.notes) {
        ++total;
        auto ct = Model::classifyChannel(n.channelIndex);
        using CT = Model::ChannelType;
        switch (ct) {
            case CT::BPM: case CT::BpmExtended: ++bpm; break;
            case CT::Stop: ++stop; break;
            case CT::BGM: ++bgm; break;
            case CT::NoteNormal:
                if (n.channelIndex >= 37 && n.channelIndex <= 45) {
                    if (n.durationInBeats > 0) ++aLong; else ++aShort;
                } else {
                    if (n.durationInBeats > 0) ++dLong; else ++dShort;
                }
                break;
            case CT::NoteLong:
                if (n.channelIndex >= 181 && n.channelIndex <= 189) ++aLong; else ++dLong;
                break;
            case CT::NoteHidden:
                if (n.channelIndex >= 109 && n.channelIndex <= 117) ++aHidden; else ++dHidden;
                break;
            case CT::NoteMine:
                if (n.channelIndex >= 469 && n.channelIndex <= 477) ++aMine; else ++dMine;
                break;
            default: break;
        }
    }
    QVariantMap m;
    m["total"] = total;
    m["bpm"] = bpm;
    m["stop"] = stop;
    m["bgm"] = bgm;
    m["aShort"] = aShort;
    m["aLong"] = aLong;
    m["aHidden"] = aHidden;
    m["aMine"] = aMine;
    m["dShort"] = dShort;
    m["dLong"] = dLong;
    m["dHidden"] = dHidden;
    m["dMine"] = dMine;
    return m;
}

void EditorDocument::convertToLong() {
    m_undoStack.push(new Commands::ConvertLongShortCommand(this, Commands::ConvertLongShortCommand::ToLong));
}

void EditorDocument::convertToShort() {
    m_undoStack.push(new Commands::ConvertLongShortCommand(this, Commands::ConvertLongShortCommand::ToShort));
}

void EditorDocument::toggleLongShort() {
    m_undoStack.push(new Commands::ConvertLongShortCommand(this, Commands::ConvertLongShortCommand::Toggle));
}

void EditorDocument::convertToHidden() {
    m_undoStack.push(new Commands::ConvertHiddenCommand(this, Commands::ConvertHiddenCommand::ToHidden));
}

void EditorDocument::convertToVisible() {
    m_undoStack.push(new Commands::ConvertHiddenCommand(this, Commands::ConvertHiddenCommand::ToVisible));
}

void EditorDocument::toggleHidden() {
    m_undoStack.push(new Commands::ConvertHiddenCommand(this, Commands::ConvertHiddenCommand::Toggle));
}

void EditorDocument::mirrorNotes() {
    m_undoStack.push(new Commands::MirrorNotesCommand(this));
}

void EditorDocument::stormNotes() {
    m_undoStack.push(new Commands::StormCommand(this));
}

void EditorDocument::modifyLabels(int value) {
    m_undoStack.push(new Commands::ModifyLabelsCommand(this, value));
}

void EditorDocument::expandByRatio(double startBeat, double endBeat, double ratio) {
    m_undoStack.push(new Commands::ExpandByRatioCommand(this, startBeat, endBeat, ratio));
}

void EditorDocument::reverseSelection(double startBeat, double endBeat) {
    m_undoStack.push(new Commands::ReverseSelectionCommand(this, startBeat, endBeat));
}

void EditorDocument::setMeasureLength(int measureIndex, double length) {
    m_undoStack.push(new Commands::SetMeasureLengthCommand(this, measureIndex, length));
}

} // namespace Editor
