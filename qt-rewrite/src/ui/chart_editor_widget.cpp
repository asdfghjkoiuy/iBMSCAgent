#include "ui/chart_editor_widget.h"

#include <QApplication>
#include <QFocusEvent>
#include <QInputDialog>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QNativeGestureEvent>
#include <QPainter>
#include <QPainterPath>
#include <QScrollArea>
#include <QScrollBar>
#include <QWheelEvent>

#include <algorithm>
#include <cmath>

namespace ibmsc {

namespace {
QString toBase36Label2(int value) {
    static const char* kDigits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int v = std::clamp(value, 0, 36 * 36 - 1);
    QString out;
    out += QChar(kDigits[(v / 36) % 36]);
    out += QChar(kDigits[v % 36]);
    return out;
}

QString normalizedChannelForDisplay(const BmsNote& note) {
    QString ch = note.channelId.trimmed().toUpper();
    if (ch.isEmpty()) {
        ch = BmsDocument::columnToDefaultChannel(note.columnIndex);
    }
    if (ch.size() == 1) {
        ch.prepend('0');
    }
    return ch;
}

int parseBase36Label(const QString& text) {
    bool ok = false;
    const int v = text.trimmed().toUpper().toInt(&ok, 36);
    if (!ok) {
        return -1;
    }
    return v;
}

int lowerBoundByVPos(const QVector<BmsNote>& notes, double value) {
    const auto it = std::lower_bound(notes.begin(), notes.end(), value, [](const BmsNote& note, double v) {
        return note.vPosition < v;
    });
    return static_cast<int>(std::distance(notes.begin(), it));
}

int upperBoundByVPos(const QVector<BmsNote>& notes, double value) {
    const auto it = std::upper_bound(notes.begin(), notes.end(), value, [](double v, const BmsNote& note) {
        return v < note.vPosition;
    });
    return static_cast<int>(std::distance(notes.begin(), it));
}

constexpr double kVisibleLookbackV = 8192.0;
} // namespace

ChartEditorWidget::ChartEditorWidget(QWidget* parent)
    : QWidget(parent) {
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
}

void ChartEditorWidget::setDocument(BmsDocument* doc) {
    m_doc = doc;
    const QSize s = minimumSizeHint();
    setMinimumSize(s);
    resize(s);
    {
        const QSize s = minimumSizeHint();
        setMinimumSize(s);
        resize(s);
    }
    updateGeometry();
    update();
}

void ChartEditorWidget::setTheme(const Theme* theme) {
    m_theme = theme;
    const QSize s = minimumSizeHint();
    setMinimumSize(s);
    resize(s);
    updateGeometry();
    update();
}

void ChartEditorWidget::setDefaultChannel(const QString& channel) {
    m_defaultChannel = channel.trimmed().toUpper();
}

void ChartEditorWidget::setDefaultValue(int valueX10000) {
    m_defaultValue = valueX10000;
}

void ChartEditorWidget::setEditMode(EditMode mode) {
    m_mode = mode;
    update();
}

void ChartEditorWidget::setWriteModifiers(bool longNote, bool hidden, bool landmine) {
    m_writeLongNote = longNote;
    m_writeHidden = hidden;
    m_writeLandmine = landmine;
}

void ChartEditorWidget::setNtInput(bool enabled) {
    m_ntInput = enabled;
}

void ChartEditorWidget::setSnapDivision(int divide) {
    m_snapDivide = std::clamp(divide, 1, 192);
}

void ChartEditorWidget::setSnapEnabled(bool enabled) {
    m_snapEnabled = enabled;
}

void ChartEditorWidget::setDisplayOptions(const DisplayOptions& options) {
    m_displayOptions = options;
    update();
}

void ChartEditorWidget::setWaveformData(const QVector<float>& samples) {
    m_waveformData = samples;
    update();
}

void ChartEditorWidget::clearWaveformData() {
    m_waveformData.clear();
    update();
}

void ChartEditorWidget::setWaveformOptions(const WaveformOptions& options) {
    m_waveformOptions = options;
    update();
}

QSize ChartEditorWidget::minimumSizeHint() const {
    return QSize(totalWidth(), totalHeight());
}

QSize ChartEditorWidget::sizeHint() const {
    return minimumSizeHint();
}

int ChartEditorWidget::columnWidth(int columnIndex) const {
    if (const ThemeColumn* c = themeColumnByIndex(columnIndex)) {
        if (!c->isVisible) {
            return 0;
        }
        return std::max(0, c->width);
    }
    if (columnIndex < 0 || columnIndex >= BmsDocument::columnCount()) {
        return 0;
    }
    if (!BmsDocument::isVisibleColumn(columnIndex)) {
        return 0;
    }
    return 40;
}

int ChartEditorWidget::totalWidth() const {
    int width = 0;
    if (m_theme && !m_theme->columns.isEmpty()) {
        for (int i = 0; i < m_theme->columns.size(); ++i) {
            width += columnWidth(i);
        }
    } else {
        width = BmsDocument::columnCount() * 40;
    }
    return width + 1;
}

int ChartEditorWidget::totalHeight() const {
    if (!m_doc) {
        return 1200;
    }
    const double maxEditable = maxEditableVPos();
    double maxV = 1920.0;
    if (!m_doc->measureBottom.isEmpty()) {
        const int last = m_doc->measureBottom.size() - 1;
        maxV = std::max(maxV, m_doc->measureBottomAt(last) + m_doc->measureLengthAt(last));
    }
    for (const BmsNote& n : m_doc->notes) {
        if (n.vPosition < 0.0) {
            continue;
        }
        const double clampedHead = std::clamp(n.vPosition, 0.0, maxEditable);
        const double clampedTail = std::clamp(n.vPosition + std::max(0.0, n.length), 0.0, maxEditable);
        maxV = std::max(maxV, std::max(clampedHead, clampedTail));
    }
    maxV = std::min(maxV, maxEditable);

    // Keep top/bottom visual margins in pixels (not vPos units), so zooming won't
    // push max measure notes out of view near the top.
    constexpr int kPixelMargin = 60;
    const int h = static_cast<int>(std::ceil(maxV * m_zoom)) + kPixelMargin * 2;
    return std::max(1200, h);
}

double ChartEditorWidget::maxEditableVPos() const {
    if (!m_doc || m_doc->measureBottom.isEmpty()) {
        return 191999.0;
    }
    const int last = static_cast<int>(m_doc->measureBottom.size()) - 1;
    return std::max(0.0, m_doc->measureBottomAt(last) + m_doc->measureLengthAt(last) - 1.0);
}

const ThemeColumn* ChartEditorWidget::themeColumnByIndex(int columnIndex) const {
    if (!m_theme || m_theme->columns.isEmpty()) {
        return nullptr;
    }
    if (columnIndex < 0 || columnIndex >= m_theme->columns.size()) {
        return nullptr;
    }
    return &m_theme->columns[columnIndex];
}

bool ChartEditorWidget::isColumnVisibleByTheme(int columnIndex) const {
    if (const ThemeColumn* c = themeColumnByIndex(columnIndex)) {
        return c->isVisible && c->width > 0;
    }
    return BmsDocument::isVisibleColumn(columnIndex);
}

bool ChartEditorWidget::isColumnEditable(int columnIndex) const {
    if (columnIndex < 0) {
        return false;
    }
    if (const ThemeColumn* c = themeColumnByIndex(columnIndex)) {
        return c->isEnabledAfterAll();
    }
    return BmsDocument::isEnabledColumn(columnIndex);
}

QVector<int> ChartEditorWidget::enabledColumnList() const {
    QVector<int> cols;
    const int colCount = m_theme && !m_theme->columns.isEmpty() ? m_theme->columns.size() : BmsDocument::columnCount();
    cols.reserve(colCount);
    for (int i = 0; i < colCount; ++i) {
        if (isColumnEditable(i)) {
            cols.push_back(i);
        }
    }
    return cols;
}

int ChartEditorWidget::nearestEnabledColumn(int columnIndex) const {
    const QVector<int> cols = enabledColumnList();
    if (cols.isEmpty()) {
        return -1;
    }
    if (columnIndex < cols.first()) {
        return cols.first();
    }
    if (columnIndex > cols.last()) {
        return cols.last();
    }
    int best = cols.first();
    int bestDist = std::abs(best - columnIndex);
    for (const int c : cols) {
        const int d = std::abs(c - columnIndex);
        if (d < bestDist) {
            best = c;
            bestDist = d;
        }
    }
    return best;
}

int ChartEditorWidget::shiftEnabledColumn(int columnIndex, int delta) const {
    const QVector<int> cols = enabledColumnList();
    if (cols.isEmpty()) {
        return columnIndex;
    }
    const int normalized = nearestEnabledColumn(columnIndex);
    int idx = cols.indexOf(normalized);
    if (idx < 0) {
        idx = 0;
    }
    idx = std::clamp(idx + delta, 0, static_cast<int>(cols.size()) - 1);
    return cols[idx];
}

QString ChartEditorWidget::displayColumnTitle(int columnIndex) const {
    if (BmsDocument::columnIdentifier(columnIndex) == 1 && columnIndex >= 27) {
        return BmsDocument::columnTitle(columnIndex);
    }
    if (const ThemeColumn* c = themeColumnByIndex(columnIndex)) {
        if (!c->title.trimmed().isEmpty()) {
            return c->title;
        }
    }
    return BmsDocument::columnTitle(columnIndex);
}

int ChartEditorWidget::columnX(int columnIndex) const {
    int x = 0;
    const int idx = std::max(0, columnIndex);
    for (int i = 0; i < idx; ++i) {
        x += columnWidth(i);
    }
    return x;
}

int ChartEditorWidget::columnAtX(int x) const {
    if (x < 0) {
        return -1;
    }
    int left = 0;
    const int colCount = m_theme && !m_theme->columns.isEmpty() ? m_theme->columns.size() : BmsDocument::columnCount();
    for (int i = 0; i < colCount; ++i) {
        const int w = columnWidth(i);
        if (w <= 0) {
            continue;
        }
        if (x >= left && x < left + w) {
            return i;
        }
        left += w;
    }
    return -1;
}

int ChartEditorWidget::vPosToY(double vPos) const {
    const int baseline = height() - 60;
    return baseline - static_cast<int>(std::round(vPos * m_zoom));
}

double ChartEditorWidget::yToVPos(int y, bool applySnap) const {
    const int baseline = height() - 60;
    const double maxV = maxEditableVPos();
    const double raw = std::clamp((baseline - y) / m_zoom, 0.0, maxV);
    if (!applySnap || !m_snapEnabled || !m_doc) {
        return raw;
    }

    const int measure = std::max(0, m_doc->measureAtPosition(raw));
    const double bottom = m_doc->measureBottomAt(measure);
    const double length = std::max(1.0, m_doc->measureLengthAt(measure));
    const double local = std::max(0.0, raw - bottom);
    const double step = length / std::max(1, m_snapDivide);
    const double snapped = std::floor((local / step) + 1e-9) * step + bottom;
    return std::clamp(snapped, 0.0, maxV);
}

QRect ChartEditorWidget::noteRect(const BmsNote& note) const {
    const int x = columnX(note.columnIndex);
    const int rawW = columnWidth(note.columnIndex);
    if (rawW <= 0) {
        return QRect();
    }
    const int w = std::max(8, rawW);
    const int yHead = vPosToY(note.vPosition);
    if (m_ntInput && note.length > 0.0) {
        const int yTail = vPosToY(note.vPosition + note.length);
        const int yTop = std::min(yHead, yTail);
        const int yBottom = std::max(yHead, yTail);
        return QRect(x + 2, yTop - 6, w - 4, std::max(12, yBottom - yTop + 12));
    }
    return QRect(x + 2, yHead - 6, w - 4, 12);
}

int ChartEditorWidget::hitTestNote(const QPointF& localPos) const {
    if (!m_doc) {
        return -1;
    }
    const double targetV = yToVPos(static_cast<int>(std::round(localPos.y())), false);
    const double scanMinV = std::max(0.0, targetV - kVisibleLookbackV);
    const double scanMaxV = targetV + 64.0;
    const int noteCount = static_cast<int>(m_doc->notes.size());
    const int begin = std::clamp(lowerBoundByVPos(m_doc->notes, scanMinV), 0, noteCount);
    const int end = std::clamp(upperBoundByVPos(m_doc->notes, scanMaxV), begin, noteCount);
    for (int i = end - 1; i >= begin; --i) {
        if (m_doc->notes[i].vPosition < 0.0) {
            continue;
        }
        if (!isChannelVisible(m_doc->notes[i])) {
            continue;
        }
        if (noteRect(m_doc->notes[i]).contains(localPos.toPoint())) {
            return i;
        }
    }
    return -1;
}

void ChartEditorWidget::clearSelection() {
    if (!m_doc) {
        return;
    }
    for (BmsNote& n : m_doc->notes) {
        n.selected = false;
    }
}

int ChartEditorWidget::selectedCount() const {
    if (!m_doc) {
        return 0;
    }
    int c = 0;
    for (const BmsNote& n : m_doc->notes) {
        c += n.selected ? 1 : 0;
    }
    return c;
}

void ChartEditorWidget::selectInRect(const QRect& rect, bool append) {
    if (!m_doc) {
        return;
    }
    if (!append) {
        clearSelection();
    }
    const QRect normalized = rect.normalized();
    const double v1 = yToVPos(normalized.bottom() + 16, false);
    const double v2 = yToVPos(normalized.top() - 16, false);
    const double scanMinV = std::max(0.0, std::min(v1, v2) - kVisibleLookbackV);
    const double scanMaxV = std::max(v1, v2) + 16.0;
    const int noteCount = static_cast<int>(m_doc->notes.size());
    const int begin = std::clamp(lowerBoundByVPos(m_doc->notes, scanMinV), 0, noteCount);
    const int end = std::clamp(upperBoundByVPos(m_doc->notes, scanMaxV), begin, noteCount);
    for (int i = begin; i < end; ++i) {
        BmsNote& n = m_doc->notes[i];
        if (n.vPosition < 0) {
            continue;
        }
        if (normalized.intersects(noteRect(n))) {
            n.selected = true;
        }
    }
}

void ChartEditorWidget::emitSelectionChanged() {
    emit selectionChanged(selectedCount());
}

void ChartEditorWidget::moveSelectedNotes(int dCol, double dVPos) {
    if (!m_doc) {
        return;
    }
    const int maxMeasure = std::max(0, static_cast<int>(m_doc->measureBottom.size()) - 1);
    const double maxV = std::max(0.0, m_doc->measureBottomAt(maxMeasure) + m_doc->measureLengthAt(maxMeasure) - 1.0);
    for (BmsNote& n : m_doc->notes) {
        if (!n.selected || n.vPosition < 0) {
            continue;
        }
        n.columnIndex = shiftEnabledColumn(n.columnIndex, dCol);
        n.vPosition = std::max(0.0, n.vPosition + dVPos);
        if (m_ntInput && n.length > 0.0 && n.vPosition + n.length > maxV) {
            n.vPosition = std::max(0.0, maxV - n.length);
        }
    }
    ensureSorted();
}

void ChartEditorWidget::applySelectDragMove(int targetCol, double targetVPos) {
    if (!m_doc || m_selectDragBaseNotes.isEmpty() || m_selectDragAnchorCol < 0) {
        return;
    }

    const QVector<int> cols = enabledColumnList();
    if (cols.isEmpty()) {
        return;
    }
    auto enabledOrdinal = [&cols](int column) {
        int idx = cols.indexOf(column);
        if (idx >= 0) {
            return idx;
        }
        if (column <= cols.first()) return 0;
        if (column >= cols.last()) return static_cast<int>(cols.size()) - 1;
        int nearest = 0;
        int bestDist = std::abs(cols[0] - column);
        for (int i = 1; i < cols.size(); ++i) {
            const int d = std::abs(cols[i] - column);
            if (d < bestDist) {
                nearest = i;
                bestDist = d;
            }
        }
        return nearest;
    };

    const int anchorOrd = enabledOrdinal(m_selectDragAnchorCol);
    const int targetOrd = enabledOrdinal(targetCol);
    int dCol = targetOrd - anchorOrd;
    double dVPos = targetVPos - m_selectDragAnchorVPos;

    int minOrd = std::numeric_limits<int>::max();
    int maxOrd = std::numeric_limits<int>::min();
    double minV = 1e18;
    for (const DragNoteState& base : m_selectDragBaseNotes) {
        const int ord = enabledOrdinal(base.column) + dCol;
        minOrd = std::min(minOrd, ord);
        maxOrd = std::max(maxOrd, ord);
        minV = std::min(minV, base.vPosition + dVPos);
    }
    if (minOrd < 0) {
        dCol += -minOrd;
    }
    const int maxOrdAllowed = static_cast<int>(cols.size()) - 1;
    if (maxOrd > maxOrdAllowed) {
        dCol -= (maxOrd - maxOrdAllowed);
    }
    if (minV < 0.0) {
        dVPos += -minV;
    }

    const int maxMeasure = std::max(0, static_cast<int>(m_doc->measureBottom.size()) - 1);
    const double maxVAllowed = std::max(0.0, m_doc->measureBottomAt(maxMeasure) + m_doc->measureLengthAt(maxMeasure) - 1.0);
    double maxV = -1.0;
    for (const DragNoteState& base : m_selectDragBaseNotes) {
        if (base.index >= 0 && base.index < m_doc->notes.size()) {
            const BmsNote& n = m_doc->notes[base.index];
            const double tail = base.vPosition + (m_ntInput ? std::max(0.0, n.length) : 0.0);
            maxV = std::max(maxV, tail + dVPos);
        } else {
            maxV = std::max(maxV, base.vPosition + dVPos);
        }
    }
    if (maxV > maxVAllowed) {
        dVPos -= (maxV - maxVAllowed);
    }

    for (const DragNoteState& base : m_selectDragBaseNotes) {
        if (base.index < 0 || base.index >= m_doc->notes.size()) {
            continue;
        }
        BmsNote& n = m_doc->notes[base.index];
        const int ord = std::clamp(enabledOrdinal(base.column) + dCol, 0, maxOrdAllowed);
        n.columnIndex = cols[ord];
        n.vPosition = std::max(0.0, base.vPosition + dVPos);
    }
    ensureSorted();
}

bool ChartEditorWidget::applyWriteAt(const QPointF& pos, Qt::MouseButton button, Qt::KeyboardModifiers mods) {
    if (!m_doc) {
        return false;
    }
    const int col = columnAtX(pos.x());
    if (!isColumnEditable(col)) {
        return false;
    }

    const bool applySnap = !mods.testFlag(Qt::ControlModifier);
    const double snappedV = std::max(0.0, yToVPos(pos.y(), applySnap));
    const qint64 vq = qRound64(snappedV * 1000000.0);
    if (m_lastWriteCol == col && m_lastWriteVQuant == vq) {
        return false;
    }
    m_lastWriteCol = col;
    m_lastWriteVQuant = vq;

    const double epsilon = 0.001;
    if (button == Qt::LeftButton) {
        const bool shiftDown = mods.testFlag(Qt::ShiftModifier);
        const bool ctrlDown = mods.testFlag(Qt::ControlModifier);
        const bool writeLong = m_writeLongNote || (shiftDown && !ctrlDown);
        const bool writeHidden = m_writeHidden || (ctrlDown && !shiftDown);
        const bool writeLandmine = m_writeLandmine || (ctrlDown && shiftDown);

        m_doc->notes.erase(std::remove_if(m_doc->notes.begin(), m_doc->notes.end(), [&](const BmsNote& n) {
                               return n.columnIndex == col && std::abs(n.vPosition - snappedV) < epsilon;
                           }),
                           m_doc->notes.end());

        BmsNote note;
        note.columnIndex = col;
        note.vPosition = snappedV;
        note.value = m_defaultValue;
        note.channelId = m_defaultChannel;
        note.longNote = writeLong;
        note.hidden = writeHidden;
        note.landmine = writeLandmine;
        note.selected = true;
        note.length = 0.0;
        clearSelection();
        m_doc->notes.push_back(note);
        ensureSorted();
        emit notePicked(note);
        emitSelectionChanged();
        return true;
    }

    if (button == Qt::RightButton) {
        auto it = std::find_if(m_doc->notes.begin(), m_doc->notes.end(), [&](const BmsNote& n) {
            return n.columnIndex == col && std::abs(n.vPosition - snappedV) < epsilon;
        });
        if (it != m_doc->notes.end()) {
            m_doc->notes.erase(it);
            emitSelectionChanged();
            return true;
        }
    }
    return false;
}

bool ChartEditorWidget::isChannelVisible(const BmsNote& note) const {
    const QString ch = normalizedChannelForDisplay(note);
    if ((ch == "03" || ch == "08") && !m_displayOptions.showBpmChannels) {
        return false;
    }
    if (ch == "09" && !m_displayOptions.showStopChannels) {
        return false;
    }
    if (ch == "SC" && !m_displayOptions.showScrollChannels) {
        return false;
    }
    if ((ch == "06" || ch == "07") && !m_displayOptions.showBlpChannels) {
        return false;
    }
    return true;
}

void ChartEditorWidget::removeSelectedNotes() {
    if (!m_doc) {
        return;
    }
    m_doc->notes.erase(std::remove_if(m_doc->notes.begin(), m_doc->notes.end(), [](const BmsNote& n) {
                           return n.selected && n.vPosition >= 0;
                       }),
                       m_doc->notes.end());
}

void ChartEditorWidget::ensureSorted() {
    if (!m_doc) {
        return;
    }
    std::sort(m_doc->notes.begin(), m_doc->notes.end(), [](const BmsNote& a, const BmsNote& b) {
        if (a.vPosition == b.vPosition) {
            return a.columnIndex < b.columnIndex;
        }
        return a.vPosition < b.vPosition;
    });
}

void ChartEditorWidget::captureZoomAnchorFromGlobal(const QPoint& globalPos) {
    QScrollArea* scrollArea = qobject_cast<QScrollArea*>(parentWidget() ? parentWidget()->parentWidget() : nullptr);
    if (!scrollArea) {
        scrollArea = qobject_cast<QScrollArea*>(parentWidget());
    }
    if (!scrollArea || !scrollArea->verticalScrollBar()) {
        m_zoomAnchorViewportPos = QPoint(width() / 2, height() / 2);
        m_zoomAnchorVPos = yToVPos(m_zoomAnchorViewportPos.y(), false);
        return;
    }

    m_zoomAnchorViewportPos = scrollArea->viewport()->mapFromGlobal(globalPos);
    const int oldScrollY = scrollArea->verticalScrollBar()->value();
    const int anchorContentY = oldScrollY + m_zoomAnchorViewportPos.y();
    m_zoomAnchorVPos = yToVPos(anchorContentY, false);
}

void ChartEditorWidget::applyZoomWithAnchor(double zoomValue) {
    const double oldZoom = m_zoom;
    const double newZoom = std::clamp(zoomValue, 0.8, 10.0);
    if (std::abs(newZoom - oldZoom) < 1e-6) {
        return;
    }
    m_zoom = newZoom;
    updateGeometry();

    QScrollArea* scrollArea = qobject_cast<QScrollArea*>(parentWidget() ? parentWidget()->parentWidget() : nullptr);
    if (!scrollArea) {
        scrollArea = qobject_cast<QScrollArea*>(parentWidget());
    }
    if (scrollArea && scrollArea->verticalScrollBar()) {
        const int newAnchorY = vPosToY(m_zoomAnchorVPos);
        const int targetScrollY = newAnchorY - m_zoomAnchorViewportPos.y();
        scrollArea->verticalScrollBar()->setValue(targetScrollY);
    }
    update();
}

void ChartEditorWidget::paintEvent(QPaintEvent* event) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    QColor bg = (m_theme ? m_theme->visual.background : QColor(20, 20, 20));
    if (bg.alpha() == 0) {
        bg = QColor(20, 20, 20);
    }
    p.fillRect(rect(), bg);

