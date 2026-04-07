#pragma once
#include <QUndoCommand>
#include <string>

namespace Editor { class EditorDocument; }

namespace Editor::Commands {

enum class ResourceType { WAV, BMP };

class EditResourceCommand : public QUndoCommand {
public:
    EditResourceCommand(EditorDocument* doc, ResourceType type, int slot,
                        std::string newFilename, QUndoCommand* parent = nullptr);
    void redo() override;
    void undo() override;

private:
    std::string& slotRef();
    EditorDocument* m_doc;
    ResourceType m_type;
    int m_slot;
    std::string m_newFilename;
    std::string m_oldFilename;
};

} // namespace Editor::Commands
