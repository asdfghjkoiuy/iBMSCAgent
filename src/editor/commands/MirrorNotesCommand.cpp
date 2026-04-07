#include "editor/commands/MirrorNotesCommand.h"
#include "editor/EditorDocument.h"
#include <unordered_map>

namespace Editor::Commands {

// Mirror mapping using base-36 decoded channel values:
// A1="16"=42 ↔ A7="18"=44, A2="11"=37 ↔ A6="15"=41, A3="12"=38 ↔ A5="14"=40
// A4="13"=39 stays
// D1="21"=73 ↔ D7="29"=81, D2="22"=74 ↔ D6="28"=80, D3="23"=75 ↔ D5="25"=77
// D4="24"=76 stays
static const std::unordered_map<int, int> kMirrorMap = {
    {42, 44}, {44, 42},  // A1 ↔ A7
    {37, 41}, {41, 37},  // A2 ↔ A6
    {38, 40}, {40, 38},  // A3 ↔ A5
    // A4 (39) stays
    {73, 81}, {81, 73},  // D1 ↔ D7
    {74, 80}, {80, 74},  // D2 ↔ D6
    {75, 77}, {77, 75},  // D3 ↔ D5
    // D4 (76) stays
};

MirrorNotesCommand::MirrorNotesCommand(EditorDocument* doc, QUndoCommand* parent)
    : QUndoCommand("Mirror Notes", parent), m_doc(doc) {}

void MirrorNotesCommand::redo() {
    auto& notes = m_doc->document().notes;
    m_oldNotes = notes;
    for (auto& n : notes) {
        if (!n.selected) continue;
        auto it = kMirrorMap.find(n.channelIndex);
        if (it != kMirrorMap.end())
            n.channelIndex = it->second;
    }
    emit m_doc->documentChanged();
}

void MirrorNotesCommand::undo() {
    m_doc->document().notes = m_oldNotes;
    emit m_doc->documentChanged();
}

} // namespace Editor::Commands
