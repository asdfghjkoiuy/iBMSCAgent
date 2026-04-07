#pragma once
#include "model/Note.h"
#include <QUndoCommand>
#include <vector>

namespace Editor { class EditorDocument; }

namespace Editor::Commands {

class DeleteNotesCommand : public QUndoCommand {
public:
    DeleteNotesCommand(EditorDocument* doc, std::vector<Model::Note> notes, QUndoCommand* parent = nullptr);
    void redo() override;
    void undo() override;

private:
    EditorDocument* m_doc;
    std::vector<Model::Note> m_notes;
};

} // namespace Editor::Commands
