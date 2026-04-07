#pragma once
#include "model/Note.h"
#include <QUndoCommand>
#include <vector>

namespace Editor { class EditorDocument; }

namespace Editor::Commands {

/// Convert selected notes: short→long, long→short, or toggle.
class ConvertLongShortCommand : public QUndoCommand {
public:
    enum Mode { ToLong, ToShort, Toggle };
    ConvertLongShortCommand(EditorDocument* doc, Mode mode, double defaultLength = 1.0, QUndoCommand* parent = nullptr);
    void redo() override;
    void undo() override;
private:
    EditorDocument* m_doc;
    Mode m_mode;
    double m_defaultLength;
    std::vector<Model::Note> m_oldNotes;
};

} // namespace Editor::Commands
