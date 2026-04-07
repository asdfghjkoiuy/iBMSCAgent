#pragma once
#include "model/BmsDocument.h"
#include "model/TimingMap.h"
#include "model/ChannelType.h"
#include <vector>
#include <string>

namespace Player {

struct PlayEvent {
    double timeSec = 0.0;   // absolute trigger time in seconds
    int slotIndex = 0;      // WAV slot to play
    int lane = -1;          // visual lane (-1 = BGM, 0..N = player keys)
    bool isLong = false;
    double endTimeSec = 0.0;
};

struct ChartInfo {
    std::string title;
    std::string artist;
    double bpm = 130.0;
    int playerChannels = 7;
    bool hasTurntable = true;
};

struct PlayerChart {
    ChartInfo info;
    std::vector<PlayEvent> events; // sorted by timeSec
    double totalDuration = 0.0;

    static PlayerChart build(const Model::BmsDocument& doc, int startMeasure = 0);
};

} // namespace Player