    const int colCount = m_theme && !m_theme->columns.isEmpty() ? m_theme->columns.size() : BmsDocument::columnCount();
    int x = 0;
    for (int i = 0; i < colCount; ++i) {
        const int w = columnWidth(i);
        if (w <= 0) {
            continue;
        }
        QColor colBg = QColor(30, 30, 30, 90);
        QColor txt = QColor(220, 220, 220);
        QString title = displayColumnTitle(i);
        if (const ThemeColumn* c = themeColumnByIndex(i)) {
            if (c->bgColor.isValid() && c->bgColor.alpha() > 0) {
                colBg = c->bgColor;
            }
            txt = c->textColor.isValid() ? c->textColor : txt;
        }

        if (m_displayOptions.showColumnBackgrounds) {
            p.fillRect(QRect(x, 0, w, height()), colBg);
        }
        QColor vline = m_theme ? m_theme->visual.vLine : QColor(70, 70, 70);
        if (vline.alpha() == 0) {
            vline = QColor(70, 70, 70);
        }
        if (m_displayOptions.showVerticalLines) {
            p.setPen(vline);
            p.drawLine(x, 0, x, height());
        }
        p.setPen(txt);
        p.drawText(QRect(x + 2, 2, w - 4, 20), Qt::AlignLeft | Qt::AlignVCenter, title);
        x += w;
    }

