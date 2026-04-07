#pragma once
#include "model/Note.h"
#include <QUndoCommand>
#include <vector>

namespace Editor { class EditorDocument; }

namespace Editor::Commands {

class DeleteMeasureCommand : public QUndoCommand {
public:
    DeleteMeasureCommand(EditorDocument* doc, int startMeasure, int count, QUndoCommand* parent = nullptr);
    void redo() override;
    void undo() override;

private:
    EditorDocument* m_doc;
    int m_startMeasure;
    int m_count;
    std::vector<Model::Note> m_removedNotes;
};

} // namespace Editor::Commands
