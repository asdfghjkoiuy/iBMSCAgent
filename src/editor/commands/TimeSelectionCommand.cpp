#include "editor/commands/TimeSelectionCommand.h"
#include "editor/EditorDocument.h"
#include <algorithm>

namespace Editor::Commands {

TimeSelectionCommand::TimeSelectionCommand(EditorDocument* doc, int startMeasure, int count, QUndoCommand* parent)
    : QUndoCommand(QObject::tr("Insert Measure"), parent)
    , m_doc(doc)
    , m_startMeasure(startMeasure)
    , m_count(count) {}

void TimeSelectionCommand::shift(int delta) {
    for (auto& n : m_doc->document().notes) {
        if (n.measureIndex >= m_startMeasure) {
            n.measureIndex = std::max(0, n.measureIndex + delta);
        }
    }
}

void TimeSelectionCommand::redo() {
    shift(m_count);
}

void TimeSelectionCommand::undo() {
    shift(-m_count);
}

} // namespace Editor::Commands
