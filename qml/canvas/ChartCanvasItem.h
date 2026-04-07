#pragma once
#include <QQuickItem>
#include <QSGNode>
#include <QString>
#include <vector>
#include "model/Note.h"
#include "model/ColumnConfig.h"

namespace Editor { class EditorDocument; }

/// Cached per-frame column layout entry.
struct ColumnLayout {
    int identifier = 0;   ///< BMS channel identifier (decimal)
    double x = 0.0;       ///< Left edge in pixels
    double width = 48.0;  ///< Column width in pixels
    QString title;
    bool isSound = true;
};

/// GPU-accelerated chart canvas rendered via Qt Quick scene graph.
/// Registered as QML element "ChartCanvas".
class ChartCanvasItem : public QQuickItem {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(EditMode editMode READ editMode WRITE setEditMode NOTIFY editModeChanged)
    Q_PROPERTY(double timeSelectionStartBeat READ timeSelectionStartBeat WRITE setTimeSelectionStartBeat NOTIFY timeSelectionRangeChanged)
    Q_PROPERTY(double timeSelectionEndBeat READ timeSelectionEndBeat WRITE setTimeSelectionEndBeat NOTIFY timeSelectionRangeChanged)
    Q_PROPERTY(double scrollBeat READ scrollBeat WRITE setScrollBeat NOTIFY scrollBeatChanged)
    Q_PROPERTY(double pxPerBeat READ pxPerBeat WRITE setPxPerBeat NOTIFY pxPerBeatChanged)
    Q_PROPERTY(int gridSubdivision READ gridSubdivision WRITE setGridSubdivision NOTIFY gridSubdivisionChanged)
    Q_PROPERTY(double playbackBeat READ playbackBeat WRITE setPlaybackBeat NOTIFY playbackBeatChanged)
    Q_PROPERTY(QObject* document READ document WRITE setDocument NOTIFY documentChanged)
    Q_PROPERTY(QObject* audioEngine READ audioEngine WRITE setAudioEngine NOTIFY audioEngineChanged)
    Q_PROPERTY(bool showWaveform READ showWaveform WRITE setShowWaveform NOTIFY showWaveformChanged)

public:
    enum EditMode {
        SelectMode = 0,
        WriteMode = 1,
        EraserMode = 2,
        TimeSelectMode = 3
    };
    Q_ENUM(EditMode)

    explicit ChartCanvasItem(QQuickItem* parent = nullptr);

    EditMode editMode() const { return m_editMode; }
    void setEditMode(EditMode mode);

    double timeSelectionStartBeat() const { return m_timeSelectionStartBeat; }
    void setTimeSelectionStartBeat(double beat);

    double timeSelectionEndBeat() const { return m_timeSelectionEndBeat; }
    void setTimeSelectionEndBeat(double beat);

    double scrollBeat() const { return m_scrollBeat; }
    void setScrollBeat(double v);

    double pxPerBeat() const { return m_pxPerBeat; }
    void setPxPerBeat(double v);

    int gridSubdivision() const { return m_gridSubdivision; }
    void setGridSubdivision(int v);

    double playbackBeat() const { return m_playbackBeat; }
    void setPlaybackBeat(double v);

    QObject* document() const { return m_document; }
    void setDocument(QObject* doc);

    QObject* audioEngine() const { return m_audioEngine; }
    void setAudioEngine(QObject* engine);

    bool showWaveform() const { return m_showWaveform; }
    void setShowWaveform(bool v);

    Q_INVOKABLE int channelAt(double x) const { return xToChannel(x); }
    Q_INVOKABLE double beatAt(double y) const { return yToBeat(y); }

signals:
    void editModeChanged();
    void timeSelectionRangeChanged();
    void scrollBeatChanged();
    void pxPerBeatChanged();
    void gridSubdivisionChanged();
    void playbackBeatChanged();
    void documentChanged();
    void audioEngineChanged();
    void showWaveformChanged();
    void notePlaced(int channelIndex, int measureIndex, int beatNum, int beatDen, int value);
    void notePlacedWithDuration(int channelIndex, int measureIndex, int beatNum, int beatDen, int value, double durationInBeats);
    void noteDeleted(int channelIndex, int measureIndex, int beatNum, int beatDen);
    void noteDoubleClicked(int channelIndex, int measureIndex, int beatNum, int beatDen, int currentValue);

protected:
    QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*) override;
    void hoverMoveEvent(QHoverEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void mouseDoubleClickEvent(QMouseEvent* e) override;
    void wheelEvent(QWheelEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;
    void geometryChange(const QRectF& newGeom, const QRectF& oldGeom) override;
    bool event(QEvent* e) override;

private:
    double beatToY(double beat) const;
    double yToBeat(double y) const;
    double snapBeat(double rawBeat) const;
    int xToChannel(double x) const;
    double channelToX(int ch) const;
    std::vector<ColumnLayout> buildColumnLayout() const;
    bool isBGroupChannel(int channel) const;
    QString laneTitleForChannel(int channel) const;

    EditMode m_editMode = WriteMode;
    double m_timeSelectionStartBeat = -1.0;
    double m_timeSelectionEndBeat = -1.0;
    double m_scrollBeat = 0.0;
    double m_pxPerBeat = 80.0;
    int m_gridSubdivision = 16;
    double m_playbackBeat = -1.0;
    QObject* m_document = nullptr;
    QObject* m_audioEngine = nullptr;
    bool m_showWaveform = true;

    // Rubber-band and drag-to-move selection state
    enum DragMode {
        DragNone,
        DragPlace,
        DragDelete,
        DragSelect,
        DragMoveNotes,
        DragResizeLN
    };
    DragMode m_dragMode = DragNone;
    QPointF m_dragStart;
    QPointF m_dragCurrent;
    double m_dragStartBeat = 0;
    int m_dragStartChannel = 0;
    int m_lastEraseMeasure = -1;
    int m_lastEraseBeatNum = -1;
    int m_lastEraseBeatDen = -1;
    int m_lastEraseChannel = -1;

    // Hover state
    double m_hoverX = -1.0;
    double m_hoverY = -1.0;

    // Long note resize state
    Model::Note m_resizeOrigNote;
    bool m_resizeFromTail = false;

    const Model::Note* hitTestNote(double x, double y) const;
    void doRubberBandSelection();
    void commitNoteMove();
};
