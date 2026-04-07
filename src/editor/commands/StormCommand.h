#pragma once
#include "model/Note.h"
#include <QUndoCommand>
#include <vector>

namespace Editor { class EditorDocument; }

namespace Editor::Commands {

/// Randomly shuffle column assignments of selected notes, preserving beat positions.
class StormCommand : public QUndoCommand {
public:
    StormCommand(EditorDocument* doc, QUndoCommand* parent = nullptr);
    void redo() override;
    void undo() override;
private:
    EditorDocument* m_doc;
    std::vector<Model::Note> m_oldNotes;
};

} // namespace Editor::Commands
