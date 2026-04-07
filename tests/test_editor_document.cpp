#include "editor/EditorDocument.h"
#include <cassert>
#include <iostream>

void test_selected_note_info() {
    Editor::EditorDocument doc;
    Model::BmsDocument model;

    model.wavTable[1] = "kick.wav";

    Model::Note note;
    note.channelIndex = 37;
    note.measureIndex = 2;
    note.beat = {48, 192};
    note.value = 1;
    note.durationInBeats = 1.5;
    note.selected = true;
    note.hidden = true;
    model.notes.push_back(note);

    doc.resetDocument(std::move(model), "/tmp/test-chart/test.bms");

    QVariantMap info = doc.selectedNoteInfo();
    assert(info.value("hasSelection").toBool());
    assert(info.value("selectedCount").toInt() == 1);
    assert(info.value("slotIndex").toInt() == 1);
    assert(info.value("slotLabel").toString() == "01");
    assert(info.value("filename").toString() == "kick.wav");
    assert(info.value("displayFilename").toString() == "kick.wav");
    assert(info.value("measureIndex").toInt() == 2);
    assert(info.value("isLong").toBool());
    assert(info.value("isHidden").toBool());
    assert(!info.value("hasError").toBool());

    std::cout << "test_selected_note_info: PASS\n";
}
