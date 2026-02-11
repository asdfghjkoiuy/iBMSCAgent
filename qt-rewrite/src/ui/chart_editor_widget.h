#pragma once

#include "core/bms_document.h"
#include "theme/theme_loader.h"

#include <QVector>
#include <QRect>
#include <QWidget>

#include <limits>

namespace ibmsc {

class ChartEditorWidget : public QWidget {
    Q_OBJECT
public:
    enum class EditMode {
        Select,
        Write,
        TimeSelect
    };

    struct DisplayOptions {
        bool showNotesGridLayers = true;
        bool showColumnBackgrounds = true;
        bool showVerticalLines = true;
        bool showBpmChannels = true;
        bool showStopChannels = true;
        bool showScrollChannels = true;
        bool showBlpChannels = true;
    };

    struct WaveformOptions {
        int width = 512;
        int offset = 0;
        int alpha = 110;
        int precision = 8;
        bool locked = false;
    };

    explicit ChartEditorWidget(QWidget* parent = nullptr);

    void setDocument(BmsDocument* doc);
    void setTheme(const Theme* theme);
    void setDefaultChannel(const QString& channel);
    void setDefaultValue(int valueX10000);
    void setEditMode(EditMode mode);
    void setWriteModifiers(bool longNote, bool hidden, bool landmine);
    void setNtInput(bool enabled);
    EditMode editMode() const { return m_mode; }
    void setSnapDivision(int divide);
    void setSnapEnabled(bool enabled);
    void setDisplayOptions(const DisplayOptions& options);
    void setWaveformData(const QVector<float>& samples);
    void clearWaveformData();
    void setWaveformOptions(const WaveformOptions& options);

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

signals:
    void aboutToEdit();
    void notePicked(const BmsNote& note);
    void documentEdited();
    void selectionChanged(int count);
    void focusEntered(ibmsc::ChartEditorWidget* editor);
    void timeSelectionChanged(double start, double length);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    bool event(QEvent* event) override;

private:
    enum class TimeDragHandle {
        None,
        NewRange,
        StartLine,
        MidLine,
        EndLine
    };

    struct DragNoteState {
        int index = -1;
        int column = 0;
        double vPosition = 0.0;
    };

    int columnAtX(int x) const;
    QRect noteRect(const BmsNote& note) const;
    int vPosToY(double vPos) const;
    double yToVPos(int y, bool applySnap = true) const;
    int columnX(int columnIndex) const;
    int columnWidth(int columnIndex) const;
    int totalWidth() const;
    int totalHeight() const;
    double maxEditableVPos() const;
    const ThemeColumn* themeColumnByIndex(int columnIndex) const;
    bool isColumnVisibleByTheme(int columnIndex) const;
    bool isColumnEditable(int columnIndex) const;
    QVector<int> enabledColumnList() const;
    int nearestEnabledColumn(int columnIndex) const;
    int shiftEnabledColumn(int columnIndex, int delta) const;
    QString displayColumnTitle(int columnIndex) const;
    int hitTestNote(const QPointF& localPos) const;
    void clearSelection();
    int selectedCount() const;
    void selectInRect(const QRect& rect, bool append);
    void emitSelectionChanged();
    void moveSelectedNotes(int dCol, double dVPos);
    void applySelectDragMove(int targetCol, double targetVPos);
    bool applyWriteAt(const QPointF& pos, Qt::MouseButton button, Qt::KeyboardModifiers mods);
    bool isChannelVisible(const BmsNote& note) const;
    void removeSelectedNotes();
    void ensureSorted();
    void captureZoomAnchorFromGlobal(const QPoint& globalPos);
    void applyZoomWithAnchor(double zoomValue);

    BmsDocument* m_doc = nullptr;
    const Theme* m_theme = nullptr;
    QString m_defaultChannel = "11";
    int m_defaultValue = 10000;
    bool m_writeLongNote = false;
    bool m_writeHidden = false;
    bool m_writeLandmine = false;
    EditMode m_mode = EditMode::Write;
    int m_snapDivide = 16;
    bool m_snapEnabled = true;
    double m_zoom = 3.0;
    QPointF m_accumulatedTrackpadDelta;
    bool m_dragSelecting = false;
    QRect m_selectRect;
    QPoint m_selectOrigin;
    bool m_selectDragActive = false;
    bool m_selectDragMoved = false;
    bool m_toggleOnRelease = false;
    int m_toggleNoteIndex = -1;
    bool m_duplicateDragMode = false;
    bool m_duplicateCreated = false;
    int m_selectDragAnchorCol = -1;
    double m_selectDragAnchorVPos = 0.0;
    QVector<DragNoteState> m_selectDragBaseNotes;
    bool m_timeSelecting = false;
    double m_timeSelAnchor = 0.0;
    double m_timeSelStart = 0.0;
    double m_timeSelLength = 0.0;
    TimeDragHandle m_timeDragHandle = TimeDragHandle::None;
    double m_timeDragPressV = 0.0;
    double m_timeDragBaseStart = 0.0;
    double m_timeDragBaseLength = 0.0;
    bool m_hoverValid = false;
    int m_hoverColumn = -1;
    double m_hoverVPos = -1.0;
    int m_hoverNoteIndex = -1;
    bool m_disableVerticalMove = false;
    bool m_writeDragging = false;
    Qt::MouseButton m_writeDragButton = Qt::NoButton;
    int m_lastWriteCol = -1;
    qint64 m_lastWriteVQuant = std::numeric_limits<qint64>::min();
    bool m_ntInput = false;
    DisplayOptions m_displayOptions;
    WaveformOptions m_waveformOptions;
    QVector<float> m_waveformData;
    bool m_ntWriting = false;
    int m_ntEditingIndex = -1;
    bool m_ntEditingExisting = false;
    double m_ntPressVPos = 0.0;
    bool m_pinchZoomActive = false;
    double m_zoomAnchorVPos = 0.0;
    QPoint m_zoomAnchorViewportPos;
};

} // namespace ibmsc
