#include "editor/commands/ConvertHiddenCommand.h"
#include "editor/EditorDocument.h"
#include "model/ChannelType.h"

namespace Editor::Commands {

using namespace Model;

// Channel shift: normal 11-19 ↔ hidden 31-39, normal 21-29 ↔ hidden 41-49
static int toHiddenChannel(int ch) {
    if (ch >= 37 && ch <= 45) return ch + 72;   // 11-19 (base36) → 31-39
    if (ch >= 73 && ch <= 81) return ch + 72;   // 21-29 → 41-49
    return ch;
}

static int toVisibleChannel(int ch) {
    if (ch >= 109 && ch <= 117) return ch - 72;  // 31-39 → 11-19
    if (ch >= 145 && ch <= 153) return ch - 72;  // 41-49 → 21-29
    return ch;
}

ConvertHiddenCommand::ConvertHiddenCommand(EditorDocument* doc, Mode mode, QUndoCommand* parent)
    : QUndoCommand("Convert Hidden/Visible", parent), m_doc(doc), m_mode(mode) {}

void ConvertHiddenCommand::redo() {
    auto& notes = m_doc->document().notes;
    m_oldNotes = notes;
    for (auto& n : notes) {
        if (!n.selected) continue;
        switch (m_mode) {
            case ToHidden:
                if (!n.hidden) {
                    n.channelIndex = toHiddenChannel(n.channelIndex);
                    n.hidden = true;
                }
                break;
            case ToVisible:
                if (n.hidden) {
                    n.channelIndex = toVisibleChannel(n.channelIndex);
                    n.hidden = false;
                }
                break;
            case Toggle:
                if (n.hidden) {
                    n.channelIndex = toVisibleChannel(n.channelIndex);
                    n.hidden = false;
                } else {
                    n.channelIndex = toHiddenChannel(n.channelIndex);
                    n.hidden = true;
                }
                break;
        }
    }
    emit m_doc->documentChanged();
}

void ConvertHiddenCommand::undo() {
    m_doc->document().notes = m_oldNotes;
    emit m_doc->documentChanged();
}

} // namespace Editor::Commands