    QColor mline = m_theme ? m_theme->visual.mLine : QColor(120, 120, 120);
    if (mline.alpha() == 0) {
        mline = QColor(120, 120, 120);
    }
    QColor gridLine = m_theme ? m_theme->visual.grid : QColor(85, 85, 85);
    if (gridLine.alpha() == 0) {
        gridLine = QColor(85, 85, 85);
    }

    const QRect clip = event ? event->rect() : rect();
    const double vTop = yToVPos(clip.top() - 40, false);
    const double vBottom = yToVPos(clip.bottom() + 40, false);
    const double visibleMinV = std::max(0.0, std::min(vTop, vBottom) - 192.0);
    const double visibleMaxV = std::max(vTop, vBottom) + 192.0;

    if (m_displayOptions.showNotesGridLayers && m_doc && !m_doc->measureBottom.isEmpty()) {
        for (int i = 0; i < m_doc->measureBottom.size(); ++i) {
            const double bottom = m_doc->measureBottomAt(i);
            const double len = std::max(1.0, m_doc->measureLengthAt(i));
            if (bottom + len < visibleMinV) {
                continue;
            }
            if (bottom > visibleMaxV) {
                break;
            }
            const int y = vPosToY(bottom);

            p.setPen(mline);
            p.drawLine(0, y, totalWidth(), y);
            p.drawText(4, y - 2, QString::number(i));

            const int div = std::max(1, m_snapDivide);
            const double step = len / div;
            p.setPen(gridLine);
            for (int s = 1; s < div; ++s) {
                const int gy = vPosToY(bottom + step * s);
                p.drawLine(0, gy, totalWidth(), gy);
            }
        }
    } else if (m_displayOptions.showNotesGridLayers) {
        const int minMeasure = std::max(0, static_cast<int>(std::floor(visibleMinV / 192.0)) - 1);
        const int maxMeasure = std::max(minMeasure + 1, static_cast<int>(std::ceil(visibleMaxV / 192.0)) + 1);
        p.setPen(mline);
        for (int i = minMeasure; i <= maxMeasure; ++i) {
            const int y = vPosToY(i * 192.0);
            p.drawLine(0, y, totalWidth(), y);
            p.drawText(4, y - 2, QString::number(i));
        }
    }

