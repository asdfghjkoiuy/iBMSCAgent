#include "core/undo/commands.h"

namespace ibmsc {

SnapshotCommand::SnapshotCommand(BmsDocument before, BmsDocument after)
    : m_before(std::move(before)), m_after(std::move(after)) {}

void SnapshotCommand::apply(BmsDocument& doc) {
    doc = m_after;
}

void SnapshotCommand::revert(BmsDocument& doc) {
    doc = m_before;
}

void UndoStack::push(std::unique_ptr<IUndoCommand> command) {
    if (!command) {
        return;
    }
    m_undo.push_back(std::move(command));
    m_redo.clear();
}

bool UndoStack::canUndo() const {
    return !m_undo.empty();
}

bool UndoStack::canRedo() const {
    return !m_redo.empty();
}

void UndoStack::undo(BmsDocument& doc) {
    if (m_undo.empty()) {
        return;
    }
    auto command = std::move(m_undo.back());
    m_undo.pop_back();
    command->revert(doc);
    m_redo.push_back(std::move(command));
}

void UndoStack::redo(BmsDocument& doc) {
    if (m_redo.empty()) {
        return;
    }
    auto command = std::move(m_redo.back());
    m_redo.pop_back();
    command->apply(doc);
    m_undo.push_back(std::move(command));
}

void UndoStack::clear() {
    m_undo.clear();
    m_redo.clear();
}

} // namespace ibmsc
