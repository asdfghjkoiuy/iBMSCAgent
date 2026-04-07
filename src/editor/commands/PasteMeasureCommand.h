#pragma once
#include <QUndoCommand>

namespace Editor { class EditorDocument; }

namespace Editor::Commands {

class PasteMeasureCommand : public QUndoCommand {
public:
    PasteMeasureCommand(EditorDocument* doc, int targetMeasure, QUndoCommand* parent = nullptr);
    void redo() override;
    void undo() override;

private:
    EditorDocument* m_doc;
    int m_targetMeasure;
    int m_span;
    int m_insertedCount = 0;
};

} // namespace Editor::Commands
