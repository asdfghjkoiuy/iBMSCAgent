#pragma once
#include "model/Note.h"
#include <QUndoCommand>
#include <vector>

namespace Editor { class EditorDocument; }

namespace Editor::Commands {

/// Batch-set the value (WAV slot label) on all selected notes.
class ModifyLabelsCommand : public QUndoCommand {
public:
    ModifyLabelsCommand(EditorDocument* doc, int newValue, QUndoCommand* parent = nullptr);
    void redo() override;
    void undo() override;
private:
    EditorDocument* m_doc;
    int m_newValue;
    std::vector<Model::Note> m_oldNotes;
};

} // namespace Editor::Commands
