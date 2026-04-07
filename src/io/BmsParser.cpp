#include "io/BmsParser.h"
#include "io/EncodingDetector.h"
#include "model/Base36.h"
#include "model/ChannelType.h"
#include <QFile>
#include <QRegularExpression>
#include <algorithm>
#include <cstdlib>
#include <unordered_map>

namespace IO {

using namespace Model;

namespace {

double absoluteBeat(const BmsDocument& doc, const Note& note) {
    double beat = 0.0;
    for (int m = 0; m < note.measureIndex; ++m)
        beat += doc.measureLength(m) * 4.0;
    beat += note.beat.toDouble() * doc.measureLength(note.measureIndex) * 4.0;
    return beat;
}

void normalizeLongNotes(BmsDocument& doc, std::vector<ParseWarning>& warnings) {
    std::vector<size_t> longIndices;
    longIndices.reserve(doc.notes.size());
    for (size_t i = 0; i < doc.notes.size(); ++i) {
        if (classifyChannel(doc.notes[i].channelIndex) == ChannelType::NoteLong)
            longIndices.push_back(i);
    }
    std::sort(longIndices.begin(), longIndices.end(), [&](size_t a, size_t b) {
        const Note& lhs = doc.notes[a];
        const Note& rhs = doc.notes[b];
        if (lhs.channelIndex != rhs.channelIndex) return lhs.channelIndex < rhs.channelIndex;
        if (lhs.measureIndex != rhs.measureIndex) return lhs.measureIndex < rhs.measureIndex;
        return lhs.beat < rhs.beat;
    });

    std::unordered_map<int, size_t> pendingByChannel;
    std::vector<size_t> removeTailIndices;

    for (size_t idx : longIndices) {
        const Note& curr = doc.notes[idx];
        auto it = pendingByChannel.find(curr.channelIndex);
        if (it == pendingByChannel.end()) {
            pendingByChannel[curr.channelIndex] = idx;
            continue;
        }

        size_t startIdx = it->second;
        pendingByChannel.erase(it);

        double startBeat = absoluteBeat(doc, doc.notes[startIdx]);
        double endBeat = absoluteBeat(doc, curr);
        double duration = endBeat - startBeat;
        if (duration > 0.0) {
            doc.notes[startIdx].durationInBeats = duration;
            removeTailIndices.push_back(idx);
        }
    }

    if (!pendingByChannel.empty()) {
        warnings.push_back({ParseWarningCode::InvalidBase36Token, 0,
                            "Unpaired long-note start found in long-note channel"});
    }

    std::sort(removeTailIndices.begin(), removeTailIndices.end(), std::greater<size_t>());
    for (size_t idx : removeTailIndices) {
        doc.notes.erase(doc.notes.begin() + static_cast<std::ptrdiff_t>(idx));
    }
}

} // namespace

ParseResult BmsParser::parse(QFile& file) {
    ParseResult result;
    QByteArray raw = file.readAll();
    Encoding enc = detectEncoding(raw);
    QString text = decodeBytes(raw, enc);

    QStringList lines = text.split('\n');
    std::vector<std::pair<int,int>> seenChannelRows; // (measure<<16 | channel)

    int lineNum = 0;
    for (const QString& rawLine : lines) {
        ++lineNum;
        QString line = rawLine.trimmed();
        if (line.isEmpty() || !line.startsWith('#'))
            continue;
        processLine(line, lineNum, result, seenChannelRows);
    }

    normalizeLongNotes(result.doc, result.warnings);

    // Default BPM warning
    if (result.doc.headers.find("BPM") == result.doc.headers.end()) {
        result.warnings.push_back({ParseWarningCode::MissingBpm, 0, "No #BPM directive; defaulting to 130"});
    }

    return result;
}

static int parseBase36Channel(const QString& s) {
    if (s.size() < 2) return -1;
    QByteArray bytes = s.left(2).toUpper().toLatin1();
    return base36ToInt(bytes.constData());
}

void BmsParser::processLine(const QString& line, int lineNum, ParseResult& result,
                             std::vector<std::pair<int,int>>& seenChannelRows) {
    // Channel data row: #MMMMCC:data  (measure 0-999, channel 2-char base-36)
    // Regex: #(\d{3,4})([0-9A-Za-z]{2}):(.*)
    static const QRegularExpression channelRe(
        R"(#(\d{3,4})([0-9A-Za-z]{2}):(.*))", QRegularExpression::CaseInsensitiveOption);
    auto m = channelRe.match(line);
    if (m.hasMatch()) {
        int measure = m.captured(1).toInt();
        QByteArray chBytes = m.captured(2).toUpper().toLatin1();
        int channel = base36ToInt(chBytes.constData());
        if (channel < 0) {
            result.warnings.push_back({ParseWarningCode::InvalidBase36Token, lineNum,
                                        "Invalid channel: " + m.captured(2)});
            return;
        }
        // Check for duplicate channel row
        int key = (measure << 16) | channel;
        if (std::find(seenChannelRows.begin(), seenChannelRows.end(),
                      std::make_pair(measure, channel)) != seenChannelRows.end()) {
            result.warnings.push_back({ParseWarningCode::DuplicateChannelRow, lineNum,
                                        QString("Duplicate #%1%2 row").arg(measure, 3, 10, QChar('0')).arg(m.captured(2))});
        } else {
            seenChannelRows.emplace_back(measure, channel);
        }
        decodeChannelRow(measure, channel, m.captured(3).trimmed(), result.doc, result.warnings, lineNum);
        return;
    }

    // Header directive: #KEY VALUE
    // Split at first space
    int spacePos = line.indexOf(' ');
    if (spacePos < 0) return; // no value

    QString key = line.mid(1, spacePos - 1).toUpper().trimmed();
    QString value = line.mid(spacePos + 1).trimmed();

    if (key.isEmpty()) return;

    // WAVxx / BMPxx / BPMxx / STOPxx
    if (key.startsWith("WAV") && key.size() == 5) {
        QByteArray slot = key.mid(3).toLatin1();
        int idx = base36ToInt(slot.constData());
        if (idx >= 1 && idx < kMaxSlots)
            result.doc.wavTable[idx] = value.toStdString();
        else
            result.warnings.push_back({ParseWarningCode::OutOfRangeSlot, lineNum, "WAV slot out of range: " + key});
        return;
    }
    if (key.startsWith("BMP") && key.size() == 5) {
        QByteArray slot = key.mid(3).toLatin1();
        int idx = base36ToInt(slot.constData());
        if (idx >= 1 && idx < kMaxSlots)
            result.doc.bmpTable[idx] = value.toStdString();
        return;
    }
    if (key.startsWith("BPM") && key.size() == 5) {
        QByteArray slot = key.mid(3).toLatin1();
        int idx = base36ToInt(slot.constData());
        if (idx >= 1 && idx < kMaxSlots) {
            result.doc.bpmTable[idx].slot = idx;
            result.doc.bpmTable[idx].bpm = value.toDouble();
        }
        return;
    }
    if (key.startsWith("STOP") && key.size() == 6) {
        QByteArray slot = key.mid(4).toLatin1();
        int idx = base36ToInt(slot.constData());
        if (idx >= 1 && idx < kMaxSlots) {
            result.doc.stopTable[idx].slot = idx;
            // STOP value is in 1/192 of a whole note = 1/48 beat
            double val = value.toDouble();
            result.doc.stopTable[idx].stopBeats = val / 192.0;
        }
        return;
    }

    // Generic header
    result.doc.headers[key.toStdString()] = value.toStdString();
}

void BmsParser::decodeChannelRow(int measure, int channel, const QString& data,
                                  BmsDocument& doc, std::vector<ParseWarning>& warnings,
                                  int lineNum) {
    if (data.isEmpty()) return;
    int len = data.length();
    if (len % 2 != 0) {
        warnings.push_back({ParseWarningCode::InvalidBase36Token, lineNum,
                             "Channel data has odd length; ignoring row"});
        return;
    }
    int numSlots = len / 2;
    for (int i = 0; i < numSlots; ++i) {
        QByteArray slotStr = data.mid(i * 2, 2).toUpper().toLatin1();
        int slotVal = base36ToInt(slotStr.constData());
        if (slotVal < 0) continue; // "00" = empty slot

        Note note;
        note.channelIndex = channel;
        note.measureIndex = measure;
        note.beat = {i, numSlots};
        note.value = slotVal;
        ChannelType ct = classifyChannel(channel);
        note.hidden = (ct == ChannelType::NoteHidden);
        note.landmine = (ct == ChannelType::NoteMine);
        doc.notes.push_back(note);
    }
}

} // namespace IO
