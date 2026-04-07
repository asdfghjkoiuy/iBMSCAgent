#pragma once
#include "model/BmsDocument.h"
#include "model/Note.h"
#include <cmath>
#include <algorithm>

namespace Editor {

inline double noteAbsoluteBeat(const Model::BmsDocument& doc, const Model::Note& note) {
    double beat = 0.0;
    for (int m = 0; m < note.measureIndex; ++m)
        beat += doc.measureLength(m) * 4.0;
    beat += note.beat.toDouble() * doc.measureLength(note.measureIndex) * 4.0;
    return beat;
}

inline void absoluteBeatToPosition(const Model::BmsDocument& doc, double absBeat,
                                   int& outMeasure, Model::BeatFraction& outBeat) {
    outMeasure = 0;
    double remain = std::max(0.0, absBeat);
    while (true) {
        double len = doc.measureLength(outMeasure) * 4.0;
        if (remain < len - 1e-9) break;
        remain -= len;
        ++outMeasure;
    }
    const int den = 192;
    double measLen = doc.measureLength(outMeasure) * 4.0;
    double norm = measLen > 1e-12 ? (remain / measLen) : 0.0;
    int num = static_cast<int>(std::round(norm * den));
    if (num >= den) { ++outMeasure; num = 0; }
    outBeat = {num, den};
}

} // namespace Editor
