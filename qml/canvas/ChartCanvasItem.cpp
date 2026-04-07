#include "ChartCanvasItem.h"
#include "editor/EditorDocument.h"
#include "audio/AudioEngine.h"
#include "model/BmsDocument.h"
#include "model/ChannelType.h"
#include "app/AppSettings.h"
#include "editor/commands/MoveNoteCommand.h"
#include "editor/commands/MoveNotesCommand.h"
#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>
#include <QSGSimpleRectNode>
#include <QCursor>
#include <QMouseEvent>
#include <QHoverEvent>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QNativeGestureEvent>
#include <cmath>

// ── Column layout constants ─────────────────────────────────────────────────
static const double kColumnSpacing = 2.0;
static const double kRulerWidth = 40.0;

// ── Color palette ────────────────────────────────────────────────────────────
static QColor colBg(30, 30, 35);
static QColor colColumnBg(40, 42, 54);
static QColor colBeatLine(60, 62, 70);
static QColor colMeasureLine(100, 105, 120);
static QColor colNote(97, 175, 239);
static QColor colNoteLong(152, 195, 121);
static QColor colNoteMine(224, 108, 117);
static QColor colNoteHidden(97, 175, 239, 90);
static QColor colCursor(255, 220, 50, 200);
static QColor colBpmMarker(229, 192, 123);
static QColor colTimeSelection(255, 140, 70, 55);

ChartCanvasItem::ChartCanvasItem(QQuickItem* parent)
    : QQuickItem(parent) {
    setFlag(ItemHasContents, true);
    setFlag(ItemAcceptsInputMethod, true);
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
    setAcceptHoverEvents(true);
    setFocus(true);
}

void ChartCanvasItem::setEditMode(EditMode mode) {
    if (m_editMode == mode) return;
    m_editMode = mode;
    emit editModeChanged();
    switch (m_editMode) {
        case SelectMode: setCursor(Qt::ArrowCursor); break;
        case WriteMode: setCursor(Qt::CrossCursor); break;
        case EraserMode: setCursor(Qt::ForbiddenCursor); break;
        case TimeSelectMode: setCursor(Qt::SplitVCursor); break;
    }
}

void ChartCanvasItem::setTimeSelectionStartBeat(double beat) {
    if (std::abs(m_timeSelectionStartBeat - beat) < 1e-9) return;
    m_timeSelectionStartBeat = beat;
    emit timeSelectionRangeChanged();
    update();
}

void ChartCanvasItem::setTimeSelectionEndBeat(double beat) {
    if (std::abs(m_timeSelectionEndBeat - beat) < 1e-9) return;
    m_timeSelectionEndBeat = beat;
    emit timeSelectionRangeChanged();
    update();
}

void ChartCanvasItem::setScrollBeat(double v) {
    v = std::max(0.0, v);
    if (m_scrollBeat == v) return;
    m_scrollBeat = v;
    emit scrollBeatChanged();
    update();
}

void ChartCanvasItem::setPxPerBeat(double v) {
    if (m_pxPerBeat == v) return;
    m_pxPerBeat = std::clamp(v, 20.0, 400.0);
    emit pxPerBeatChanged();
    update();
}

void ChartCanvasItem::setGridSubdivision(int v) {
    if (m_gridSubdivision == v) return;
    m_gridSubdivision = v;
    emit gridSubdivisionChanged();
    update();
}

void ChartCanvasItem::setPlaybackBeat(double v) {
    m_playbackBeat = v;
    emit playbackBeatChanged();
    // Auto-scroll: keep cursor in the bottom 20% threshold
    double visibleBeats = height() / m_pxPerBeat;
    double cursorY = beatToY(v);
    if (cursorY > height() * 0.8) {
        setScrollBeat(v - visibleBeats * 0.5);
    }
    update();
}

void ChartCanvasItem::setDocument(QObject* doc) {
    if (m_document) {
        disconnect(m_document, nullptr, this, nullptr);
    }
    m_document = doc;
    if (m_document) {
        connect(m_document, SIGNAL(documentChanged()), this, SLOT(update()));
    }
    emit documentChanged();
    update();
}

void ChartCanvasItem::setAudioEngine(QObject* engine) {
    if (m_audioEngine == engine) return;
    m_audioEngine = engine;
    emit audioEngineChanged();
    update();
}

void ChartCanvasItem::setShowWaveform(bool v) {
    if (m_showWaveform == v) return;
    m_showWaveform = v;
    emit showWaveformChanged();
    auto* ae = qobject_cast<Audio::AudioEngine*>(m_audioEngine);
    if (ae) ae->setShowWaveform(v);
    update();
}

bool ChartCanvasItem::isBGroupChannel(int channel) const {
    return channel == 1 || (channel >= 101 && channel <= 132);
}

QString ChartCanvasItem::laneTitleForChannel(int channel) const {
    const auto layout = buildColumnLayout();
    for (const auto& col : layout) {
        if (col.identifier == channel) return col.title;
    }
    if (channel == 1) return QStringLiteral("B1");
    if (channel >= 101) return QString("B%1").arg(channel - 99);
    return QString("%1").arg(channel);
}

