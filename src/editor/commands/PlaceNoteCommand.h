#pragma once
#include "model/Note.h"
#include <QUndoCommand>

namespace Editor { class EditorDocument; }

namespace Editor::Commands {

class PlaceNoteCommand : public QUndoCommand {
public:
    PlaceNoteCommand(EditorDocument* doc, Model::Note note, QUndoCommand* parent = nullptr);
    void redo() override;
    void undo() override;

private:
    EditorDocument* m_doc;
    Model::Note m_note;
};

} // namespace Editor::Commands
