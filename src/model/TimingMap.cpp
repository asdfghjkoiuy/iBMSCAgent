#include "model/TimingMap.h"
#include "model/ChannelType.h"
#include <algorithm>
#include <cmath>

namespace Model {

constexpr int kMaxMeasures = 1000;

TimingMap::TimingMap(const BmsDocument& doc) {
    build(doc);
}

void TimingMap::build(const BmsDocument& doc) {
    // Collect BPM and STOP events from notes
    struct RawEvent {
        double cumulativeBeat;
        double bpmValue; // 0 if this is a stop event
        double stopBeats; // only used for stop events
        bool isStop;
    };
    std::vector<RawEvent> rawEvents;

    // Pre-compute cumulative beat starts for each measure
    m_measureBeatStart.resize(kMaxMeasures + 1, 0.0);
    double cumulativeBeat = 0.0;
    for (int m = 0; m <= kMaxMeasures; ++m) {
        m_measureBeatStart[m] = cumulativeBeat;
        // Each measure has 4 * measureLength beats (one measure = 4 quarter-note beats)
        cumulativeBeat += 4.0 * doc.measureLength(m);
    }

    // Gather BPM/STOP notes
    for (const auto& note : doc.notes) {
        ChannelType ct = classifyChannel(note.channelIndex);
        if (ct == ChannelType::BPM) {
            // value is hex-encoded BPM (00-FF)
            double bpm = static_cast<double>(note.value);
            if (bpm > 0.0) {
                double cb = m_measureBeatStart[note.measureIndex] + 4.0 * note.beat.toDouble() * doc.measureLength(note.measureIndex);
                rawEvents.push_back({cb, bpm, 0.0, false});
            }
        } else if (ct == ChannelType::BpmExtended) {
            // value is slot index into bpmTable
            if (note.value >= 1 && note.value < kMaxSlots) {
                double bpm = doc.bpmTable[note.value].bpm;
                if (bpm > 0.0) {
                    double cb = m_measureBeatStart[note.measureIndex] + 4.0 * note.beat.toDouble() * doc.measureLength(note.measureIndex);
                    rawEvents.push_back({cb, bpm, 0.0, false});
                }
            }
        } else if (ct == ChannelType::Stop) {
            if (note.value >= 1 && note.value < kMaxSlots) {
                double stopBeats = doc.stopTable[note.value].stopBeats;
                if (stopBeats > 0.0) {
                    double cb = m_measureBeatStart[note.measureIndex] + 4.0 * note.beat.toDouble() * doc.measureLength(note.measureIndex);
                    rawEvents.push_back({cb, 0.0, stopBeats, true});
                }
            }
        }
    }

    // Sort events by cumulative beat
    std::sort(rawEvents.begin(), rawEvents.end(), [](const RawEvent& a, const RawEvent& b) {
        return a.cumulativeBeat < b.cumulativeBeat;
    });

    // Build timing event list
    m_events.clear();
    double currentBpm = doc.initialBpm();
    double currentTime = 0.0;
    double currentBeat = 0.0;

    m_events.push_back({currentBeat, currentTime, currentBpm});

    for (const auto& raw : rawEvents) {
        // Advance time to the event's beat
        double beatDelta = raw.cumulativeBeat - currentBeat;
        if (beatDelta < 0) beatDelta = 0;
        double timeDelta = (currentBpm > 0.0) ? (beatDelta / currentBpm) * 60.0 : 0.0;
        currentTime += timeDelta;
        currentBeat = raw.cumulativeBeat;

        if (raw.isStop) {
            // STOP: advance time without advancing beat
            double stopTime = (currentBpm > 0.0) ? (raw.stopBeats / currentBpm) * 60.0 : 0.0;
            currentTime += stopTime;
            // After stop the BPM hasn't changed; add an event marking the resume point
            m_events.push_back({currentBeat, currentTime, currentBpm});
        } else {
            currentBpm = raw.bpmValue;
            m_events.push_back({currentBeat, currentTime, currentBpm});
        }
    }

    // Compute total duration (up to kMaxMeasures)
    double totalBeats = m_measureBeatStart[kMaxMeasures];
    m_totalDuration = cumulativeBeatToSeconds(totalBeats);
}

double TimingMap::cumulativeBeatToSeconds(double beat) const {
    if (m_events.empty()) return 0.0;
    // Find the last event at or before `beat`
    int idx = static_cast<int>(m_events.size()) - 1;
    while (idx > 0 && m_events[idx].absoluteBeat > beat) --idx;
    const auto& ev = m_events[idx];
    double beatDelta = beat - ev.absoluteBeat;
    if (beatDelta < 0) beatDelta = 0;
    return ev.absoluteTime + (ev.bpm > 0.0 ? (beatDelta / ev.bpm) * 60.0 : 0.0);
}

double TimingMap::measureBeatToCumulative(int measure, double beatOffset) const {
    if (measure < 0 || measure > kMaxMeasures) return 0.0;
    return m_measureBeatStart[measure] + beatOffset * 4.0;
}

double TimingMap::beatToSeconds(int measure, double beatOffset) const {
    return cumulativeBeatToSeconds(measureBeatToCumulative(measure, beatOffset));
}

} // namespace Model