std::vector<ColumnLayout> ChartCanvasItem::buildColumnLayout() const {
    std::vector<ColumnLayout> layout;
    layout.reserve(32);
    const auto configs = App::AppSettings::instance()->columnConfigs();
    double xPos = kRulerWidth;
    for (const auto& cfg : configs) {
        if (!cfg.enabled || !cfg.isNoteColumn) continue;
        ColumnLayout cl;
        cl.identifier = cfg.identifier;
        cl.x = xPos;
        cl.width = static_cast<double>(cfg.width);
        cl.title = cfg.title;
        cl.isSound = cfg.isSound;
        layout.push_back(cl);
        xPos += cl.width + kColumnSpacing;
    }

    // Append dynamic B-group columns for BGM tracks beyond B1
    int maxB = 1;
    if (m_document) {
        auto* edDoc = qobject_cast<Editor::EditorDocument*>(m_document);
        if (edDoc) {
            for (const auto& n : edDoc->document().notes) {
                if (isBGroupChannel(n.channelIndex) && n.channelIndex > 1)
                    maxB = std::max(maxB, n.channelIndex);
            }
        }
    }
    // Add extra B columns (101+) if document uses them
    if (maxB > 1) {
        for (int ch = 101; ch <= std::min(132, maxB + 1); ++ch) {
            ColumnLayout cl;
            cl.identifier = ch;
            cl.x = xPos;
            cl.width = 40.0;
            cl.title = QString("B%1").arg(ch - 99);
            cl.isSound = true;
            layout.push_back(cl);
            xPos += cl.width + kColumnSpacing;
        }
    }
    return layout;
}

void ChartCanvasItem::geometryChange(const QRectF& newGeom, const QRectF& oldGeom) {
    QQuickItem::geometryChange(newGeom, oldGeom);
    update();
}

// Beat-to-pixel conversion: beat 0 is at bottom, beats increase upward
double ChartCanvasItem::beatToY(double beat) const {
    return height() - (beat - m_scrollBeat) * m_pxPerBeat;
}
double ChartCanvasItem::yToBeat(double y) const {
    return m_scrollBeat + (height() - y) / m_pxPerBeat;
}
double ChartCanvasItem::snapBeat(double rawBeat) const {
    double grid = 1.0 / m_gridSubdivision;
    return std::round(rawBeat / grid) * grid;
}
int ChartCanvasItem::xToChannel(double x) const {
    const auto layout = buildColumnLayout();
    for (const auto& col : layout) {
        if (x >= col.x && x < col.x + col.width)
            return col.identifier;
    }
    return -1;
}
double ChartCanvasItem::channelToX(int ch) const {
    const auto layout = buildColumnLayout();
    for (const auto& col : layout) {
        if (col.identifier == ch) return col.x;
    }
    return -1.0;
}

// ── Scene graph rendering ────────────────────────────────────────────────────

