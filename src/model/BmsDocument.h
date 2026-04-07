#pragma once
#include "model/Note.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace Model {

constexpr int kMaxSlots = 1296; // indices 0..1295, valid slots are 1..1295

/// A named BPM table entry (from #BPMxx directive).
struct BpmEntry {
    int slot = 0;       ///< Slot index 1-1295
    double bpm = 0.0;
};

/// A STOP sequence entry (from #STOPxx directive), value in 1/192 beats.
struct StopEntry {
    int slot = 0;
    double stopBeats = 0.0; ///< Duration in beats (converted from 1/192 units)
};

/// Complete in-memory representation of a BMS file.
/// No Qt types — pure C++ value type.
struct BmsDocument {
    // ── Header fields ─────────────────────────────────────────────────────────
    std::unordered_map<std::string, std::string> headers; ///< e.g. "TITLE" → "My Song"

    // ── Resource tables ───────────────────────────────────────────────────────
    std::string wavTable[kMaxSlots]; ///< WAV resource filenames by slot index
    std::string bmpTable[kMaxSlots]; ///< BMP resource filenames by slot index

    // ── Timing tables ─────────────────────────────────────────────────────────
    BpmEntry bpmTable[kMaxSlots];   ///< Named BPM table (from #BPMxx)
    StopEntry stopTable[kMaxSlots]; ///< Stop table (from #STOPxx)

    // ── Measure properties ────────────────────────────────────────────────────
    std::unordered_map<int, double> measureLengths; ///< Overridden measure lengths (default 1.0 = 4/4)

    // ── Note list ─────────────────────────────────────────────────────────────
    std::vector<Note> notes;

    // ── Convenience accessors ─────────────────────────────────────────────────
    double measureLength(int measure) const;
    std::string header(const std::string& key, const std::string& defaultVal = "") const;
    double initialBpm() const; ///< Returns the value of #BPM header, or 130.0 if absent
};

} // namespace Model
