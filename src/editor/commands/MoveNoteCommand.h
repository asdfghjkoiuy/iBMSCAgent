#pragma once
#include "model/Note.h"
#include <QUndoCommand>

namespace Editor { class EditorDocument; }

namespace Editor::Commands {

class MoveNoteCommand : public QUndoCommand {
public:
    MoveNoteCommand(EditorDocument* doc, Model::Note before, Model::Note after, QUndoCommand* parent = nullptr);
    void redo() override;
    void undo() override;

private:
    void apply(const Model::Note& from, const Model::Note& to);

    EditorDocument* m_doc;
    Model::Note m_before;
    Model::Note m_after;
};

} // namespace Editor::Commands
