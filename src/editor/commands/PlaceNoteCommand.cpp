#include "editor/commands/PlaceNoteCommand.h"
#include "editor/EditorDocument.h"
#include <algorithm>

namespace Editor::Commands {

PlaceNoteCommand::PlaceNoteCommand(EditorDocument* doc, Model::Note note, QUndoCommand* parent)
    : QUndoCommand(QObject::tr("Place Note"), parent)
    , m_doc(doc)
    , m_note(note) {}

void PlaceNoteCommand::redo() {
    m_doc->document().notes.push_back(m_note);
}

void PlaceNoteCommand::undo() {
    auto& notes = m_doc->document().notes;
    // Remove the note that matches channel, measure, beat, value
    notes.erase(std::remove_if(notes.begin(), notes.end(), [&](const Model::Note& n) {
        return n.channelIndex == m_note.channelIndex
            && n.measureIndex == m_note.measureIndex
            && n.beat == m_note.beat
            && n.value == m_note.value;
    }), notes.end());
}

} // namespace Editor::Commands