QSGNode* ChartCanvasItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*) {
    // Root container node
    QSGNode* root = oldNode ? oldNode : new QSGNode();
    // Clear all children and rebuild (simple approach; optimise later with node reuse)
    root->removeAllChildNodes();

    const double w = width();
    const double h = height();
    const auto cols = buildColumnLayout();
    if (w <= 0 || h <= 0) return root;

    auto addRect = [&](double x, double y, double rw, double rh, QColor color) {
        auto* node = new QSGSimpleRectNode(QRectF(x, y, rw, rh), color);
        root->appendChildNode(node);
    };

    // ── Background ────────────────────────────────────────────────────────────
    addRect(0, 0, w, h, colBg);

    // ── Column backgrounds ────────────────────────────────────────────────────
    for (const auto& col : cols) {
        addRect(col.x, 0, col.width, h, colColumnBg);
    }

    // ── Beat and measure lines (viewport-culled) ──────────────────────────────
    double visibleBeats = h / m_pxPerBeat;
    double startBeat = m_scrollBeat - 1.0;
    double endBeat = m_scrollBeat + visibleBeats + 1.0;
    double gridStep = 1.0 / m_gridSubdivision;

    double beat = std::floor(startBeat / gridStep) * gridStep;
    while (beat <= endBeat) {
        double y = beatToY(beat);
        if (y >= -1 && y <= h + 1) {
            bool isMeasure = std::fmod(std::fabs(beat), 4.0) < 1e-9;
            double lh = isMeasure ? 2.0 : 1.0;
            QColor lc = isMeasure ? colMeasureLine : colBeatLine;
            addRect(kRulerWidth, y - lh * 0.5, w - kRulerWidth, lh, lc);
        }
        beat += gridStep;
    }

    if (m_timeSelectionStartBeat >= 0.0 && m_timeSelectionEndBeat >= 0.0) {
        double topY = beatToY(std::max(m_timeSelectionStartBeat, m_timeSelectionEndBeat));
        double botY = beatToY(std::min(m_timeSelectionStartBeat, m_timeSelectionEndBeat));
        if (botY >= -2.0 && topY <= h + 2.0) {
            addRect(kRulerWidth, topY, w - kRulerWidth, std::max(2.0, botY - topY), colTimeSelection);
        }
    }

    if (m_showWaveform) {
        auto* ae = qobject_cast<Audio::AudioEngine*>(m_audioEngine);
        auto* edDoc = qobject_cast<Editor::EditorDocument*>(m_document);
        if (ae && edDoc) {
            double maxBeat = 16.0;
            const auto& doc = edDoc->document();
            for (const auto& n : doc.notes) {
                double b = n.measureIndex * 4.0 + n.beat.toDouble() * 4.0 * doc.measureLength(n.measureIndex);
                maxBeat = std::max(maxBeat, b + n.durationInBeats);
            }

            const auto& globalPeaks = ae->slotWaveformPeaks(ae->globalPreviewSlot());
            if (!globalPeaks.empty()) {
                for (int y = 0; y < static_cast<int>(h); y += 3) {
                    double beatAtY = yToBeat(static_cast<double>(y));
                    if (beatAtY < 0.0 || beatAtY > maxBeat) continue;
                    int idx = static_cast<int>((beatAtY / maxBeat) * (globalPeaks.size() - 1));
                    idx = std::clamp(idx, 0, static_cast<int>(globalPeaks.size()) - 1);
                    double amp = std::clamp(static_cast<double>(globalPeaks[static_cast<size_t>(idx)]), 0.0, 1.0);
                    double half = amp * (kRulerWidth * 0.45);
                    double midX = kRulerWidth * 0.5;
                    addRect(midX - half, static_cast<double>(y), half * 2.0, 2.0, QColor(255, 170, 110, 110));
                }
            }

            for (const auto& col : cols) {
                int ch = col.identifier;
                if (!isBGroupChannel(ch)) continue;

                int slot = -1;
                for (const auto& n : doc.notes) {
                    if (n.channelIndex == ch && n.value > 0) {
                        slot = n.value;
                        break;
                    }
                }
                if (slot < 1) continue;

                const auto& peaks = ae->slotWaveformPeaks(slot);
                if (peaks.empty()) continue;

                for (int y = 0; y < static_cast<int>(h); y += 3) {
                    double beatAtY = yToBeat(static_cast<double>(y));
                    if (beatAtY < 0.0 || beatAtY > maxBeat) continue;
                    int idx = static_cast<int>((beatAtY / maxBeat) * (peaks.size() - 1));
                    idx = std::clamp(idx, 0, static_cast<int>(peaks.size()) - 1);
                    double amp = std::clamp(static_cast<double>(peaks[static_cast<size_t>(idx)]), 0.0, 1.0);
                    double half = amp * ((col.width - 4.0) * 0.45);
                    double midX = col.x + col.width * 0.5;
                    addRect(midX - half, static_cast<double>(y), half * 2.0, 2.0, QColor(140, 205, 255, 90));
                }
            }
        }
    }

    // ── Notes (viewport-culled) ───────────────────────────────────────────────
    if (m_document) {
        auto* edDoc = qobject_cast<Editor::EditorDocument*>(m_document);
        if (edDoc) {
            double dragDeltaBeat = 0;
            int dragDeltaChOffset = 0;
            if (m_dragMode == DragMoveNotes) {
                double endBeat = snapBeat(yToBeat(m_dragCurrent.y()));
                int endCh = xToChannel(m_dragCurrent.x());
                if (endCh >= 0) {
                    dragDeltaBeat = endBeat - m_dragStartBeat;
                    int startIndex = -1, endIndex = -1;
                    for (int i = 0; i < static_cast<int>(cols.size()); ++i) {
                        if (cols[static_cast<size_t>(i)].identifier == m_dragStartChannel) startIndex = i;
                        if (cols[static_cast<size_t>(i)].identifier == endCh) endIndex = i;
                    }
                    if (startIndex >= 0 && endIndex >= 0) {
                        dragDeltaChOffset = endIndex - startIndex;
                    }
                }
            }

            const auto& doc = edDoc->document();
            for (const auto& note : doc.notes) {
                using namespace Model;
                ChannelType ct = classifyChannel(note.channelIndex);
                if (!isVisibleChannel(ct)) continue;

                double noteBeat = note.measureIndex * 4.0 + note.beat.toDouble() * 4.0 * doc.measureLength(note.measureIndex);
                int chIndex = -1;
                for (int i = 0; i < static_cast<int>(cols.size()); ++i) {
                    if (cols[static_cast<size_t>(i)].identifier == note.channelIndex) { chIndex = i; break; }
                }
                if (chIndex < 0) continue;

                if (note.selected && m_dragMode == DragMoveNotes) {
                    noteBeat += dragDeltaBeat;
                    chIndex = std::clamp(chIndex + dragDeltaChOffset, 0, static_cast<int>(cols.size()) - 1);
                }

                if (noteBeat < startBeat || noteBeat > endBeat) continue;

                double cx = cols[static_cast<size_t>(chIndex)].x;
                double cw = cols[static_cast<size_t>(chIndex)].width;
                double ny = beatToY(noteBeat);
                double noteEndBeat = noteBeat + std::max(0.0, note.durationInBeats);
                double nyTail = beatToY(noteEndBeat);
                QColor nc = note.landmine ? colNoteMine
                          : note.hidden ? colNoteHidden
                          : (note.durationInBeats > 0.0) ? colNoteLong
                          : colNote;

                if (note.durationInBeats > 0.0) {
                    double topY = std::min(ny, nyTail);
                    double botY = std::max(ny, nyTail);
                    if (botY >= -12 && topY <= h + 12) {
                        addRect(cx + 10, topY, cw - 20, std::max(2.0, botY - topY), QColor(nc.red(), nc.green(), nc.blue(), 160));
                        addRect(cx + 1, nyTail - 6, cw - 2, 12, nc.darker(115));
                    }
                }

                if (note.selected) {
                    nc = nc.lighter(150);
                    // Add a slight border/halo for selected notes
                    addRect(cx, ny - 7, cw, 14, QColor(255, 255, 255, 100));
                }
                addRect(cx + 1, ny - 6, cw - 2, 12, nc);
            }
        }
    }

    if (m_dragMode == DragSelect && m_editMode != TimeSelectMode) {
        double rx = std::min(m_dragStart.x(), m_dragCurrent.x());
        double ry = std::min(m_dragStart.y(), m_dragCurrent.y());
        double rw = std::abs(m_dragStart.x() - m_dragCurrent.x());
        double rh = std::abs(m_dragStart.y() - m_dragCurrent.y());
        addRect(rx, ry, rw, rh, QColor(255, 255, 255, 60));
    }

    // ── Note placement preview (ghost note while dragging) ────────────────────
    if (m_dragMode == DragPlace && m_dragStartChannel >= 0) {
        double previewBeat = m_dragStartBeat;
        double endBeat = snapBeat(yToBeat(m_dragCurrent.y()));
        double cx = channelToX(m_dragStartChannel);
        if (cx >= 0) {
            const auto& col = [&]() -> const ColumnLayout& {
                static ColumnLayout dummy;
                for (const auto& c : cols) {
                    if (c.identifier == m_dragStartChannel) return c;
                }
                return dummy;
            }();
            double cw = col.width > 0 ? col.width : 48.0;
            double ny = beatToY(previewBeat);
            double duration = std::max(0.0, endBeat - previewBeat);
            if (duration > (1.0 / std::max(1, m_gridSubdivision))) {
                // Long note preview
                double nyTail = beatToY(endBeat);
                double topY = std::min(ny, nyTail);
                double botY = std::max(ny, nyTail);
                addRect(cx + 10, topY, cw - 20, std::max(2.0, botY - topY), QColor(152, 195, 121, 80));
                addRect(cx + 1, nyTail - 6, cw - 2, 12, QColor(152, 195, 121, 120));
            }
            // Head
            addRect(cx + 1, ny - 6, cw - 2, 12, QColor(97, 175, 239, 140));
        }
    }

    // ── Hover highlight ───────────────────────────────────────────────────────
    if (m_hoverX >= 0 && m_hoverY >= 0 && m_document) {
        // Column hover: highlight the column under cursor
        int hovCh = xToChannel(m_hoverX);
        if (hovCh >= 0) {
            for (const auto& col : cols) {
                if (col.identifier == hovCh) {
                    addRect(col.x, 0, col.width, h, QColor(255, 255, 255, 12));
                    break;
                }
            }
        }
        // Note hover: bright outline
        const Model::Note* hovered = hitTestNote(m_hoverX, m_hoverY);
        if (hovered) {
            int chIdx = -1;
            for (int i = 0; i < static_cast<int>(cols.size()); ++i) {
                if (cols[static_cast<size_t>(i)].identifier == hovered->channelIndex) { chIdx = i; break; }
            }
            if (chIdx >= 0) {
                double cx = cols[static_cast<size_t>(chIdx)].x;
                double cw = cols[static_cast<size_t>(chIdx)].width;
                auto* edDoc = qobject_cast<Editor::EditorDocument*>(m_document);
                const auto& doc = edDoc->document();
                double noteBeat = hovered->measureIndex * 4.0 + hovered->beat.toDouble() * doc.measureLength(hovered->measureIndex) * 4.0;
                double ny = beatToY(noteBeat);
                // Bright white border
                addRect(cx - 2, ny - 8, cw + 4, 16, QColor(255, 255, 255, 180));
                // Slightly lighter note on top
                addRect(cx + 1, ny - 6, cw - 2, 12, QColor(255, 255, 255, 60));
            }
        }
    }

    // ── Playback cursor ────────────────────────────────────────────────────────
    if (m_playbackBeat >= 0) {
        double cy = beatToY(m_playbackBeat);
        if (cy >= 0 && cy <= h)
            addRect(kRulerWidth, cy - 1, w - kRulerWidth, 3, colCursor);
    }

    return root;
}

