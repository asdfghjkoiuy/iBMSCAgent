#pragma once
#include <QUndoCommand>

namespace Editor {
class EditorDocument;

namespace Commands {

class SetMeasureLengthCommand : public QUndoCommand {
public:
    SetMeasureLengthCommand(EditorDocument* doc, int measureIndex, double newLength, QUndoCommand* parent = nullptr);
    void redo() override;
    void undo() override;

private:
    EditorDocument* m_doc;
    int m_measureIndex;
    double m_newLength;
    double m_oldLength;
    bool m_hadOldEntry;
};

} // namespace Commands
} // namespace Editor
