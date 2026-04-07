#include "editor/commands/BpmToolsCommands.h"
#include "editor/EditorDocument.h"
#include "editor/EditorUtil.h"
#include "model/TimingMap.h"
#include "model/ChannelType.h"
#include <QUndoCommand>
#include <cmath>
#include <algorithm>

namespace Editor::Commands {

using namespace Model;

// ── ConstBpmCommand ─────────────────────────────────────────────────────────

ConstBpmCommand::ConstBpmCommand(EditorDocument* doc, double targetBpm, QUndoCommand* parent)
    : QUndoCommand("Constant BPM", parent), m_doc(doc), m_targetBpm(targetBpm) {}

void ConstBpmCommand::redo() {
    auto& doc = m_doc->document();
    m_oldNotes = doc.notes;
    m_oldHeaders = doc.headers;

    TimingMap timing(doc);

    struct NoteTime { Note note; double timeSec; double endTimeSec; };
    std::vector<NoteTime> timed;
    for (const auto& n : doc.notes) {
        ChannelType ct = classifyChannel(n.channelIndex);
        if (ct == ChannelType::BPM || ct == ChannelType::BpmExtended) continue;
        double absBeat = noteAbsoluteBeat(doc, n);
        double sec = timing.cumulativeBeatToSeconds(absBeat);
        double endSec = sec;
        if (n.durationInBeats > 0.0)
            endSec = timing.cumulativeBeatToSeconds(absBeat + n.durationInBeats);
        timed.push_back({n, sec, endSec});
    }

    std::vector<Note> newNotes;
    newNotes.reserve(timed.size());
    for (auto& nt : timed) {
        double newAbsBeat = nt.timeSec * m_targetBpm / 60.0;
        Note nn = nt.note;
        int meas; BeatFraction bf;
        absoluteBeatToPosition(doc, newAbsBeat, meas, bf);
        nn.measureIndex = meas;
        nn.beat = bf;
        if (nn.durationInBeats > 0.0) {
            double newEndBeat = nt.endTimeSec * m_targetBpm / 60.0;
            nn.durationInBeats = std::max(0.0, newEndBeat - newAbsBeat);
        }
        newNotes.push_back(nn);
    }

    doc.notes = newNotes;
    doc.headers["BPM"] = std::to_string(static_cast<int>(m_targetBpm));
    m_newNotes = doc.notes;
    m_newHeaders = doc.headers;
    emit m_doc->documentChanged();
}

void ConstBpmCommand::undo() {
    auto& doc = m_doc->document();
    doc.notes = m_oldNotes;
    doc.headers = m_oldHeaders;
    emit m_doc->documentChanged();
}

// ── ScaleBpmCommand ─────────────────────────────────────────────────────────

ScaleBpmCommand::ScaleBpmCommand(EditorDocument* doc, double ratio, QUndoCommand* parent)
    : QUndoCommand("Scale BPM", parent), m_doc(doc), m_ratio(ratio) {}

void ScaleBpmCommand::redo() {
    auto& doc = m_doc->document();
    m_oldNotes = doc.notes;
    m_oldHeaders = doc.headers;

    auto it = doc.headers.find("BPM");
    if (it != doc.headers.end()) {
        try {
            double bpm = std::stod(it->second) * m_ratio;
            it->second = std::to_string(static_cast<int>(std::round(bpm)));
        } catch (...) {}
    }

    for (int i = 1; i < kMaxSlots; ++i) {
        if (doc.bpmTable[i].bpm > 0.0)
            doc.bpmTable[i].bpm *= m_ratio;
    }

    for (auto& n : doc.notes) {
        ChannelType ct = classifyChannel(n.channelIndex);
        if (ct == ChannelType::BPM)
            n.value = static_cast<int>(std::round(n.value * m_ratio));
    }

    m_newNotes = doc.notes;
    m_newHeaders = doc.headers;
    emit m_doc->documentChanged();
}

void ScaleBpmCommand::undo() {
    auto& doc = m_doc->document();
    doc.notes = m_oldNotes;
    doc.headers = m_oldHeaders;
    for (int i = 1; i < kMaxSlots; ++i) {
        if (doc.bpmTable[i].bpm > 0.0)
            doc.bpmTable[i].bpm /= m_ratio;
    }
    emit m_doc->documentChanged();
}

} // namespace Editor::Commands