// ── Mouse interaction ────────────────────────────────────────────────────────

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
        double tailBeat = noteBeat + std::max(0.0, note.durationInBeats);
        double ny = beatToY(noteBeat);
        double nyTail = beatToY(tailBeat);
        if (y >= ny - 6 && y <= ny + 6) {
            return &note;
        }
        if (note.durationInBeats > 0.0) {
            double topY = std::min(ny, nyTail);
            double botY = std::max(ny, nyTail);
            if (y >= topY && y <= botY) {
                return &note;
            }
        }
    }
    return nullptr;
}

void ChartCanvasItem::hoverMoveEvent(QHoverEvent* e) {
    m_hoverX = e->position().x();
    m_hoverY = e->position().y();
    switch (m_editMode) {
        case SelectMode: setCursor(Qt::ArrowCursor); break;
        case WriteMode: setCursor(Qt::CrossCursor); break;
        case EraserMode: setCursor(Qt::ForbiddenCursor); break;
        case TimeSelectMode: setCursor(Qt::SplitVCursor); break;
    }
    update(); // repaint to show hover highlight
}

void ChartCanvasItem::doRubberBandSelection() {
    if (!m_document) return;
    auto* edDoc = qobject_cast<Editor::EditorDocument*>(m_document);
    if (!edDoc) return;

    double y1 = std::min(m_dragStart.y(), m_dragCurrent.y());
    double y2 = std::max(m_dragStart.y(), m_dragCurrent.y());
    double b1 = yToBeat(y2); // max Y is lowest beat
    double b2 = yToBeat(y1); // min Y is highest beat

    double x1 = std::min(m_dragStart.x(), m_dragCurrent.x());
    double x2 = std::max(m_dragStart.x(), m_dragCurrent.x());
    int minCol = -1, maxCol = -1;
    const auto cols = buildColumnLayout();
    for (int i = 0; i < static_cast<int>(cols.size()); ++i) {
        if (cols[static_cast<size_t>(i)].x + cols[static_cast<size_t>(i)].width > x1 && cols[static_cast<size_t>(i)].x < x2) {
            if (minCol < 0) minCol = i;
            maxCol = i;
        }
    }

    auto& notes = edDoc->document().notes;
    for (auto& note : notes) {
        double noteBeat = note.measureIndex * 4.0 + note.beat.toDouble() * 4.0 * edDoc->document().measureLength(note.measureIndex);

        bool inRange = false;
        if (minCol >= 0) {
            for (int i = minCol; i <= maxCol; ++i) {
                if (cols[static_cast<size_t>(i)].identifier == note.channelIndex) {
                    if (noteBeat >= b1 && noteBeat <= b2) {
                        inRange = true;
                    }
                    break;
                }
            }
        }
        // If Shift is pressed, maybe append selection, but for simplicity let's just set
        note.selected = inRange;
    }
    edDoc->notifySelectionChanged();
}