    if (m_mode == EditMode::TimeSelect && m_timeSelLength > 0.0) {
        const int y1 = vPosToY(m_timeSelStart);
        const int ym = vPosToY(m_timeSelStart + m_timeSelLength * 0.5);
        const int y2 = vPosToY(m_timeSelStart + m_timeSelLength);
        const int yTop = std::min(y1, y2);
        const int yBottom = std::max(y1, y2);
        p.fillRect(QRect(0, yTop, totalWidth(), yBottom - yTop), QColor(120, 180, 255, 40));
        p.setPen(QPen(QColor(120, 180, 255, 190), 2));
        p.drawLine(0, y1, totalWidth(), y1);
        p.drawLine(0, ym, totalWidth(), ym);
        p.drawLine(0, y2, totalWidth(), y2);
    }

    if (m_mode == EditMode::Write && m_hoverValid && m_hoverColumn >= 0 && isColumnEditable(m_hoverColumn) && m_hoverVPos >= 0.0) {
        BmsNote ghost;
        ghost.columnIndex = m_hoverColumn;
        ghost.vPosition = m_hoverVPos;
        QRect gr = noteRect(ghost);
        QColor ghostFill(180, 220, 255, 90);
        if (const ThemeColumn* c = themeColumnByIndex(m_hoverColumn)) {
            ghostFill = c->noteColor;
            ghostFill.setAlpha(90);
        }
        p.setPen(QPen(QColor(200, 240, 255, 220), 1, Qt::DashLine));
        p.setBrush(ghostFill);
        p.drawRoundedRect(gr, 2, 2);
    }

