#include "model/TimingMap.h"
#include "model/BmsDocument.h"
#include <cassert>
#include <cmath>
#include <iostream>

static bool approxEq(double a, double b, double eps = 1e-6) {
    return std::fabs(a - b) < eps;
}

void test_constant_bpm() {
    using namespace Model;
    BmsDocument doc;
    doc.headers["BPM"] = "120";
    TimingMap tm(doc);

    // At 120 BPM, one beat = 0.5s; measure 0 beat 1.0 (of 4) = 0.5s
    assert(approxEq(tm.beatToSeconds(0, 0.25), 0.5));
    // measure 1 beat 0 = 4 beats * 0.5s = 2.0s
    assert(approxEq(tm.beatToSeconds(1, 0.0), 2.0));
    std::cout << "test_constant_bpm: PASS\n";
}

void test_bpm_change() {
    using namespace Model;
    BmsDocument doc;
    doc.headers["BPM"] = "120";

    // Add a BPM change at measure 0 beat 0.5 (2 beats in) to 60 BPM
    Note n;
    n.channelIndex = 3; // BPM channel
    n.measureIndex = 0;
    n.beat = {1, 2}; // 0.5 of measure = 2 beats
    n.value = 60;    // new BPM value (hex channel uses direct value)
    doc.notes.push_back(n);

    TimingMap tm(doc);
    // First 2 beats at 120 BPM: 2 * 0.5 = 1.0s
    // Next 2 beats at 60 BPM: 2 * 1.0 = 2.0s
    // Total measure 1 start: 3.0s
    assert(approxEq(tm.beatToSeconds(1, 0.0), 3.0, 1e-4));
    std::cout << "test_bpm_change: PASS\n";
}

// Called from test_main.cpp
