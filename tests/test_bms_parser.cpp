#include "io/BmsParser.h"
#include <cassert>
#include <iostream>
#include <QTemporaryFile>
#include <QTextStream>

void test_parse_header() {
    QTemporaryFile f;
    (void)f.open();
    QTextStream ts(&f);
    ts << "#TITLE My Test Song\n";
    ts << "#BPM 140\n";
    ts << "#WAV01 kick.wav\n";
    ts.flush();
    f.seek(0);

    auto result = IO::BmsParser::parse(f);
    assert(result.doc.header("TITLE") == "My Test Song");
    assert(result.doc.header("BPM") == "140");
    assert(result.doc.wavTable[1] == "kick.wav");
    std::cout << "test_parse_header: PASS\n";
}

void test_parse_channel_row() {
    QTemporaryFile f;
    (void)f.open();
    QTextStream ts(&f);
    ts << "#BPM 120\n";
    ts << "#00111:0100\n"; // measure 1, channel "11" (base-36 = 37), 2 slots: slot 01 at beat 0/2
    ts.flush();
    f.seek(0);

    auto result = IO::BmsParser::parse(f);
    // Should have one note at measure 1, channel 11 (decimal for 0x11=17), beat 0/2
    bool found = false;
    for (const auto& n : result.doc.notes) {
        // "11" in base-36 = 1*36+1 = 37
        if (n.channelIndex == 37 && n.measureIndex == 1 && n.value == 1) {
            found = true;
            break;
        }
    }
    assert(found);
    std::cout << "test_parse_channel_row: PASS\n";
}

// Called from test_main.cpp
