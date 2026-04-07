#include "io/BmsSerializer.h"
#include "model/Base36.h"
#include "model/ChannelType.h"
#include <QFile>
#include <QStringConverter>
#include <algorithm>
#include <cmath>
#include <map>
#include <vector>

namespace IO {

using namespace Model;

namespace {

int toLongLaneChannel(int channel) {
    if (channel >= 37 && channel <= 45) return channel + 144;
    if (channel >= 73 && channel <= 81) return channel + 144;
    return channel;
}

double toAbsoluteBeat(const BmsDocument& doc, const Note& note) {
    double beat = 0.0;
    for (int m = 0; m < note.measureIndex; ++m)
        beat += doc.measureLength(m) * 4.0;
    beat += note.beat.toDouble() * doc.measureLength(note.measureIndex) * 4.0;
    return beat;
}

void fromAbsoluteBeat(const BmsDocument& doc, double absoluteBeat, int& outMeasure, BeatFraction& outBeat) {
    outMeasure = 0;
    double remain = std::max(0.0, absoluteBeat);
    while (true) {
        double len = doc.measureLength(outMeasure) * 4.0;
        if (remain < len - 1e-9) break;
        remain -= len;
        ++outMeasure;
    }

    int den = 192;
    double measureLenBeats = doc.measureLength(outMeasure) * 4.0;
    double normalized = measureLenBeats > 1e-12 ? (remain / measureLenBeats) : 0.0;
    int num = static_cast<int>(std::round(normalized * den));
    if (num >= den) {
        ++outMeasure;
        num = 0;
    }
    outBeat = {num, den};
}

} // namespace

// Ordered list of well-known header keys for canonical output order
static const char* kHeaderOrder[] = {
    "PLAYER", "GENRE", "TITLE", "SUBTITLE", "ARTIST", "SUBARTIST",
    "BPM", "PLAYLEVEL", "RANK", "TOTAL", "VOLWAV",
    "STAGEFILE", "BANNER", "BACKBMP", "LNOBJ", "LNTYPE",
    nullptr
};

bool BmsSerializer::save(const BmsDocument& doc, QFile& file, Encoding encoding) {
    QStringConverter::Encoding qtEnc = QStringConverter::Utf8;
    if (encoding == Encoding::ShiftJIS || encoding == Encoding::EucJP) {
        auto optEnc = QStringConverter::encodingForName(encodingToCodecName(encoding));
        if (optEnc.has_value()) qtEnc = optEnc.value();
    }
    QStringEncoder encoder(qtEnc);

    QByteArray output;
    if (encoding == Encoding::UTF8)
        output += "\xEF\xBB\xBF";

    auto writeLine = [&](const QString& line) {
        output += encoder(line + "\n");
    };

    // ── Headers ──────────────────────────────────────────────────────────────
    // Write known headers in canonical order first
    std::vector<std::string> writtenKeys;
    for (int i = 0; kHeaderOrder[i]; ++i) {
        std::string k = kHeaderOrder[i];
        auto it = doc.headers.find(k);
        if (it != doc.headers.end()) {
            writeLine(QString("#%1 %2")
                .arg(QString::fromStdString(k))
                .arg(QString::fromStdString(it->second)));
            writtenKeys.push_back(k);
        }
    }
    // Remaining headers in sorted order
    std::map<std::string, std::string> sortedHeaders(doc.headers.begin(), doc.headers.end());
    for (const auto& [k, v] : sortedHeaders) {
        if (std::find(writtenKeys.begin(), writtenKeys.end(), k) == writtenKeys.end()) {
            writeLine(QString("#%1 %2").arg(QString::fromStdString(k)).arg(QString::fromStdString(v)));
        }
    }
    writeLine("");

    // ── WAV table ─────────────────────────────────────────────────────────────
    for (int i = 1; i < kMaxSlots; ++i) {
        if (!doc.wavTable[i].empty()) {
            writeLine(QString("#WAV%1 %2")
                .arg(QString::fromStdString(intToBase36(i)))
                .arg(QString::fromStdString(doc.wavTable[i])));
        }
    }
    // ── BMP table ─────────────────────────────────────────────────────────────
    for (int i = 1; i < kMaxSlots; ++i) {
        if (!doc.bmpTable[i].empty()) {
            writeLine(QString("#BMP%1 %2")
                .arg(QString::fromStdString(intToBase36(i)))
                .arg(QString::fromStdString(doc.bmpTable[i])));
        }
    }
    // ── Named BPM table ───────────────────────────────────────────────────────
    for (int i = 1; i < kMaxSlots; ++i) {
        if (doc.bpmTable[i].bpm > 0.0) {
            writeLine(QString("#BPM%1 %2")
                .arg(QString::fromStdString(intToBase36(i)))
                .arg(doc.bpmTable[i].bpm));
        }
    }
    // ── STOP table ────────────────────────────────────────────────────────────
    for (int i = 1; i < kMaxSlots; ++i) {
        if (doc.stopTable[i].stopBeats > 0.0) {
            writeLine(QString("#STOP%1 %2")
                .arg(QString::fromStdString(intToBase36(i)))
                .arg(static_cast<int>(doc.stopTable[i].stopBeats * 192.0)));
        }
    }
    writeLine("");

    // ── Measure lengths ───────────────────────────────────────────────────────
    for (const auto& [m, len] : doc.measureLengths) {
        if (len != 1.0) {
            writeLine(QString("#%1%2: %3")
                .arg(m, 3, 10, QChar('0'))
                .arg("02") // channel 02 = measure length
                .arg(len));
        }
    }

    // ── Note data ─────────────────────────────────────────────────────────────
    // Group notes by (measure, channel), then encode.
    // Long notes are emitted as LN-channel start/end markers for compatibility.
    std::vector<Note> expandedNotes;
    expandedNotes.reserve(doc.notes.size() * 2);
    for (const auto& n : doc.notes) {
        Note head = n;
        if (n.durationInBeats > 0.0) {
            head.channelIndex = toLongLaneChannel(n.channelIndex);
        }
        expandedNotes.push_back(head);

        if (n.durationInBeats > 0.0) {
            Note tail = head;
            int endMeasure = 0;
            BeatFraction endBeat{};
            fromAbsoluteBeat(doc, toAbsoluteBeat(doc, n) + n.durationInBeats, endMeasure, endBeat);
            tail.measureIndex = endMeasure;
            tail.beat = endBeat;
            tail.durationInBeats = 0.0;
            expandedNotes.push_back(tail);
        }
    }

    using MeasureChannel = std::pair<int,int>;
    std::map<MeasureChannel, std::vector<const Note*>> grouped;
    for (const auto& n : expandedNotes)
        grouped[{n.measureIndex, n.channelIndex}].push_back(&n);

    for (const auto& [mc, notes] : grouped) {
        int measure = mc.first;
        int channel = mc.second;
        // Find LCM of all denominators to determine grid size
        int gridSize = 1;
        for (const auto* n : notes) {
            int d = n->beat.den;
            // LCM via GCD
            int g = gridSize;
            int tmp = d;
            while (tmp) { int r = g % tmp; g = tmp; tmp = r; }
            gridSize = gridSize / g * d;
        }
        if (gridSize < 1) gridSize = 1;
        if (gridSize > 192) gridSize = 192; // cap for sanity

        // Create grid array
        std::vector<int> grid(gridSize, 0);
        for (const auto* n : notes) {
            int pos = (n->beat.num * gridSize) / n->beat.den;
            if (pos >= 0 && pos < gridSize)
                grid[pos] = n->value;
        }

        // Encode grid to base-36 string
        QString data;
        data.reserve(gridSize * 2);
        for (int v : grid) {
            if (v < 1 || v >= kMaxSlots)
                data += "00";
            else
                data += QString::fromStdString(intToBase36(v));
        }

        writeLine(QString("#%1%2:%3")
            .arg(measure, 3, 10, QChar('0'))
            .arg(QString::fromStdString(intToBase36(channel)))
            .arg(data));
    }

    return file.write(output) == output.size();
}

} // namespace IO
