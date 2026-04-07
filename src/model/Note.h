#pragma once
#include <cstdint>

namespace Model {

/// Beat position within a measure as a rational number.
/// Represents `num / den` beats from the start of the measure (0 = start).
/// Using integers avoids floating-point drift accumulation.
struct BeatFraction {
    int32_t num = 0; // numerator
    int32_t den = 1; // denominator (always > 0)

    double toDouble() const { return static_cast<double>(num) / den; }

    bool operator<(const BeatFraction& o) const { return num * o.den < o.num * den; }
    bool operator==(const BeatFraction& o) const { return num * o.den == o.num * den; }
    bool operator<=(const BeatFraction& o) const { return !(o < *this); }
};

/// A single note event in the chart.
struct Note {
    int channelIndex = 0;   ///< BMS channel (decimal, e.g. 11 = P1 key 1)
    int measureIndex = 0;   ///< 0-based measure number
    BeatFraction beat;      ///< Position within the measure [0, measureLength)
    int value = 0;          ///< Resource slot index (1-1295), or encoded BPM/STOP value
    double durationInBeats = 0.0; ///< 0.0 = short note, > 0.0 = long note length in beats
    bool selected = false;  ///< Editor-only: whether this note is currently selected
    bool hidden = false;    ///< Whether this note is on a hidden channel (31-39/41-49)
    bool landmine = false;  ///< Whether this note is a landmine (D1-D9/E1-E9)
};

} // namespace Model
