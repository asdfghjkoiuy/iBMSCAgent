#include "editor/commands/EditHeaderCommand.h"
#include "editor/EditorDocument.h"

namespace Editor::Commands {

EditHeaderCommand::EditHeaderCommand(EditorDocument* doc, std::string key, std::string newValue, QUndoCommand* parent)
    : QUndoCommand(QObject::tr("Edit Header"), parent)
    , m_doc(doc)
    , m_key(std::move(key))
    , m_newValue(std::move(newValue))
    , m_oldValue(doc->document().header(m_key)) {}

void EditHeaderCommand::redo() {
    m_doc->document().headers[m_key] = m_newValue;
}

void EditHeaderCommand::undo() {
    if (m_oldValue.empty())
        m_doc->document().headers.erase(m_key);
    else
        m_doc->document().headers[m_key] = m_oldValue;
}

} // namespace Editor::Commands
