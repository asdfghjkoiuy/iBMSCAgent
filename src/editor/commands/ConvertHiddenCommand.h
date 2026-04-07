#pragma once
#include "model/Note.h"
#include <QUndoCommand>
#include <vector>

namespace Editor { class EditorDocument; }

namespace Editor::Commands {

/// Convert selected notes: visible→hidden, hidden→visible, or toggle.
class ConvertHiddenCommand : public QUndoCommand {
public:
    enum Mode { ToHidden, ToVisible, Toggle };
    ConvertHiddenCommand(EditorDocument* doc, Mode mode, QUndoCommand* parent = nullptr);
    void redo() override;
    void undo() override;
private:
    EditorDocument* m_doc;
    Mode m_mode;
    std::vector<Model::Note> m_oldNotes;
};

} // namespace Editor::Commands
