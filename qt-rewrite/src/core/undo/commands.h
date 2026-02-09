#pragma once

#include "core/undo/i_undo_command.h"

#include <memory>
#include <vector>

namespace ibmsc {

class SnapshotCommand final : public IUndoCommand {
public:
    SnapshotCommand(BmsDocument before, BmsDocument after);
    void apply(BmsDocument& doc) override;
    void revert(BmsDocument& doc) override;

private:
    BmsDocument m_before;
    BmsDocument m_after;
};

class UndoStack {
public:
    void push(std::unique_ptr<IUndoCommand> command);
    bool canUndo() const;
    bool canRedo() const;
    void undo(BmsDocument& doc);
    void redo(BmsDocument& doc);
    void clear();

private:
    std::vector<std::unique_ptr<IUndoCommand>> m_undo;
    std::vector<std::unique_ptr<IUndoCommand>> m_redo;
};

} // namespace ibmsc
