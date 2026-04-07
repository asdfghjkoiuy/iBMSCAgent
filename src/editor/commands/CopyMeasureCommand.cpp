#include "editor/commands/CopyMeasureCommand.h"
#include "editor/EditorDocument.h"

namespace Editor::Commands {

CopyMeasureCommand::CopyMeasureCommand(EditorDocument* doc, int startMeasure, int count, QUndoCommand* parent)
    : QUndoCommand(QObject::tr("Copy Measure"), parent)
    , m_doc(doc)
    , m_startMeasure(startMeasure)
    , m_count(count) {}

void CopyMeasureCommand::redo() {
    std::vector<Model::Note> copied;
    const int endMeasure = m_startMeasure + m_count;
    for (const auto& note : m_doc->document().notes) {
        if (note.measureIndex >= m_startMeasure && note.measureIndex < endMeasure) {
            Model::Note rel = note;
            rel.measureIndex -= m_startMeasure;
            rel.selected = false;
            copied.push_back(rel);
        }
    }
    m_doc->setMeasureClipboard(std::move(copied), m_count);
}

void CopyMeasureCommand::undo() {
    // Copy does not mutate document state.
}

} // namespace Editor::Commands
