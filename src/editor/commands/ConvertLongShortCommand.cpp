#include "editor/commands/ConvertLongShortCommand.h"
#include "editor/EditorDocument.h"

namespace Editor::Commands {

ConvertLongShortCommand::ConvertLongShortCommand(EditorDocument* doc, Mode mode, double defaultLength, QUndoCommand* parent)
    : QUndoCommand("Convert Long/Short", parent), m_doc(doc), m_mode(mode), m_defaultLength(defaultLength) {}

void ConvertLongShortCommand::redo() {
    auto& notes = m_doc->document().notes;
    m_oldNotes = notes;
    for (auto& n : notes) {
        if (!n.selected) continue;
        switch (m_mode) {
            case ToLong:
                if (n.durationInBeats <= 0.0) n.durationInBeats = m_defaultLength;
                break;
            case ToShort:
                n.durationInBeats = 0.0;
                break;
            case Toggle:
                n.durationInBeats = (n.durationInBeats > 0.0) ? 0.0 : m_defaultLength;
                break;
        }
    }
    emit m_doc->documentChanged();
}

void ConvertLongShortCommand::undo() {
    m_doc->document().notes = m_oldNotes;
    emit m_doc->documentChanged();
}

} // namespace Editor::Commands