    if (!m_waveformData.isEmpty()) {
        const int drawWidth = std::clamp(m_waveformOptions.width, 1, std::max(8, totalWidth()));
        const int centerX = std::clamp(totalWidth() - 16 + m_waveformOptions.offset, 0, totalWidth());
        const int halfW = drawWidth / 2;
        const int pointStep = std::max(1, m_waveformOptions.precision);
        QPen wavePen(QColor(128, 204, 255, std::clamp(m_waveformOptions.alpha, 0, 255)), 1);
        p.setPen(wavePen);
        QPainterPath path;
        bool pathStarted = false;
        const int n = m_waveformData.size();
        if (n > 1) {
            for (int i = 0; i < n; i += pointStep) {
                const float amp = std::clamp(m_waveformData[i], -1.0f, 1.0f);
                const double ratio = static_cast<double>(i) / static_cast<double>(n - 1);
                const double vPos = ratio * maxEditableVPos();
                const int y = vPosToY(vPos);
                if (y < -4 || y > height() + 4) {
                    continue;
                }
                const int x = centerX + static_cast<int>(std::round(amp * halfW));
                if (!pathStarted) {
                    path.moveTo(x, y);
                    pathStarted = true;
                } else {
                    path.lineTo(x, y);
                }
            }
        }
        if (pathStarted) {
            p.drawPath(path);
        }
    }

    if (!m_doc) {
        return;
    }

    if (!m_displayOptions.showNotesGridLayers) {
        return;
    }

    const double noteMinV = std::max(0.0, visibleMinV - 64.0);
    const double noteMaxV = visibleMaxV + 64.0;

    const int noteCount = static_cast<int>(m_doc->notes.size());
    const int begin = std::clamp(lowerBoundByVPos(m_doc->notes, noteMinV - kVisibleLookbackV), 0, noteCount);
    const int end = std::clamp(upperBoundByVPos(m_doc->notes, noteMaxV + 64.0), begin, noteCount);
    for (int i = begin; i < end; ++i) {
        const BmsNote& note = m_doc->notes[i];
        if (note.vPosition < 0.0) {
            continue;
        }
        const double tail = note.vPosition + std::max(0.0, note.length);
        if (tail < noteMinV || note.vPosition > noteMaxV) {
            continue;
        }
        if (!isChannelVisible(note)) {
            continue;
        }

        QRect r = noteRect(note);
        if (!r.intersects(clip.adjusted(-20, -20, 20, 20))) {
            continue;
        }

        QColor fill = QColor(245, 190, 70, 220);
        if (const ThemeColumn* c = themeColumnByIndex(note.columnIndex)) {
            fill = note.longNote ? c->longNoteColor : c->noteColor;
            if (!fill.isValid()) {
                fill = QColor(245, 190, 70, 220);
            }
        }

        p.setPen(note.selected ? (m_theme ? m_theme->visual.selected : QColor(255, 255, 0)) : QColor(10, 10, 10));
        p.setBrush(fill);
        p.drawRoundedRect(r, 2, 2);
        if (m_zoom >= 1.2 && r.height() >= 10) {
            p.setPen(QColor(0, 0, 0));
            p.drawText(r, Qt::AlignCenter, toBase36Label2(note.value / 10000));
        }
    }

    if (m_mode == EditMode::Select && m_hoverNoteIndex >= 0 && m_hoverNoteIndex < m_doc->notes.size()) {
        const BmsNote& n = m_doc->notes[m_hoverNoteIndex];
        if (n.vPosition >= 0.0) {
            QRect hr = noteRect(n).adjusted(-1, -1, 1, 1);
            p.setBrush(Qt::NoBrush);
            p.setPen(QPen(QColor(130, 255, 170, 230), 2));
            p.drawRect(hr);
        }
    }

    if (m_dragSelecting && m_mode == EditMode::Select) {
        p.setPen(QPen(QColor(160, 220, 255), 1, Qt::DashLine));
        p.setBrush(QColor(160, 220, 255, 35));
        p.drawRect(m_selectRect.normalized());
    }

    if (!m_doc || m_doc->notes.size() <= 1) {
        p.setPen(QColor(220, 220, 220, 180));
        p.drawText(QRect(20, 30, width() - 40, 24), Qt::AlignLeft | Qt::AlignVCenter, "Editor ready: click to add notes");
    }
}

