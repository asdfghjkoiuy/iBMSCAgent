#include "editor/commands/ModifyLabelsCommand.h"
#include "editor/EditorDocument.h"

namespace Editor::Commands {

ModifyLabelsCommand::ModifyLabelsCommand(EditorDocument* doc, int newValue, QUndoCommand* parent)
    : QUndoCommand("Modify Labels", parent), m_doc(doc), m_newValue(newValue) {}

void ModifyLabelsCommand::redo() {
    auto& notes = m_doc->document().notes;
    m_oldNotes = notes;
    for (auto& n : notes) {
        if (n.selected) n.value = m_newValue;
    }
    emit m_doc->documentChanged();
}

void ModifyLabelsCommand::undo() {
    m_doc->document().notes = m_oldNotes;
    emit m_doc->documentChanged();
}

} // namespace Editor::Commands
