#pragma once
#include <QUndoCommand>

namespace Editor { class EditorDocument; }

namespace Editor::Commands {

class TimeSelectionCommand : public QUndoCommand {
public:
    TimeSelectionCommand(EditorDocument* doc, int startMeasure, int count, QUndoCommand* parent = nullptr);
    void redo() override;
    void undo() override;

private:
    void shift(int delta);

    EditorDocument* m_doc;
    int m_startMeasure;
    int m_count;
};

} // namespace Editor::Commands
