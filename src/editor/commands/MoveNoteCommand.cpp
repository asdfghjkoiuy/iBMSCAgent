#include "editor/commands/MoveNoteCommand.h"
#include "editor/EditorDocument.h"
#include <utility>

namespace Editor::Commands {

MoveNoteCommand::MoveNoteCommand(EditorDocument* doc, Model::Note before, Model::Note after, QUndoCommand* parent)
    : QUndoCommand(QObject::tr("Move Note"), parent)
    , m_doc(doc)
    , m_before(std::move(before))
    , m_after(std::move(after)) {}

void MoveNoteCommand::apply(const Model::Note& from, const Model::Note& to) {
    auto& notes = m_doc->document().notes;
    for (auto& n : notes) {
        if (n.channelIndex == from.channelIndex
         && n.measureIndex == from.measureIndex
         && n.beat == from.beat
         && n.value == from.value) {
            n.channelIndex = to.channelIndex;
            n.measureIndex = to.measureIndex;
            n.beat = to.beat;
            n.durationInBeats = to.durationInBeats;
            return;
        }
    }
}

void MoveNoteCommand::redo() {
    apply(m_before, m_after);
}

void MoveNoteCommand::undo() {
    apply(m_after, m_before);
}

} // namespace Editor::Commands
