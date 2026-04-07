#pragma once
#include "model/Note.h"
#include <QUndoCommand>
#include <vector>

namespace Editor { class EditorDocument; }

namespace Editor::Commands {

/// Mirror selected notes across the column axis (A1↔A7, A2↔A6, A3↔A5, A4 stays).
class MirrorNotesCommand : public QUndoCommand {
public:
    MirrorNotesCommand(EditorDocument* doc, QUndoCommand* parent = nullptr);
    void redo() override;
    void undo() override;
private:
    EditorDocument* m_doc;
    std::vector<Model::Note> m_oldNotes;
};

} // namespace Editor::Commands
