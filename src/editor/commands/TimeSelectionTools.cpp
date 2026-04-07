#include "editor/commands/TimeSelectionTools.h"
#include "editor/EditorDocument.h"
#include "editor/EditorUtil.h"
#include <cmath>
#include <algorithm>

namespace Editor::Commands {

using namespace Model;

// ── ExpandByRatioCommand ────────────────────────────────────────────────────

ExpandByRatioCommand::ExpandByRatioCommand(EditorDocument* doc, double startBeat, double endBeat, double ratio, QUndoCommand* parent)
    : QUndoCommand("Expand by Ratio", parent), m_doc(doc), m_startBeat(startBeat), m_endBeat(endBeat), m_ratio(ratio) {}

void ExpandByRatioCommand::redo() {
    auto& doc = m_doc->document();
    m_oldNotes = doc.notes;
    for (auto& n : doc.notes) {
        double ab = noteAbsoluteBeat(doc, n);
        if (ab < m_startBeat || ab > m_endBeat) continue;
        double rel = ab - m_startBeat;
        double newAb = m_startBeat + rel * m_ratio;
        int meas; BeatFraction bf;
        absoluteBeatToPosition(doc, newAb, meas, bf);
        n.measureIndex = meas;
        n.beat = bf;
    }
    emit m_doc->documentChanged();
}

void ExpandByRatioCommand::undo() {
    m_doc->document().notes = m_oldNotes;
    emit m_doc->documentChanged();
}

// ── ReverseSelectionCommand ─────────────────────────────────────────────────

ReverseSelectionCommand::ReverseSelectionCommand(EditorDocument* doc, double startBeat, double endBeat, QUndoCommand* parent)
    : QUndoCommand("Reverse Selection", parent), m_doc(doc), m_startBeat(startBeat), m_endBeat(endBeat) {}

void ReverseSelectionCommand::redo() {
    auto& doc = m_doc->document();
    m_oldNotes = doc.notes;
    for (auto& n : doc.notes) {
        double ab = noteAbsoluteBeat(doc, n);
        if (ab < m_startBeat || ab > m_endBeat) continue;
        double newAb = m_startBeat + (m_endBeat - ab);
        int meas; BeatFraction bf;
        absoluteBeatToPosition(doc, newAb, meas, bf);
        n.measureIndex = meas;
        n.beat = bf;
    }
    emit m_doc->documentChanged();
}

void ReverseSelectionCommand::undo() {
    m_doc->document().notes = m_oldNotes;
    emit m_doc->documentChanged();
}

} // namespace Editor::Commands