void ChartCanvasItem::commitNoteMove() {
    if (!m_document || m_dragMode != DragMoveNotes) return;
    auto* edDoc = qobject_cast<Editor::EditorDocument*>(m_document);
    if (!edDoc) return;

    double endBeat = snapBeat(yToBeat(m_dragCurrent.y()));
    int endCh = xToChannel(m_dragCurrent.x());
    if (endCh < 0) endCh = m_dragStartChannel;

    double deltaBeat = endBeat - m_dragStartBeat;
    int deltaChOffset = 0;

    const auto cols = buildColumnLayout();
    int startIndex = -1, endIndex = -1;
    for (int i = 0; i < static_cast<int>(cols.size()); ++i) {
        if (cols[static_cast<size_t>(i)].identifier == m_dragStartChannel) startIndex = i;
        if (cols[static_cast<size_t>(i)].identifier == endCh) endIndex = i;
    }
    if (startIndex >= 0 && endIndex >= 0) {
        deltaChOffset = endIndex - startIndex;
    }

    if (std::abs(deltaBeat) < 1e-9 && deltaChOffset == 0) {
        return; // nothing moved
    }

    std::vector<Editor::Commands::MoveNotesCommand::MovedNote> moves;
    const auto& doc = edDoc->document();
    for (const auto& note : doc.notes) {
        if (!note.selected) continue;

        Editor::Commands::MoveNotesCommand::MovedNote move;
        move.before = note;

        double oldBeat = note.measureIndex * 4.0 + note.beat.toDouble() * 4.0 * doc.measureLength(note.measureIndex);
        double newBeat = oldBeat + deltaBeat;
        if (newBeat < 0) newBeat = 0;

        int oldChIndex = -1;
        for (int i = 0; i < static_cast<int>(cols.size()); ++i) {
            if (cols[static_cast<size_t>(i)].identifier == note.channelIndex) { oldChIndex = i; break; }
        }
        int newChIndex = oldChIndex + deltaChOffset;
        newChIndex = std::clamp(newChIndex, 0, static_cast<int>(cols.size()) - 1);
        
        move.after = note;
        move.after.channelIndex = cols[static_cast<size_t>(newChIndex)].identifier;
        
        int targetMeasure = 0;
        double accum = 0.0;
        while (true) {
            double len = doc.measureLength(targetMeasure) * 4.0;
            if (accum + len > newBeat - 1e-9) {
                break;
            }
            accum += len;
            targetMeasure++;
        }
        double beatInMeasure = newBeat - accum;
        move.after.measureIndex = targetMeasure;
        
        double normalized = beatInMeasure / (doc.measureLength(targetMeasure) * 4.0);
        int den = m_gridSubdivision * 4;
        int num = static_cast<int>(std::round(normalized * den));
        
        move.after.beat = Model::BeatFraction{num, den};
        moves.push_back(move);
    }

    if (!moves.empty()) {
        if (moves.size() == 1) {
            edDoc->undoStack()->push(new Editor::Commands::MoveNoteCommand(edDoc, moves.front().before, moves.front().after));
        } else {
            edDoc->undoStack()->push(new Editor::Commands::MoveNotesCommand(edDoc, std::move(moves)));
        }
    }
}

