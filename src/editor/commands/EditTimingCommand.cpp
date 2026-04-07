#include "editor/commands/EditTimingCommand.h"
#include "editor/EditorDocument.h"

namespace Editor::Commands {

EditBpmCommand::EditBpmCommand(EditorDocument* doc, int slot, double newBpm, QUndoCommand* parent)
    : QUndoCommand(QObject::tr("Edit BPM"), parent)
    , m_doc(doc)
    , m_slot(slot)
    , m_newBpm(newBpm)
    , m_oldBpm(doc->document().bpmTable[slot].bpm) {}

void EditBpmCommand::redo() { m_doc->document().bpmTable[m_slot].bpm = m_newBpm; emit m_doc->documentChanged(); }
void EditBpmCommand::undo() { m_doc->document().bpmTable[m_slot].bpm = m_oldBpm; emit m_doc->documentChanged(); }

EditStopCommand::EditStopCommand(EditorDocument* doc, int slot, double newStopBeats, QUndoCommand* parent)
    : QUndoCommand(QObject::tr("Edit Stop"), parent)
    , m_doc(doc)
    , m_slot(slot)
    , m_newStopBeats(newStopBeats)
    , m_oldStopBeats(doc->document().stopTable[slot].stopBeats) {}

void EditStopCommand::redo() { m_doc->document().stopTable[m_slot].stopBeats = m_newStopBeats; emit m_doc->documentChanged(); }
void EditStopCommand::undo() { m_doc->document().stopTable[m_slot].stopBeats = m_oldStopBeats; emit m_doc->documentChanged(); }

} // namespace Editor::Commands