void ChartEditorWidget::mousePressEvent(QMouseEvent* event) {
    if (!m_doc) {
        return;
    }

    emit focusEntered(this);
    setFocus(Qt::MouseFocusReason);
    const bool appendSelect = event->modifiers().testFlag(Qt::ControlModifier);

    if (m_mode == EditMode::Select) {
        const int hit = hitTestNote(event->position());
        if (event->button() == Qt::LeftButton) {
            if (hit >= 0 && event->type() == QEvent::MouseButtonDblClick) {
                const int current = std::clamp(m_doc->notes[hit].value / 10000, 1, 1295);
                QString initial = QString::number(current, 36).toUpper().rightJustified(2, '0');
                bool ok = false;
                const QString entered = QInputDialog::getText(this,
                                                              "Edit Label",
                                                              "Label (base36, 01..ZZ):",
                                                              QLineEdit::Normal,
                                                              initial,
                                                              &ok)
                                            .trimmed()
                                            .toUpper();
                if (ok && !entered.isEmpty()) {
                    const int parsed = parseBase36Label(entered);
                    if (parsed >= 1 && parsed < BmsDocument::kTableSize) {
                        emit aboutToEdit();
                        m_doc->notes[hit].value = parsed * 10000;
                        emit notePicked(m_doc->notes[hit]);
                        emit documentEdited();
                        update();
                    }
                }
                return;
            }

            const bool multiLabel = event->modifiers().testFlag(Qt::ControlModifier) && event->modifiers().testFlag(Qt::ShiftModifier);
            if (hit >= 0) {
                if (multiLabel) {
                    const int label = m_doc->notes[hit].value;
                    for (BmsNote& n : m_doc->notes) {
                        if (n.vPosition < 0.0) {
                            continue;
                        }
                        if (n.value == label) {
                            n.selected = !n.selected;
                        }
                    }
                    m_selectDragActive = false;
                    m_selectDragMoved = false;
                    m_toggleOnRelease = false;
                    m_toggleNoteIndex = -1;
                    emitSelectionChanged();
                    update();
                    return;
                }

                const bool ctrlOnly = event->modifiers().testFlag(Qt::ControlModifier) && !event->modifiers().testFlag(Qt::ShiftModifier);
                m_toggleOnRelease = ctrlOnly;
                m_toggleNoteIndex = hit;
                m_duplicateDragMode = ctrlOnly;
                m_duplicateCreated = false;

                if (!appendSelect && !m_doc->notes[hit].selected) {
                    clearSelection();
                }
                if (!m_doc->notes[hit].selected) {
                    m_doc->notes[hit].selected = true;
                }
                emit notePicked(m_doc->notes[hit]);
                emitSelectionChanged();

                m_selectDragActive = true;
                m_selectDragMoved = false;
                m_selectDragAnchorCol = m_doc->notes[hit].columnIndex;
                m_selectDragAnchorVPos = std::max(0.0, yToVPos(event->position().y(), true));
                m_selectDragBaseNotes.clear();
                for (int i = 0; i < m_doc->notes.size(); ++i) {
                    const BmsNote& n = m_doc->notes[i];
                    if (n.selected && n.vPosition >= 0.0) {
                        DragNoteState state;
                        state.index = i;
                        state.column = n.columnIndex;
                        state.vPosition = n.vPosition;
                        m_selectDragBaseNotes.push_back(state);
                    }
                }
                if (!m_selectDragBaseNotes.isEmpty()) {
                    emit aboutToEdit();
                }
            } else {
                m_dragSelecting = true;
                m_selectOrigin = event->position().toPoint();
                m_selectRect = QRect(m_selectOrigin, QSize(1, 1));
                m_selectDragActive = false;
                m_selectDragMoved = false;
                m_toggleOnRelease = false;
                m_toggleNoteIndex = -1;
                m_duplicateDragMode = false;
                m_duplicateCreated = false;
                if (!appendSelect) {
                    clearSelection();
                }
                emitSelectionChanged();
            }
        }
        update();
        return;
    }

    if (m_mode == EditMode::TimeSelect) {
        if (event->button() == Qt::LeftButton) {
            m_timeSelecting = true;
            const bool applySnap = !event->modifiers().testFlag(Qt::ControlModifier);
            const double now = std::max(0.0, yToVPos(event->position().y(), applySnap));
            const double tol = 6.0 / std::max(0.001, m_zoom);
            const double start = m_timeSelStart;
            const double end = m_timeSelStart + m_timeSelLength;
            const double mid = m_timeSelStart + m_timeSelLength * 0.5;

            m_timeDragHandle = TimeDragHandle::NewRange;
            if (m_timeSelLength > 0.0) {
                if (std::abs(now - start) <= tol) {
                    m_timeDragHandle = TimeDragHandle::StartLine;
                } else if (std::abs(now - mid) <= tol) {
                    m_timeDragHandle = TimeDragHandle::MidLine;
                } else if (std::abs(now - end) <= tol) {
                    m_timeDragHandle = TimeDragHandle::EndLine;
                }
            }

            m_timeDragPressV = now;
            m_timeDragBaseStart = m_timeSelStart;
            m_timeDragBaseLength = m_timeSelLength;
            m_timeSelAnchor = now;

            if (m_timeDragHandle == TimeDragHandle::NewRange) {
                m_timeSelStart = m_timeSelAnchor;
                m_timeSelLength = 0.0;
            }
            emit timeSelectionChanged(m_timeSelStart, m_timeSelLength);
            update();
        }
        return;
    }

    if (m_mode == EditMode::Write && m_ntInput && event->button() == Qt::LeftButton) {
        const int col = columnAtX(event->position().x());
        if (!isColumnEditable(col)) {
            return;
        }
        const bool applySnap = !event->modifiers().testFlag(Qt::ControlModifier);
        const double vPos = std::max(0.0, yToVPos(event->position().y(), applySnap));

        const double epsilon = 0.001;
        int hit = -1;
        for (int i = 0; i < m_doc->notes.size(); ++i) {
            const BmsNote& n = m_doc->notes[i];
            if (n.vPosition < 0.0) continue;
            if (n.columnIndex != col) {
                continue;
            }
            const bool exactHead = std::abs(n.vPosition - vPos) < epsilon;
            const bool inLongBody = n.length > epsilon && vPos > n.vPosition && vPos < (n.vPosition + n.length);
            if (exactHead || inLongBody) {
                hit = i;
                break;
            }
        }

        emit aboutToEdit();
        const bool shiftDown = event->modifiers().testFlag(Qt::ShiftModifier);
        const bool ctrlDown = event->modifiers().testFlag(Qt::ControlModifier);
        const bool writeLong = m_writeLongNote || (shiftDown && !ctrlDown);
        const bool writeHidden = m_writeHidden || (ctrlDown && !shiftDown);
        const bool writeLandmine = m_writeLandmine || (ctrlDown && shiftDown);

        if (hit >= 0) {
            clearSelection();
            m_doc->notes[hit].selected = true;
            m_ntEditingIndex = hit;
            m_ntEditingExisting = true;
            m_ntPressVPos = m_doc->notes[hit].vPosition;
            m_ntWriting = true;
            emit notePicked(m_doc->notes[hit]);
            emitSelectionChanged();
            update();
            return;
        }

        BmsNote note;
        note.columnIndex = col;
        note.vPosition = vPos;
        note.length = 0.0;
        note.value = m_defaultValue;
        note.channelId = m_defaultChannel;
        note.longNote = writeLong;
        note.hidden = writeHidden;
        note.landmine = writeLandmine;
        note.selected = true;
        clearSelection();
        m_doc->notes.push_back(note);
        ensureSorted();

        for (int i = 0; i < m_doc->notes.size(); ++i) {
            if (m_doc->notes[i].selected) {
                m_ntEditingIndex = i;
                break;
            }
        }
        m_ntEditingExisting = false;
        m_ntPressVPos = vPos;
        m_ntWriting = true;
        emit notePicked(note);
        emit documentEdited();
        emitSelectionChanged();
        update();
        return;
    }

    if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton) {
        emit aboutToEdit();
        m_writeDragging = true;
        m_writeDragButton = event->button();
        m_lastWriteCol = -1;
        m_lastWriteVQuant = std::numeric_limits<qint64>::min();
        if (applyWriteAt(event->position(), m_writeDragButton, event->modifiers())) {
            emit documentEdited();
        }
    }

    updateGeometry();
    update();
}

void ChartEditorWidget::mouseDoubleClickEvent(QMouseEvent* event) {
    mousePressEvent(event);
}

