const Model::Note* ChartCanvasItem::hitTestNote(double x, double y) const {
    if (!m_document) return nullptr;
    auto* edDoc = qobject_cast<Editor::EditorDocument*>(m_document);
    if (!edDoc) return nullptr;

    int ch = xToChannel(x);
    if (ch < 0) return nullptr;

    double beat = yToBeat(y);
    const auto& doc = edDoc->document();
    for (const auto& note : doc.notes) {
        if (note.channelIndex != ch) continue;
        double noteBeat = note.measureIndex * 4.0 + note.beat.toDouble() * 4.0 * doc.measureLength(note.measureIndex);
        double ny = beatToY(noteBeat);
        if (y >= ny - 6 && y <= ny + 6) {
            return &note;
        }
    }
    return nullptr;
}