void ChartCanvasItem::mousePressEvent(QMouseEvent* e) {
    if (!m_document) return;
    auto* edDoc = qobject_cast<Editor::EditorDocument*>(m_document);

    m_dragStart = e->position();
    m_dragCurrent = e->position();
    m_dragStartBeat = snapBeat(yToBeat(e->position().y()));
    m_dragStartChannel = xToChannel(e->position().x());

    const Model::Note* hitNote = hitTestNote(e->position().x(), e->position().y());

    // Play keysound on click: always in SelectMode or WriteMode (hitting existing note),
    // or when previewOnClick is on in other modes
    bool shouldPreview = false;
    if (hitNote && hitNote->value > 0 && m_audioEngine) {
        auto* ae = qobject_cast<Audio::AudioEngine*>(m_audioEngine);
        if (ae) {
            shouldPreview = (m_editMode == SelectMode || m_editMode == WriteMode);
            if (!shouldPreview) {
                auto* settings = App::AppSettings::instance();
                shouldPreview = settings->previewOnClick();
            }
            if (shouldPreview) ae->play(hitNote->value);
        }
    }

    // WriteMode: clicking an existing note previews + selects it, never places on top
    if (e->button() == Qt::LeftButton && hitNote && m_editMode == WriteMode) {
        for (auto& note : edDoc->document().notes) note.selected = false;
        const_cast<Model::Note*>(hitNote)->selected = true;
        edDoc->notifySelectionChanged();
        m_dragMode = DragNone;
        update();
        e->accept();
        return;
    }

    if (e->button() == Qt::LeftButton) {
        if (m_editMode == EraserMode) {
            m_dragMode = DragDelete;
            if (hitNote) {
                emit noteDeleted(hitNote->channelIndex, hitNote->measureIndex, hitNote->beat.num, hitNote->beat.den);
                m_lastEraseChannel = hitNote->channelIndex;
                m_lastEraseMeasure = hitNote->measureIndex;
                m_lastEraseBeatNum = hitNote->beat.num;
                m_lastEraseBeatDen = hitNote->beat.den;
            }
        } else if (m_editMode == WriteMode) {
            m_dragMode = DragPlace;
        } else if (m_editMode == TimeSelectMode) {
            m_dragMode = DragSelect; // reuse drag for visual feedback
            setTimeSelectionStartBeat(m_dragStartBeat);
            setTimeSelectionEndBeat(m_dragStartBeat);
        } else {
            // Check if clicking near the tail of a long note for resize
            bool startResize = false;
            if (hitNote && hitNote->durationInBeats > 0.0 && edDoc) {
                const auto& doc = edDoc->document();
                double noteBeat = hitNote->measureIndex * 4.0 + hitNote->beat.toDouble() * doc.measureLength(hitNote->measureIndex) * 4.0;
                double tailBeat = noteBeat + hitNote->durationInBeats;
                double tailY = beatToY(tailBeat);
                double headY = beatToY(noteBeat);
                double mouseY = e->position().y();
                // If within 8px of tail, resize from tail
                if (std::abs(mouseY - tailY) < 8.0) {
                    m_dragMode = DragResizeLN;
                    m_resizeOrigNote = *hitNote;
                    m_resizeFromTail = true;
                    startResize = true;
                }
                // If within 8px of head, resize from head
                else if (std::abs(mouseY - headY) < 8.0 && hitNote->durationInBeats > 0.0) {
                    m_dragMode = DragResizeLN;
                    m_resizeOrigNote = *hitNote;
                    m_resizeFromTail = false;
                    startResize = true;
                }
            }
            if (!startResize) {
                if ((e->modifiers() & Qt::ShiftModifier) || !hitNote) {
                    m_dragMode = DragSelect;
                    for (auto& n : edDoc->document().notes) n.selected = false;
                    edDoc->notifySelectionChanged();
                } else {
                    m_dragMode = DragMoveNotes;
                    if (!hitNote->selected) {
                        for (auto& n : edDoc->document().notes) n.selected = false;
                        const_cast<Model::Note*>(hitNote)->selected = true;
                        edDoc->notifySelectionChanged();
                    }
                }
            }
        }
    } else if (e->button() == Qt::RightButton) {
        if (hitNote) {
            emit noteDeleted(hitNote->channelIndex, hitNote->measureIndex, hitNote->beat.num, hitNote->beat.den);
        }
    }
    update();
}

void ChartCanvasItem::mouseMoveEvent(QMouseEvent* e) {
    if (m_dragMode != DragNone) {
        m_dragCurrent = e->position();
        if (m_dragMode == DragSelect) {
            if (m_editMode == TimeSelectMode) {
                setTimeSelectionEndBeat(snapBeat(yToBeat(e->position().y())));
            } else {
                doRubberBandSelection();
            }
        } else if (m_dragMode == DragDelete && m_editMode == EraserMode) {
            const Model::Note* hitNote = hitTestNote(e->position().x(), e->position().y());
            if (hitNote) {
                bool sameAsLast = hitNote->channelIndex == m_lastEraseChannel
                               && hitNote->measureIndex == m_lastEraseMeasure
                               && hitNote->beat.num == m_lastEraseBeatNum
                               && hitNote->beat.den == m_lastEraseBeatDen;
                if (!sameAsLast) {
                    emit noteDeleted(hitNote->channelIndex, hitNote->measureIndex, hitNote->beat.num, hitNote->beat.den);
                    m_lastEraseChannel = hitNote->channelIndex;
                    m_lastEraseMeasure = hitNote->measureIndex;
                    m_lastEraseBeatNum = hitNote->beat.num;
                    m_lastEraseBeatDen = hitNote->beat.den;
                }
            }
        }
        update();
    }
}

void ChartCanvasItem::mouseReleaseEvent(QMouseEvent* e) {
    if (m_dragMode == DragMoveNotes) {
        commitNoteMove();
    } else if (m_dragMode == DragResizeLN) {
        // Commit LN resize
        auto* edDoc = qobject_cast<Editor::EditorDocument*>(m_document);
        if (edDoc) {
            double mouseEndBeat = snapBeat(yToBeat(e->position().y()));
            const auto& doc = edDoc->document();
            double noteBeat = m_resizeOrigNote.measureIndex * 4.0
                + m_resizeOrigNote.beat.toDouble() * doc.measureLength(m_resizeOrigNote.measureIndex) * 4.0;
            double newDuration = m_resizeOrigNote.durationInBeats;
            if (m_resizeFromTail) {
                newDuration = std::max(1.0 / std::max(1, m_gridSubdivision), mouseEndBeat - noteBeat);
            } else {
                // Resize from head: move head, keep tail fixed
                double tailBeat = noteBeat + m_resizeOrigNote.durationInBeats;
                newDuration = std::max(1.0 / std::max(1, m_gridSubdivision), tailBeat - mouseEndBeat);
            }
            // Find and update the note via MoveNotesCommand (before/after)
            Editor::Commands::MoveNotesCommand::MovedNote mv;
            mv.before = m_resizeOrigNote;
            mv.after = m_resizeOrigNote;
            mv.after.durationInBeats = newDuration;
            if (!m_resizeFromTail) {
                // Head moved: update beat position
                double newHeadBeat = noteBeat + m_resizeOrigNote.durationInBeats - newDuration;
                int meas = 0; double remain = std::max(0.0, newHeadBeat);
                while (true) {
                    double len = doc.measureLength(meas) * 4.0;
                    if (remain < len - 1e-9) break;
                    remain -= len; ++meas;
                }
                int den = 192;
                double norm = remain / (doc.measureLength(meas) * 4.0);
                mv.after.measureIndex = meas;
                mv.after.beat = {static_cast<int32_t>(std::round(norm * den)), den};
            }
            std::vector<Editor::Commands::MoveNotesCommand::MovedNote> moves = {mv};
            edDoc->undoStack()->push(new Editor::Commands::MoveNotesCommand(edDoc, std::move(moves)));
        }
    } else if (m_dragMode == DragPlace && m_dragStartChannel >= 0) {
        double endBeat = snapBeat(yToBeat(e->position().y()));
        int measure = static_cast<int>(m_dragStartBeat / 4.0);
        double beatInMeasure = m_dragStartBeat - measure * 4.0;
        int beatNum = static_cast<int>(std::round(beatInMeasure * m_gridSubdivision));
        int beatDen = static_cast<int>(m_gridSubdivision * 4.0);
        double minLn = 1.0 / std::max(1, m_gridSubdivision);
        double duration = std::max(0.0, endBeat - m_dragStartBeat);
        if (duration >= minLn) {
            emit notePlacedWithDuration(m_dragStartChannel, measure, beatNum, beatDen, 1, duration);
        } else {
            emit notePlaced(m_dragStartChannel, measure, beatNum, beatDen, 1);
        }
    }
    m_dragMode = DragNone;
    m_lastEraseMeasure = -1;
    m_lastEraseBeatNum = -1;
    m_lastEraseBeatDen = -1;
    m_lastEraseChannel = -1;
    update();
}

