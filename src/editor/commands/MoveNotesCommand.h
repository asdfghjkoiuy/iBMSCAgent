#pragma once
#include "model/Note.h"
#include <QUndoCommand>
#include <vector>

namespace Editor { class EditorDocument; }

namespace Editor::Commands {

/// Moves notes by a signed delta in beat fractions (same denominator for all).
class MoveNotesCommand : public QUndoCommand {
public:
    struct MovedNote {
        Model::Note before;
        Model::Note after;
    };

    MoveNotesCommand(EditorDocument* doc, std::vector<MovedNote> moves, QUndoCommand* parent = nullptr);
    void redo() override;
    void undo() override;

private:
    void apply(bool forward);
    EditorDocument* m_doc;
    std::vector<MovedNote> m_moves;
};

} // namespace Editor::Commands
