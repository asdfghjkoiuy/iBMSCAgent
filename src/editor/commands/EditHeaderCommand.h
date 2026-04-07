#pragma once
#include <QUndoCommand>
#include <string>

namespace Editor { class EditorDocument; }

namespace Editor::Commands {

class EditHeaderCommand : public QUndoCommand {
public:
    EditHeaderCommand(EditorDocument* doc, std::string key, std::string newValue, QUndoCommand* parent = nullptr);
    void redo() override;
    void undo() override;

private:
    EditorDocument* m_doc;
    std::string m_key;
    std::string m_newValue;
    std::string m_oldValue;
};

} // namespace Editor::Commands
