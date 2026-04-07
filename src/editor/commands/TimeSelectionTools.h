#pragma once
#include "model/Note.h"
#include <QUndoCommand>
#include <vector>

namespace Editor { class EditorDocument; }

namespace Editor::Commands {

/// Scale note positions within a time range by a ratio.
class ExpandByRatioCommand : public QUndoCommand {
public:
    ExpandByRatioCommand(EditorDocument* doc, double startBeat, double endBeat, double ratio, QUndoCommand* parent = nullptr);
    void redo() override;
    void undo() override;
private:
    EditorDocument* m_doc;
    double m_startBeat, m_endBeat, m_ratio;
    std::vector<Model::Note> m_oldNotes;
};

/// Reverse note positions within a time range.
class ReverseSelectionCommand : public QUndoCommand {
public:
    ReverseSelectionCommand(EditorDocument* doc, double startBeat, double endBeat, QUndoCommand* parent = nullptr);
    void redo() override;
    void undo() override;
private:
    EditorDocument* m_doc;
    double m_startBeat, m_endBeat;
    std::vector<Model::Note> m_oldNotes;
};

} // namespace Editor::Commands