void ChartCanvasItem::mouseDoubleClickEvent(QMouseEvent* e) {
    const Model::Note* hitNote = hitTestNote(e->position().x(), e->position().y());
    if (hitNote) {
        emit noteDoubleClicked(hitNote->channelIndex, hitNote->measureIndex,
                               hitNote->beat.num, hitNote->beat.den, hitNote->value);
        e->accept();
        return;
    }
    QQuickItem::mouseDoubleClickEvent(e);
}

void ChartCanvasItem::wheelEvent(QWheelEvent* e) {
    if (e->modifiers() & Qt::ControlModifier) {
        // Zoom centered on mouse position
        double dy = 0.0;
        if (!e->pixelDelta().isNull()) {
            dy = e->pixelDelta().y();
        } else {
            dy = e->angleDelta().y();
        }
        if (std::abs(dy) > 0.1) {
            double factor = 1.0 + dy * 0.005;
            factor = std::clamp(factor, 0.8, 1.25);
            // Beat under cursor before zoom
            double mouseY = e->position().y();
            double beatAtMouse = yToBeat(mouseY);
            double newPxPerBeat = std::clamp(m_pxPerBeat * factor, 20.0, 400.0);
            // After zoom, adjust scroll so beatAtMouse stays at same mouseY
            // mouseY = height() - (beatAtMouse - newScroll) * newPxPerBeat
            // newScroll = beatAtMouse - (height() - mouseY) / newPxPerBeat
            double newScroll = beatAtMouse - (height() - mouseY) / newPxPerBeat;
            m_pxPerBeat = newPxPerBeat;
            emit pxPerBeatChanged();
            setScrollBeat(std::max(0.0, newScroll));
            update();
        }
    } else {
        // Scroll: use pixelDelta for smooth trackpad, angleDelta for discrete wheel
        double dy = 0.0;
        if (!e->pixelDelta().isNull()) {
            dy = e->pixelDelta().y();
            // pixelDelta is in screen pixels — convert to beats
            double beatDelta = dy / m_pxPerBeat;
            setScrollBeat(m_scrollBeat + beatDelta);
        } else {
            double delta = e->angleDelta().y() / 120.0;
            setScrollBeat(m_scrollBeat + delta * (4.0 / m_gridSubdivision));
        }
    }
    e->accept();
}

bool ChartCanvasItem::event(QEvent* e) {
    if (e->type() == QEvent::NativeGesture) {
        auto* ge = static_cast<QNativeGestureEvent*>(e);
        if (ge->gestureType() == Qt::ZoomNativeGesture) {
            double factor = 1.0 + ge->value();
            factor = std::clamp(factor, 0.8, 1.25);
            double mouseY = ge->position().y();
            double beatAtMouse = yToBeat(mouseY);
            double newPxPerBeat = std::clamp(m_pxPerBeat * factor, 20.0, 400.0);
            double newScroll = beatAtMouse - (height() - mouseY) / newPxPerBeat;
            m_pxPerBeat = newPxPerBeat;
            emit pxPerBeatChanged();
            setScrollBeat(std::max(0.0, newScroll));
            update();
            e->accept();
            return true;
        }
    }
    return QQuickItem::event(e);
}

