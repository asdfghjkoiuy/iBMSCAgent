#pragma once
#include <QUndoCommand>

namespace Editor { class EditorDocument; }

namespace Editor::Commands {

class EditBpmCommand : public QUndoCommand {
public:
    /// Edit a named BPM table entry (slot 1-1295).
    EditBpmCommand(EditorDocument* doc, int slot, double newBpm, QUndoCommand* parent = nullptr);
    void redo() override;
    void undo() override;

private:
    EditorDocument* m_doc;
    int m_slot;
    double m_newBpm;
    double m_oldBpm;
};

class EditStopCommand : public QUndoCommand {
public:
    /// Edit a STOP table entry (slot 1-1295). Value in beats.
    EditStopCommand(EditorDocument* doc, int slot, double newStopBeats, QUndoCommand* parent = nullptr);
    void redo() override;
    void undo() override;

private:
    EditorDocument* m_doc;
    int m_slot;
    double m_newStopBeats;
    double m_oldStopBeats;
};

} // namespace Editor::Commands
