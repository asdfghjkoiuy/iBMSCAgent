#include "editor/commands/PasteMeasureCommand.h"
#include "editor/EditorDocument.h"

namespace Editor::Commands {

PasteMeasureCommand::PasteMeasureCommand(EditorDocument* doc, int targetMeasure, QUndoCommand* parent)
    : QUndoCommand(QObject::tr("Paste Measure"), parent)
    , m_doc(doc)
    , m_targetMeasure(targetMeasure)
    , m_span(doc->measureClipboardSpan()) {}

void PasteMeasureCommand::redo() {
    if (m_span <= 0) return;

    auto& notes = m_doc->document().notes;
    for (auto& n : notes) {
        if (n.measureIndex >= m_targetMeasure) {
            n.measureIndex += m_span;
        }
    }

    m_insertedCount = 0;
    for (const auto& rel : m_doc->measureClipboardNotes()) {
        Model::Note n = rel;
        n.measureIndex += m_targetMeasure;
        n.selected = false;
        notes.push_back(n);
        ++m_insertedCount;
    }
}

void PasteMeasureCommand::undo() {
    if (m_span <= 0) return;

    auto& notes = m_doc->document().notes;

    // Remove the inserted notes from the end (they were appended by redo)
    if (m_insertedCount > 0 && static_cast<int>(notes.size()) >= m_insertedCount)
        notes.erase(notes.end() - m_insertedCount, notes.end());

    for (auto& n : notes) {
        if (n.measureIndex >= m_targetMeasure + m_span) {
            n.measureIndex -= m_span;
        }
    }
}

} // namespace Editor::Commands
