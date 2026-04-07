#pragma once
#include "model/BmsDocument.h"
#include <vector>

namespace Model {

/// A BPM/STOP event for internal timing computation.
struct TimingEvent {
    double absoluteBeat = 0.0; ///< Cumulative beat count from the start of the chart
    double absoluteTime = 0.0; ///< Seconds from the start of the chart
    double bpm = 130.0;        ///< BPM in effect after this event
};

/// Pre-computed timing table built from a BmsDocument.
/// Maps (measure, beatFraction) → absolute seconds.
class TimingMap {
public:
    explicit TimingMap(const BmsDocument& doc);

    /// Returns the absolute time in seconds for a given measure and beat offset (0..measureLength).
    double beatToSeconds(int measure, double beatOffset) const;

    /// Returns the absolute time in seconds for a cumulative beat index.
    double cumulativeBeatToSeconds(double beat) const;

    /// Returns the cumulative beat index for a given measure and beat offset.
    double measureBeatToCumulative(int measure, double beatOffset) const;

    /// Total duration in seconds (up to the last note or 1000 measures).
    double totalDuration() const { return m_totalDuration; }

private:
    void build(const BmsDocument& doc);

    std::vector<TimingEvent> m_events; ///< Sorted by absoluteBeat
    double m_totalDuration = 0.0;
    // Pre-computed cumulative beat starts for each measure
    std::vector<double> m_measureBeatStart; // index = measure number
};

} // namespace Model
