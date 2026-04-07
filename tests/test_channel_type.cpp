#include "model/ChannelType.h"
#include <cassert>
#include <iostream>

void test_channel_type() {
    using namespace Model;

    assert(classifyChannel(1) == ChannelType::BGM);   // "01"
    assert(classifyChannel(3) == ChannelType::BPM);   // "03"
    assert(classifyChannel(4) == ChannelType::BGA);   // "04"
    assert(classifyChannel(9) == ChannelType::Stop);  // "09"

    // P1 normal notes: base-36 "11"-"19" = 37-45
    for (int ch = 37; ch <= 45; ++ch)
        assert(classifyChannel(ch) == ChannelType::NoteNormal);
    // P2 normal notes: base-36 "21"-"29" = 73-81
    for (int ch = 73; ch <= 81; ++ch)
        assert(classifyChannel(ch) == ChannelType::NoteNormal);
    // P1 long notes: base-36 "51"-"59" = 181-189
    for (int ch = 181; ch <= 189; ++ch)
        assert(classifyChannel(ch) == ChannelType::NoteLong);

    assert(isPlayerNoteChannel(ChannelType::NoteNormal));
    assert(isPlayerNoteChannel(ChannelType::NoteLong));
    assert(!isPlayerNoteChannel(ChannelType::BGM));

    std::cout << "test_channel_type: PASS\n";
}

// Called from test_main.cpp