void ChartCanvasItem::keyPressEvent(QKeyEvent* e) {
    if (!m_document) { QQuickItem::keyPressEvent(e); return; }
    auto* edDoc = qobject_cast<Editor::EditorDocument*>(m_document);
    if (!edDoc) { QQuickItem::keyPressEvent(e); return; }

    bool hasSelection = false;
    for (const auto& n : edDoc->document().notes) {
        if (n.selected) { hasSelection = true; break; }
    }

    if (!hasSelection) { QQuickItem::keyPressEvent(e); return; }

    const auto cols = buildColumnLayout();
    double gridStep = 1.0 / std::max(1, m_gridSubdivision);
    bool ctrl = e->modifiers() & Qt::ControlModifier;
    double beatDelta = ctrl ? (1.0 / 192.0) : gridStep;

    switch (e->key()) {
    case Qt::Key_Up: {
        // Move selected notes up (increase beat)
        std::vector<Editor::Commands::MoveNotesCommand::MovedNote> moves;
        const auto& doc = edDoc->document();
        for (const auto& note : doc.notes) {
            if (!note.selected) continue;
            Editor::Commands::MoveNotesCommand::MovedNote mv;
            mv.before = note;
            mv.after = note;
            double absBeat = note.measureIndex * 4.0 + note.beat.toDouble() * doc.measureLength(note.measureIndex) * 4.0;
            double newBeat = absBeat + beatDelta;
            int meas = 0; double remain = newBeat;
            while (true) {
                double len = doc.measureLength(meas) * 4.0;
                if (remain < len - 1e-9) break;
                remain -= len; ++meas;
            }
            mv.after.measureIndex = meas;
            int den = 192;
            double norm = remain / (doc.measureLength(meas) * 4.0);
            mv.after.beat = {static_cast<int32_t>(std::round(norm * den)), den};
            moves.push_back(mv);
        }
        if (!moves.empty())
            edDoc->undoStack()->push(new Editor::Commands::MoveNotesCommand(edDoc, std::move(moves)));
        e->accept();
        update();
        return;
    }
    case Qt::Key_Down: {
        std::vector<Editor::Commands::MoveNotesCommand::MovedNote> moves;
        const auto& doc = edDoc->document();
        for (const auto& note : doc.notes) {
            if (!note.selected) continue;
            Editor::Commands::MoveNotesCommand::MovedNote mv;
            mv.before = note;
            mv.after = note;
            double absBeat = note.measureIndex * 4.0 + note.beat.toDouble() * doc.measureLength(note.measureIndex) * 4.0;
            double newBeat = std::max(0.0, absBeat - beatDelta);
            int meas = 0; double remain = newBeat;
            while (true) {
                double len = doc.measureLength(meas) * 4.0;
                if (remain < len - 1e-9) break;
                remain -= len; ++meas;
            }
            mv.after.measureIndex = meas;
            int den = 192;
            double norm = remain / (doc.measureLength(meas) * 4.0);
            mv.after.beat = {static_cast<int32_t>(std::round(norm * den)), den};
            moves.push_back(mv);
        }
        if (!moves.empty())
            edDoc->undoStack()->push(new Editor::Commands::MoveNotesCommand(edDoc, std::move(moves)));
        e->accept();
        update();
        return;
    }
    case Qt::Key_Left:
    case Qt::Key_Right: {
        int dir = (e->key() == Qt::Key_Left) ? -1 : 1;
        std::vector<Editor::Commands::MoveNotesCommand::MovedNote> moves;
        const auto& doc = edDoc->document();
        for (const auto& note : doc.notes) {
            if (!note.selected) continue;
            int curIdx = -1;
            for (int i = 0; i < static_cast<int>(cols.size()); ++i) {
                if (cols[static_cast<size_t>(i)].identifier == note.channelIndex) { curIdx = i; break; }
            }
            if (curIdx < 0) continue;
            int newIdx = std::clamp(curIdx + dir, 0, static_cast<int>(cols.size()) - 1);
            if (newIdx == curIdx) continue;
            Editor::Commands::MoveNotesCommand::MovedNote mv;
            mv.before = note;
            mv.after = note;
            mv.after.channelIndex = cols[static_cast<size_t>(newIdx)].identifier;
            moves.push_back(mv);
        }
        if (!moves.empty())
            edDoc->undoStack()->push(new Editor::Commands::MoveNotesCommand(edDoc, std::move(moves)));
        e->accept();
        update();
        return;
    }
    case Qt::Key_L:
        edDoc->toggleLongShort();
        e->accept();
        update();
        return;
    case Qt::Key_S:
        if (!(e->modifiers() & Qt::ControlModifier)) {
            edDoc->convertToShort();
            e->accept();
            update();
            return;
        }
        break;
    case Qt::Key_H:
        edDoc->toggleHidden();
        e->accept();
        update();
        return;
    case Qt::Key_Delete:
    case Qt::Key_Backspace: {
        // Delete all selected notes
        const auto& doc = edDoc->document();
        for (const auto& note : doc.notes) {
            if (note.selected)
                edDoc->deleteNote(note.channelIndex, note.measureIndex, note.beat.num, note.beat.den);
        }
        e->accept();
        update();
        return;
    }
    default:
        // Number keys 0-8: move to column
        if (e->key() >= Qt::Key_0 && e->key() <= Qt::Key_8) {
            int colIdx = e->key() - Qt::Key_0;
            // Map: 0=scratch(first col), 1-8=A1-A8
            if (colIdx < static_cast<int>(cols.size())) {
                std::vector<Editor::Commands::MoveNotesCommand::MovedNote> moves;
                const auto& doc = edDoc->document();
                int targetCh = cols[static_cast<size_t>(colIdx)].identifier;
                for (const auto& note : doc.notes) {
                    if (!note.selected || note.channelIndex == targetCh) continue;
                    Editor::Commands::MoveNotesCommand::MovedNote mv;
                    mv.before = note;
                    mv.after = note;
                    mv.after.channelIndex = targetCh;
                    moves.push_back(mv);
                }
                if (!moves.empty())
                    edDoc->undoStack()->push(new Editor::Commands::MoveNotesCommand(edDoc, std::move(moves)));
                e->accept();
                update();
                return;
            }
        }
        break;
    }
    QQuickItem::keyPressEvent(e);
}
