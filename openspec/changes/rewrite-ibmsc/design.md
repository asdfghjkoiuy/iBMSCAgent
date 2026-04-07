## Context

iBMSC is a BMS (Be-Music Source) chart editor. The original is a single-file VB.NET WinForms application (~8000 lines in MainWindow.vb) that is Windows-only, uses GDI+ for chart rendering, and has no separation of concerns. The goal is a clean-slate C++ rewrite that is cross-platform, GPU-accelerated, and maintainable.

## Goals / Non-Goals

**Goals:**
- Full BMS/BME/BML/PMS format compatibility (read and write)
- Qt Quick / QML UI with fluid 60fps animations and smooth scrolling
- Layered architecture: model → I/O → audio → UI
- Undo/redo for all editor mutations
- macOS, Windows, Linux support from one codebase
- Ship as a standalone app with no runtime dependencies beyond Qt 6

**Non-Goals:**
- bmson write support (read-only for now)
- Built-in BMS player/game engine (Pulsus remains the external preview tool)
- Cloud sync, online features, or plugin system
- Re-implementing every niche option from the original (can add incrementally)

## Decisions

### D1: Qt Quick / QML for all UI, C++ for all logic

Qt Widgets was the natural first choice (see the failed `iBMSCPP` prototype) but it cannot deliver smooth 60fps rendering for the chart canvas or fluid panel animations. Qt Quick's scene graph is retained-mode and GPU-accelerated, making it the right foundation for the chart editor canvas.

**Architecture**: C++ classes exposed to QML via `Q_OBJECT` / `QML_ELEMENT`. Business logic stays in C++; QML handles layout, animation, and visual styling only.

**Alternative considered**: Dear ImGui — excellent performance but requires building a full widget library from scratch. Qt Quick gives us theming, accessibility, text rendering, and platform integration for free.

### D2: Chart canvas as a custom QQuickItem subclass (not QML Canvas 2D)

The chart canvas must render thousands of note rectangles, beat lines, and column backgrounds at 60fps. `Canvas {}` in QML (HTML5 2D API) re-renders from scratch every frame on CPU. A `QQuickItem` subclass with a custom `QSGNode` tree lets us update only changed nodes, keeping work proportional to edits rather than total chart size.

**Alternative considered**: QML `Repeater` with `Rectangle` items — too many QML objects causes GC pressure and poor perf on large charts.

### D3: BMS model is a plain C++ value-type layer with no Qt dependency

`BmsDocument` and related types live in a `model/` namespace with no Qt headers. This keeps I/O and the model unit-testable without a Qt event loop. Qt types (`QString`, `QList`) are used only at the boundary between model and UI.

**Alternative considered**: Inherit from `QObject` for change notifications — rejected because it forces heap allocation, prevents value semantics, and couples the model to Qt.

### D4: Command pattern undo/redo stack (`QUndoStack`)

Qt provides `QUndoStack` / `QUndoCommand`. We use it as-is. Each user action (place, delete, move note; change BPM; edit WAV table entry; etc.) is a `QUndoCommand` subclass. The stack is owned by the document; QML binds the undo/redo enabled states and text.

### D5: Audio via Qt Multimedia (QAudioOutput + manual mixing) with fallback to miniaudio

Qt 6 Multimedia covers all three platforms and handles device selection. For low-latency simultaneous playback of many keysounds we push PCM buffers ourselves (decoded via libsndfile or Qt's own decoder) into a `QAudioSink`. If Qt Multimedia proves too high-latency, we embed [miniaudio](https://miniaud.io/) (single-header, no dependencies) as a fallback.

### D6: File layout

```
src/
  model/        # BmsDocument, Note, Measure, resource tables — no Qt
  io/           # BMS parser/serializer — depends on model + QString/QFile
  audio/        # AudioEngine — depends on model + Qt Multimedia
  editor/       # EditorState, UndoCommands — depends on model + Qt
  app/          # QML-registered C++ types, main.cpp
qml/
  Main.qml
  components/   # Reusable QML controls
  canvas/       # ChartCanvas.qml + ChartCanvasItem.cpp (QQuickItem)
  panels/       # ResourcePanel.qml, HeaderPanel.qml, etc.
```

### D7: QML theming via Qt Quick Controls 2 Material or a custom style

We implement a custom dark style (Material Dark as base). Colors and spacing are defined as QML singletons (`Theme.qml`) so the entire UI can be reskinned. The default target look is a dark editor aesthetic inspired by modern DAWs.

## Risks / Trade-offs

- **QQuickItem subclass complexity** → Mitigate by starting with a simpler `ShaderEffect`-free implementation and profiling before optimizing the scene graph node tree.
- **Qt Multimedia latency on Windows** → miniaudio fallback is already planned (D5).
- **BMS parser edge cases** (encoding detection, malformed files, non-standard channels) → Fuzzing with real BMS archives early; maintain a test corpus.
- **Large charts (1000+ measures, dense notes)** → Viewport culling in the canvas item; only submit visible note nodes to the scene graph.
- **macOS code-signing and notarization** → Bundle Qt frameworks inside `.app`; use `macdeployqt`. Address as a separate CI/CD task.

## Migration Plan

1. New project lives at `iBMSCQt2/` (or directly in `iBMSCAgent/` as the canonical project).
2. The VB.NET `iBMSC/` source is kept read-only for reference until feature parity is reached.
3. No data migration needed — BMS files are the interchange format.
4. Replace the Pulsus preview integration by passing the file path via CLI argument (same as today).

## Open Questions

- Exact key-binding scheme: replicate iBMSC defaults exactly or redesign?
- Whether to support multi-document (tabbed) editing in v1 or single-document only.
- Localization strategy: English + Japanese + Chinese (Simplified) at launch?
