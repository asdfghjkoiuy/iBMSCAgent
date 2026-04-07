#include "editor/commands/SetMeasureLengthCommand.h"
#include "editor/EditorDocument.h"
#include <cmath>

namespace Editor::Commands {

SetMeasureLengthCommand::SetMeasureLengthCommand(EditorDocument* doc, int measureIndex, double newLength, QUndoCommand* parent)
    : QUndoCommand(QObject::tr("Set Measure Length"), parent)
    , m_doc(doc)
    , m_measureIndex(measureIndex)
    , m_newLength(newLength)
{
    auto& lengths = doc->document().measureLengths;
    auto it = lengths.find(measureIndex);
    m_hadOldEntry = (it != lengths.end());
    m_oldLength = m_hadOldEntry ? it->second : 1.0;
}

void SetMeasureLengthCommand::redo() {
    auto& lengths = m_doc->document().measureLengths;
    if (std::fabs(m_newLength - 1.0) < 1e-9)
        lengths.erase(m_measureIndex);
    else
        lengths[m_measureIndex] = m_newLength;
    emit m_doc->documentChanged();
}

void SetMeasureLengthCommand::undo() {
    auto& lengths = m_doc->document().measureLengths;
    if (!m_hadOldEntry)
        lengths.erase(m_measureIndex);
    else
        lengths[m_measureIndex] = m_oldLength;
    emit m_doc->documentChanged();
}

} // namespace Editor::Commands
