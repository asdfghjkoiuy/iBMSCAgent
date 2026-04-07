#include "editor/commands/RemoveNoteCommand.h"
#include "editor/EditorDocument.h"
#include <algorithm>

namespace Editor::Commands {

RemoveNoteCommand::RemoveNoteCommand(EditorDocument* doc, Model::Note note, QUndoCommand* parent)
    : QUndoCommand(QObject::tr("Remove Note"), parent)
    , m_doc(doc)
    , m_note(note) {}

void RemoveNoteCommand::redo() {
    auto& notes = m_doc->document().notes;
    notes.erase(std::remove_if(notes.begin(), notes.end(), [&](const Model::Note& n) {
        return n.channelIndex == m_note.channelIndex
            && n.measureIndex == m_note.measureIndex
            && n.beat == m_note.beat
            && n.value == m_note.value;
    }), notes.end());
}

void RemoveNoteCommand::undo() {
    m_doc->document().notes.push_back(m_note);
}

} // namespace Editor::Commands
