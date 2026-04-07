#include "editor/commands/DeleteMeasureCommand.h"
#include "editor/EditorDocument.h"

namespace Editor::Commands {

DeleteMeasureCommand::DeleteMeasureCommand(EditorDocument* doc, int startMeasure, int count, QUndoCommand* parent)
    : QUndoCommand(QObject::tr("Delete Measure"), parent)
    , m_doc(doc)
    , m_startMeasure(startMeasure)
    , m_count(count) {}

void DeleteMeasureCommand::redo() {
    auto& notes = m_doc->document().notes;
    m_removedNotes.clear();

    const int endMeasure = m_startMeasure + m_count;
    std::vector<Model::Note> kept;
    kept.reserve(notes.size());

    for (auto note : notes) {
        if (note.measureIndex >= m_startMeasure && note.measureIndex < endMeasure) {
            m_removedNotes.push_back(note);
            continue;
        }
        if (note.measureIndex >= endMeasure) {
            note.measureIndex -= m_count;
        }
        kept.push_back(note);
    }

    notes = std::move(kept);
}

void DeleteMeasureCommand::undo() {
    auto& notes = m_doc->document().notes;
    for (auto& note : notes) {
        if (note.measureIndex >= m_startMeasure) {
            note.measureIndex += m_count;
        }
    }
    for (const auto& removed : m_removedNotes) {
        notes.push_back(removed);
    }
}

} // namespace Editor::Commands
