#include "editor/commands/StormCommand.h"
#include "editor/EditorDocument.h"
#include <algorithm>
#include <random>

namespace Editor::Commands {

StormCommand::StormCommand(EditorDocument* doc, QUndoCommand* parent)
    : QUndoCommand("Storm", parent), m_doc(doc) {}

void StormCommand::redo() {
    auto& notes = m_doc->document().notes;
    m_oldNotes = notes;

    // Collect channels used by selected notes
    std::vector<int> channels;
    std::vector<size_t> indices;
    for (size_t i = 0; i < notes.size(); ++i) {
        if (notes[i].selected) {
            channels.push_back(notes[i].channelIndex);
            indices.push_back(i);
        }
    }
    if (channels.size() < 2) return;

    // Shuffle channels
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(channels.begin(), channels.end(), gen);

    // Reassign
    for (size_t i = 0; i < indices.size(); ++i) {
        notes[indices[i]].channelIndex = channels[i];
    }
    emit m_doc->documentChanged();
}

void StormCommand::undo() {
    m_doc->document().notes = m_oldNotes;
    emit m_doc->documentChanged();
}

} // namespace Editor::Commands
