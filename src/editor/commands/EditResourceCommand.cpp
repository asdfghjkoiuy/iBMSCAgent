#include "editor/commands/EditResourceCommand.h"
#include "editor/EditorDocument.h"
#include "model/BmsDocument.h"

namespace Editor::Commands {

EditResourceCommand::EditResourceCommand(EditorDocument* doc, ResourceType type, int slot,
                                          std::string newFilename, QUndoCommand* parent)
    : QUndoCommand(type == ResourceType::WAV ? QObject::tr("Edit WAV Slot") : QObject::tr("Edit BMP Slot"), parent)
    , m_doc(doc)
    , m_type(type)
    , m_slot(slot)
    , m_newFilename(std::move(newFilename))
    , m_oldFilename(slotRef()) {}

std::string& EditResourceCommand::slotRef() {
    auto& d = m_doc->document();
    return m_type == ResourceType::WAV ? d.wavTable[m_slot] : d.bmpTable[m_slot];
}

void EditResourceCommand::redo() { slotRef() = m_newFilename; }
void EditResourceCommand::undo() { slotRef() = m_oldFilename; }

} // namespace Editor::Commands