void ChartEditorWidget::mouseMoveEvent(QMouseEvent* event) {
    if (!m_doc) {
        return;
    }

    bool needsRepaint = false;
    if (event->buttons() == Qt::NoButton) {
        const int newHover = hitTestNote(event->position());
        if (newHover != m_hoverNoteIndex) {
            m_hoverNoteIndex = newHover;
            needsRepaint = true;
        }
    }

    if (m_dragSelecting && m_mode == EditMode::Select) {
        const QRect nextRect = QRect(m_selectOrigin, event->position().toPoint()).normalized();
        if (nextRect != m_selectRect) {
            m_selectRect = nextRect;
            selectInRect(m_selectRect, event->modifiers().testFlag(Qt::ControlModifier));
            emitSelectionChanged();
            update();
        }
        return;
    }

    if (m_selectDragActive && m_mode == EditMode::Select && (event->buttons() & Qt::LeftButton)) {
        const int col = nearestEnabledColumn(columnAtX(event->position().x()));
        if (col < 0) {
            return;
        }
        double vPos = std::max(0.0, yToVPos(event->position().y(), true));
        if (m_disableVerticalMove) {
            vPos = m_selectDragAnchorVPos;
        }

        if (m_duplicateDragMode && !m_duplicateCreated) {
            const int oldSize = m_doc->notes.size();
            for (int i = 0; i < oldSize; ++i) {
                BmsNote& src = m_doc->notes[i];
                if (!src.selected || src.vPosition < 0.0) {
                    continue;
                }
                BmsNote copy = src;
                copy.selected = true;
                src.selected = false;
                m_doc->notes.push_back(copy);
            }

            m_selectDragBaseNotes.clear();
            for (int i = oldSize; i < m_doc->notes.size(); ++i) {
                if (!m_doc->notes[i].selected || m_doc->notes[i].vPosition < 0.0) {
                    continue;
                }
                DragNoteState state;
                state.index = i;
                state.column = m_doc->notes[i].columnIndex;
                state.vPosition = m_doc->notes[i].vPosition;
                m_selectDragBaseNotes.push_back(state);
            }
            m_duplicateCreated = true;
            m_toggleOnRelease = false;
        }

        applySelectDragMove(col, vPos);
        m_selectDragMoved = true;
        update();
        return;
    }

    if (m_timeSelecting && m_mode == EditMode::TimeSelect) {
        const bool applySnap = !event->modifiers().testFlag(Qt::ControlModifier);
        const double now = std::max(0.0, yToVPos(event->position().y(), applySnap));

        switch (m_timeDragHandle) {
            case TimeDragHandle::NewRange: {
                m_timeSelStart = std::min(m_timeSelAnchor, now);
                m_timeSelLength = std::abs(now - m_timeSelAnchor);
                break;
            }
            case TimeDragHandle::StartLine: {
                const double fixedEnd = m_timeDragBaseStart + m_timeDragBaseLength;
                m_timeSelStart = std::min(now, fixedEnd);
                m_timeSelLength = std::abs(fixedEnd - now);
                break;
            }
            case TimeDragHandle::EndLine: {
                const double fixedStart = m_timeDragBaseStart;
                m_timeSelStart = std::min(fixedStart, now);
                m_timeSelLength = std::abs(now - fixedStart);
                break;
            }
            case TimeDragHandle::MidLine: {
                const double delta = now - m_timeDragPressV;
                double start = m_timeDragBaseStart + delta;
                start = std::max(0.0, start);
                const int maxMeasure = std::max(0, static_cast<int>(m_doc->measureBottom.size()) - 1);
                const double maxV = std::max(0.0, m_doc->measureBottomAt(maxMeasure) + m_doc->measureLengthAt(maxMeasure) - 1.0);
                start = std::min(start, std::max(0.0, maxV - m_timeDragBaseLength));
                m_timeSelStart = start;
                m_timeSelLength = m_timeDragBaseLength;
                break;
            }
            case TimeDragHandle::None:
                break;
        }

        const double lower = m_timeSelStart;
        const double upper = m_timeSelStart + m_timeSelLength;
        for (BmsNote& n : m_doc->notes) {
            if (n.vPosition < 0.0) {
                continue;
            }
            if (m_ntInput && n.length > 0.0) {
                n.selected = (n.vPosition < upper) && ((n.vPosition + n.length) >= lower);
            } else {
                n.selected = n.vPosition >= lower && n.vPosition < upper;
            }
        }
        emit timeSelectionChanged(m_timeSelStart, m_timeSelLength);
        emitSelectionChanged();
        update();
        return;
    }

    if (m_mode == EditMode::Write && m_ntInput && m_ntWriting && (event->buttons() & Qt::LeftButton)) {
        if (m_ntEditingIndex >= 0 && m_ntEditingIndex < m_doc->notes.size()) {
            const bool applySnap = !event->modifiers().testFlag(Qt::ControlModifier);
            const double now = std::max(0.0, yToVPos(event->position().y(), applySnap));
            BmsNote& n = m_doc->notes[m_ntEditingIndex];

            if (now >= m_ntPressVPos) {
                n.vPosition = m_ntPressVPos;
                n.length = now - m_ntPressVPos;
            } else {
                n.vPosition = now;
                n.length = m_ntPressVPos - now;
            }
            const int maxMeasure = std::max(0, static_cast<int>(m_doc->measureBottom.size()) - 1);
            const double maxV = std::max(0.0, m_doc->measureBottomAt(maxMeasure) + m_doc->measureLengthAt(maxMeasure) - 1.0);
            if (n.vPosition < 0.0) {
                n.length += n.vPosition;
                n.vPosition = 0.0;
            }
            if (n.vPosition + n.length > maxV) {
                n.length = std::max(0.0, maxV - n.vPosition);
            }
            n.longNote = n.length > 0.0 ? true : n.longNote;
            ensureSorted();
            emitSelectionChanged();
            emit documentEdited();
            update();
        }
        return;
    }

    if (m_mode == EditMode::Write && event->buttons() == Qt::NoButton) {
        const int col = columnAtX(event->position().x());
        const bool applySnap = !event->modifiers().testFlag(Qt::ControlModifier);
        const double v = std::max(0.0, yToVPos(event->position().y(), applySnap));
        const bool valid = isColumnEditable(col);
        const bool hoverChanged =
            (m_hoverValid != valid) ||
            (m_hoverColumn != col) ||
            (std::abs(m_hoverVPos - v) >= 0.0001);
        m_hoverValid = valid;
        m_hoverColumn = col;
        m_hoverVPos = v;
        if (hoverChanged || needsRepaint) {
            update();
        }
        return;
    }

    if (m_mode == EditMode::Write && m_writeDragging && (event->buttons() & m_writeDragButton)) {
        if (applyWriteAt(event->position(), m_writeDragButton, event->modifiers())) {
            emit documentEdited();
        }
        update();
        return;
    }

    if (needsRepaint) {
        update();
    }
    QWidget::mouseMoveEvent(event);
}

void ChartEditorWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (m_dragSelecting && event->button() == Qt::LeftButton) {
        m_dragSelecting = false;
        emitSelectionChanged();
        update();
        return;
    }

    if (m_selectDragActive && event->button() == Qt::LeftButton) {
        const bool moved = m_selectDragMoved;
        m_selectDragActive = false;
        m_selectDragMoved = false;
        m_selectDragBaseNotes.clear();
        m_selectDragAnchorCol = -1;
        m_selectDragAnchorVPos = 0.0;
        m_duplicateDragMode = false;
        m_duplicateCreated = false;

        if (m_toggleOnRelease && !moved && m_toggleNoteIndex >= 0 && m_toggleNoteIndex < m_doc->notes.size()) {
            m_doc->notes[m_toggleNoteIndex].selected = !m_doc->notes[m_toggleNoteIndex].selected;
            emitSelectionChanged();
            update();
        } else if (moved) {
            emit documentEdited();
            emitSelectionChanged();
            updateGeometry();
            update();
        }

        m_toggleOnRelease = false;
        m_toggleNoteIndex = -1;
        return;
    }

    if (m_timeSelecting && event->button() == Qt::LeftButton) {
        m_timeSelecting = false;
        m_timeDragHandle = TimeDragHandle::None;
        emit timeSelectionChanged(m_timeSelStart, m_timeSelLength);
        emitSelectionChanged();
        update();
        return;
    }

    if (m_mode == EditMode::Write && (event->button() == Qt::LeftButton || event->button() == Qt::RightButton)) {
        m_writeDragging = false;
        m_writeDragButton = Qt::NoButton;
        m_lastWriteCol = -1;
        m_lastWriteVQuant = std::numeric_limits<qint64>::min();
    }
    if (m_mode == EditMode::Write && event->button() == Qt::LeftButton) {
        m_ntWriting = false;
        m_ntEditingIndex = -1;
        m_ntEditingExisting = false;
        m_ntPressVPos = 0.0;
    }

    QWidget::mouseReleaseEvent(event);
}

void ChartEditorWidget::keyPressEvent(QKeyEvent* event) {
    if (!m_doc) {
        return;
    }

    const double step = event->modifiers().testFlag(Qt::ControlModifier) ? 1.0 : (192.0 / std::max(1, m_snapDivide));

    if (event->key() == Qt::Key_A &&
        event->modifiers().testFlag(Qt::ControlModifier) &&
        event->modifiers().testFlag(Qt::ShiftModifier) &&
        m_hoverNoteIndex >= 0 && m_hoverNoteIndex < m_doc->notes.size()) {
        const int label = m_doc->notes[m_hoverNoteIndex].value;
        for (BmsNote& n : m_doc->notes) {
            if (n.vPosition < 0.0) {
                continue;
            }
            if (n.value == label) {
                n.selected = true;
            }
        }
        emitSelectionChanged();
        update();
        event->accept();
        return;
    }

    if (event->matches(QKeySequence::SelectAll)) {
        for (BmsNote& n : m_doc->notes) {
            if (n.vPosition >= 0) {
                n.selected = true;
            }
        }
        emitSelectionChanged();
        update();
        event->accept();
        return;
    }

    switch (event->key()) {
        case Qt::Key_Delete:
        case Qt::Key_Backspace:
            if (selectedCount() > 0) {
                emit aboutToEdit();
                removeSelectedNotes();
                emit documentEdited();
                emitSelectionChanged();
                update();
            }
            event->accept();
            return;
        case Qt::Key_Up:
            emit aboutToEdit();
            moveSelectedNotes(0, -step);
            emit documentEdited();
            update();
            event->accept();
            return;
        case Qt::Key_Down:
            emit aboutToEdit();
            moveSelectedNotes(0, step);
            emit documentEdited();
            update();
            event->accept();
            return;
        case Qt::Key_Left:
            emit aboutToEdit();
            moveSelectedNotes(-1, 0.0);
            emit documentEdited();
            update();
            event->accept();
            return;
        case Qt::Key_Right:
            emit aboutToEdit();
            moveSelectedNotes(1, 0.0);
            emit documentEdited();
            update();
            event->accept();
            return;
        case Qt::Key_D:
            m_disableVerticalMove = !m_disableVerticalMove;
            event->accept();
            return;
        default:
            break;
    }

    QWidget::keyPressEvent(event);
}

void ChartEditorWidget::focusInEvent(QFocusEvent* event) {
    emit focusEntered(this);
    QWidget::focusInEvent(event);
}

void ChartEditorWidget::wheelEvent(QWheelEvent* event) {
    const bool zoomModifier =
        event->modifiers().testFlag(Qt::ControlModifier) ||
        event->modifiers().testFlag(Qt::MetaModifier) ||
        QApplication::keyboardModifiers().testFlag(Qt::ControlModifier) ||
        QApplication::keyboardModifiers().testFlag(Qt::MetaModifier);
    if (zoomModifier) {
        double raw = 0.0;
        if (!event->pixelDelta().isNull()) {
            raw = static_cast<double>(event->pixelDelta().y()) / 30.0;
        } else if (event->angleDelta().y() != 0) {
            raw = static_cast<double>(event->angleDelta().y()) / 120.0;
        }
        raw = std::clamp(raw, -8.0, 8.0);
        if (std::abs(raw) < 0.0001) {
            event->accept();
            return;
        }
        captureZoomAnchorFromGlobal(event->globalPosition().toPoint());
        const double factor = std::exp(raw * 0.12);
        applyZoomWithAnchor(m_zoom * factor);
        event->accept();
        return;
    }

    event->ignore();
}

bool ChartEditorWidget::event(QEvent* event) {
    if (event->type() == QEvent::Leave) {
        if (m_hoverValid) {
            m_hoverValid = false;
        }
        m_hoverNoteIndex = -1;
        update();
    }

    if (event->type() == QEvent::NativeGesture) {
        auto* gesture = static_cast<QNativeGestureEvent*>(event);
        if (gesture->gestureType() == Qt::BeginNativeGesture) {
            m_pinchZoomActive = true;
            captureZoomAnchorFromGlobal(gesture->globalPosition().toPoint());
            event->accept();
            return true;
        }
        if (gesture->gestureType() == Qt::EndNativeGesture) {
            m_pinchZoomActive = false;
            event->accept();
            return true;
        }
        if (gesture->gestureType() == Qt::ZoomNativeGesture) {
            if (!m_pinchZoomActive) {
                m_pinchZoomActive = true;
                captureZoomAnchorFromGlobal(gesture->globalPosition().toPoint());
            }
            const double delta = std::clamp(gesture->value(), -5.0, 5.0);
            // macOS pinch semantics: out => positive => zoom in.
            const double target = m_zoom * std::exp(delta * 0.95);
            applyZoomWithAnchor(target);
            event->accept();
            return true;
        }
    }

    if (event->type() == QEvent::Wheel) {
        auto* wheel = static_cast<QWheelEvent*>(event);
        if (wheel->modifiers().testFlag(Qt::ControlModifier)) {
            return QWidget::event(event);
        }

        if (!wheel->pixelDelta().isNull()) {
            auto* scrollArea = qobject_cast<QScrollArea*>(parentWidget() ? parentWidget()->parentWidget() : nullptr);
            if (!scrollArea) {
                scrollArea = qobject_cast<QScrollArea*>(parentWidget());
            }
            if (scrollArea) {
                m_accumulatedTrackpadDelta += QPointF(wheel->pixelDelta());

                if (auto* vbar = scrollArea->verticalScrollBar()) {
                    vbar->setValue(vbar->value() - static_cast<int>(std::round(m_accumulatedTrackpadDelta.y())));
                    m_accumulatedTrackpadDelta.setY(0.0);
                }
                if (auto* hbar = scrollArea->horizontalScrollBar()) {
                    hbar->setValue(hbar->value() - static_cast<int>(std::round(m_accumulatedTrackpadDelta.x())));
                    m_accumulatedTrackpadDelta.setX(0.0);
                }
                event->accept();
                return true;
            }
        }
    }

    return QWidget::event(event);
}

} // namespace ibmsc
