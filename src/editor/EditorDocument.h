#pragma once
#include "model/BmsDocument.h"
#include <QObject>
#include <QUndoStack>
#include <QString>
#include <QVariantMap>
#include <QVariantList>
#include <vector>

namespace Editor {

namespace Commands {
class CopyMeasureCommand;
class PasteMeasureCommand;
}

/// Wraps a BmsDocument with an undo stack and dirty-state tracking.
/// Registered as a QML context property so QML can bind to isDirty, canUndo, canRedo.
class EditorDocument : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isDirty READ isDirty NOTIFY isDirtyChanged)
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY canUndoChanged)
    Q_PROPERTY(bool canRedo READ canRedo NOTIFY canRedoChanged)
    Q_PROPERTY(QString undoText READ undoText NOTIFY canUndoChanged)
    Q_PROPERTY(QString redoText READ redoText NOTIFY canRedoChanged)
    Q_PROPERTY(QString filePath READ filePath NOTIFY filePathChanged)
    Q_PROPERTY(QVariantMap selectedNoteInfo READ selectedNoteInfo NOTIFY selectedNoteInfoChanged)

public:
    explicit EditorDocument(QObject* parent = nullptr);

    Model::BmsDocument& document() { return m_doc; }
    const Model::BmsDocument& document() const { return m_doc; }

    QUndoStack* undoStack() { return &m_undoStack; }

    bool isDirty() const { return !m_undoStack.isClean(); }
    bool canUndo() const { return m_undoStack.canUndo(); }
    bool canRedo() const { return m_undoStack.canRedo(); }
    QString undoText() const { return m_undoStack.undoText(); }
    QString redoText() const { return m_undoStack.redoText(); }
    QString filePath() const { return m_filePath; }
    QVariantMap selectedNoteInfo() const;

    void setFilePath(const QString& path);

    /// Replace the document (e.g. after loading). Clears undo stack.
    void resetDocument(Model::BmsDocument doc, const QString& filePath = {});

    /// Marks the current stack state as clean (after save).
    void markClean();

    /// Read a header field value (for QML bindings).
    Q_INVOKABLE QString headerValue(const QString& key) const;
    /// Push an EditHeaderCommand for the given key/value change.
    Q_INVOKABLE void setHeaderValue(const QString& key, const QString& value);
    /// Push a PlaceNoteCommand from QML canvas interaction.
    Q_INVOKABLE void placeNote(int channelIndex, int measureIndex, int beatNum, int beatDen, int value);
    /// Push a PlaceNoteCommand with long-note duration support.
    Q_INVOKABLE void placeNoteWithDuration(int channelIndex, int measureIndex, int beatNum, int beatDen, int value, double durationInBeats);
    /// Push a DeleteNotesCommand for the note at the given position (if found).
    Q_INVOKABLE void deleteNote(int channelIndex, int measureIndex, int beatNum, int beatDen);
    /// Copy selected notes to system clipboard. Returns false if nothing selected.
    Q_INVOKABLE bool copySelection();
    /// Cut selected notes (copy + undoable delete). Returns false if nothing selected.
    Q_INVOKABLE bool cutSelection();
    /// Paste notes from clipboard anchored at the provided absolute beat.
    Q_INVOKABLE bool pasteFromClipboard(double anchorBeat);
    /// Insert empty measures at the given measure index.
    Q_INVOKABLE void insertEmptyMeasure(int measureIndex, int count = 1);
    /// Delete measures from the given measure index.
    Q_INVOKABLE void deleteMeasure(int measureIndex, int count = 1);
    /// Copy notes in measure range into an internal measure clipboard.
    Q_INVOKABLE void copyMeasure(int measureIndex, int count = 1);
    /// Paste previously copied measure notes at target measure.
    Q_INVOKABLE void pasteMeasure(int measureIndex);
    /// Returns a list of BPM-change and STOP events as QVariantMap {beat, label, isStop}.
    /// beat = cumulative beat (measureIndex * 4 + fraction * 4 * measureLength).
    Q_INVOKABLE QVariantList timingEvents() const;

    /// Returns note statistics as QVariantMap with counts by type.
    Q_INVOKABLE QVariantMap noteStatistics() const;

    /// Push an EditResourceCommand for WAV slot.
    Q_INVOKABLE void setWavFilename(int slot, const QString& filename);
    /// Push an EditResourceCommand for BMP slot.
    Q_INVOKABLE void setBmpFilename(int slot, const QString& filename);

    // Note conversion commands
    Q_INVOKABLE void convertToLong();
    Q_INVOKABLE void convertToShort();
    Q_INVOKABLE void toggleLongShort();
    Q_INVOKABLE void convertToHidden();
    Q_INVOKABLE void convertToVisible();
    Q_INVOKABLE void toggleHidden();
    Q_INVOKABLE void mirrorNotes();
    Q_INVOKABLE void stormNotes();
    Q_INVOKABLE void modifyLabels(int value);
    Q_INVOKABLE void expandByRatio(double startBeat, double endBeat, double ratio);
    Q_INVOKABLE void reverseSelection(double startBeat, double endBeat);
    Q_INVOKABLE void setMeasureLength(int measureIndex, double length);

public slots:
    void undo() { m_undoStack.undo(); }
    void redo() { m_undoStack.redo(); }
    void notifySelectionChanged();

signals:
    void isDirtyChanged();
    void canUndoChanged();
    void canRedoChanged();
    void filePathChanged();
    void documentChanged(); ///< Emitted after any undo command modifies the document
    void selectedNoteInfoChanged();

private slots:
    void onCleanChanged(bool clean);
    void onIndexChanged();

private:
    friend class Commands::CopyMeasureCommand;
    friend class Commands::PasteMeasureCommand;

    void setMeasureClipboard(std::vector<Model::Note> notes, int span);
    const std::vector<Model::Note>& measureClipboardNotes() const { return m_measureClipboardNotes; }
    int measureClipboardSpan() const { return m_measureClipboardSpan; }

    Model::BmsDocument m_doc;
    QUndoStack m_undoStack;
    QString m_filePath;
    std::vector<Model::Note> m_measureClipboardNotes;
    int m_measureClipboardSpan = 0;
};

} // namespace Editor
