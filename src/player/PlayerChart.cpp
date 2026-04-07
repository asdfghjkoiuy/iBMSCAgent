#include "player/PlayerChart.h"
#include <algorithm>

namespace Player {

// Map BMS channel identifier to visual lane index.
// Returns -1 for BGM/non-player channels.
static int channelToLane(int ch) {
    using namespace Model;
    ChannelType ct = classifyChannel(ch);
    if (ct == ChannelType::BGM) return -1;
    if (!isPlayerNoteChannel(ct)) return -1;
    // P1 keys: ch 37..45 → lane 0..8
    if (ch >= 37 && ch <= 45) return ch - 37;
    // P1 long: ch 181..189 → lane 0..8
    if (ch >= 181 && ch <= 189) return ch - 181;
    // P1 hidden: ch 109..117 → lane 0..8
    if (ch >= 109 && ch <= 117) return ch - 109;
    return -1;
}

PlayerChart PlayerChart::build(const Model::BmsDocument& doc, int startMeasure) {
    PlayerChart chart;

    auto it = doc.headers.find("TITLE");
    if (it != doc.headers.end()) chart.info.title = it->second;
    it = doc.headers.find("ARTIST");
    if (it != doc.headers.end()) chart.info.artist = it->second;
    it = doc.headers.find("BPM");
    if (it != doc.headers.end()) {
        try { chart.info.bpm = std::stod(it->second); } catch (...) {}
    }

    Model::TimingMap timing(doc);
    chart.totalDuration = timing.totalDuration();

    double startTimeSec = 0.0;
    if (startMeasure > 0) {
        double startBeat = timing.measureBeatToCumulative(startMeasure, 0.0);
        startTimeSec = timing.cumulativeBeatToSeconds(startBeat);
    }

    for (const auto& note : doc.notes) {
        if (note.measureIndex < startMeasure) continue;
        if (note.value <= 0) continue;

        double beatOffset = note.beat.toDouble() * doc.measureLength(note.measureIndex);
        double absBeat = timing.measureBeatToCumulative(note.measureIndex, beatOffset);
        double timeSec = timing.cumulativeBeatToSeconds(absBeat) - startTimeSec;

        PlayEvent ev;
        ev.timeSec = timeSec;
        ev.slotIndex = note.value;
        ev.lane = channelToLane(note.channelIndex);
        ev.isLong = note.durationInBeats > 0.0;
        if (ev.isLong) {
            double endBeat = absBeat + note.durationInBeats;
            ev.endTimeSec = timing.cumulativeBeatToSeconds(endBeat) - startTimeSec;
        }
        chart.events.push_back(ev);
    }

    std::sort(chart.events.begin(), chart.events.end(),
              [](const PlayEvent& a, const PlayEvent& b) { return a.timeSec < b.timeSec; });

    chart.totalDuration -= startTimeSec;
    return chart;
}

} // namespace Player
