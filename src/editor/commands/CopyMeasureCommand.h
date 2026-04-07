#pragma once
#include <QUndoCommand>

namespace Editor { class EditorDocument; }

namespace Editor::Commands {

class CopyMeasureCommand : public QUndoCommand {
public:
    CopyMeasureCommand(EditorDocument* doc, int startMeasure, int count, QUndoCommand* parent = nullptr);
    void redo() override;
    void undo() override;

private:
    EditorDocument* m_doc;
    int m_startMeasure;
    int m_count;
};

} // namespace Editor::Commands
