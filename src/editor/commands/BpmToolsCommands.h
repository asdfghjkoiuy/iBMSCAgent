#pragma once
#include "model/Note.h"
#include "model/BmsDocument.h"
#include <QUndoCommand>
#include <unordered_map>
#include <vector>

namespace Editor { class EditorDocument; }

namespace Editor::Commands {

class ConstBpmCommand : public QUndoCommand {
public:
    ConstBpmCommand(EditorDocument* doc, double targetBpm, QUndoCommand* parent = nullptr);
    void redo() override;
    void undo() override;

private:
    EditorDocument* m_doc;
    double m_targetBpm;
    std::vector<Model::Note> m_oldNotes;
    std::unordered_map<std::string, std::string> m_oldHeaders;
    std::vector<Model::Note> m_newNotes;
    std::unordered_map<std::string, std::string> m_newHeaders;
};

class ScaleBpmCommand : public QUndoCommand {
public:
    ScaleBpmCommand(EditorDocument* doc, double ratio, QUndoCommand* parent = nullptr);
    void redo() override;
    void undo() override;

private:
    EditorDocument* m_doc;
    double m_ratio;
    std::vector<Model::Note> m_oldNotes;
    std::unordered_map<std::string, std::string> m_oldHeaders;
    std::vector<Model::Note> m_newNotes;
    std::unordered_map<std::string, std::string> m_newHeaders;
};

} // namespace Editor::Commands
