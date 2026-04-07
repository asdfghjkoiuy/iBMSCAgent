#include "editor/commands/DeleteNotesCommand.h"
#include "editor/EditorDocument.h"
#include <algorithm>

namespace Editor::Commands {

DeleteNotesCommand::DeleteNotesCommand(EditorDocument* doc, std::vector<Model::Note> notes, QUndoCommand* parent)
    : QUndoCommand(QObject::tr("Delete Notes"), parent)
    , m_doc(doc)
    , m_notes(std::move(notes)) {}

void DeleteNotesCommand::redo() {
    auto& docNotes = m_doc->document().notes;
    for (const auto& del : m_notes) {
        docNotes.erase(std::remove_if(docNotes.begin(), docNotes.end(), [&](const Model::Note& n) {
            return n.channelIndex == del.channelIndex
                && n.measureIndex == del.measureIndex
                && n.beat == del.beat
                && n.value == del.value;
        }), docNotes.end());
    }
}

void DeleteNotesCommand::undo() {
    for (const auto& n : m_notes)
        m_doc->document().notes.push_back(n);
}

} // namespace Editor::Commands
