#include "editor/commands/MoveNotesCommand.h"
#include "editor/EditorDocument.h"
#include <algorithm>

namespace Editor::Commands {

MoveNotesCommand::MoveNotesCommand(EditorDocument* doc, std::vector<MovedNote> moves, QUndoCommand* parent)
    : QUndoCommand(QObject::tr("Move Notes"), parent)
    , m_doc(doc)
    , m_moves(std::move(moves)) {}

void MoveNotesCommand::apply(bool forward) {
    auto& notes = m_doc->document().notes;
    for (const auto& mv : m_moves) {
        const Model::Note& from = forward ? mv.before : mv.after;
        const Model::Note& to   = forward ? mv.after  : mv.before;
        for (auto& n : notes) {
            if (n.channelIndex == from.channelIndex
             && n.measureIndex == from.measureIndex
             && n.beat == from.beat
             && n.value == from.value) {
                n.channelIndex = to.channelIndex;
                n.measureIndex = to.measureIndex;
                n.beat = to.beat;
                break;
            }
        }
    }
}

void MoveNotesCommand::redo() { apply(true); }
void MoveNotesCommand::undo() { apply(false); }

} // namespace Editor::Commands
