## Context

The rewrite-ibmsc and ableton-ui-redesign changes delivered a working Qt6/QML BMS editor with core I/O, audio, canvas rendering, undo/redo, and an Ableton-inspired UI shell. The original VB iBMSC (~17,800 lines across 25 files) contains additional features that power users depend on: column configuration, BPM manipulation tools, the native `.ibmsc` format, SM import, statistics, O2 adjustment, and a full visual theming system. These are not yet present in the C++ rewrite.

The codebase is now stable enough to add these features without architectural changes — all new work fits into the existing `src/`, `qml/panels/`, and `qml/components/` structure.

## Goals / Non-Goals

**Goals:**
- Port all remaining VB features that affect editing workflow (column config, BPM tools, ibmsc format, SM import, statistics, O2 adjust)
- Implement visual/color theming with per-column customization
- Add locale/language XML loading
- Complete keyboard shortcut coverage and status bar polish
- Keep all new dialogs consistent with the existing Ableton-inspired flat dark theme

**Non-Goals:**
- Porting the VB WinForms layout or any Windows-specific UI patterns
- Supporting the legacy `.ibmsc` undo history snapshot (note data and settings only)
- Full WCAG accessibility audit
- Automated UI tests for new dialogs

## Decisions

### Column Config: C++ model + QML binding
The VB `Column` struct and `EditorColumns.vb` manage per-column state (enabled, width, title, BMS channel). In the rewrite, a new `ColumnConfig` struct is added to `BmsDocument` (or held in `AppSettings` for persistence). `ChartCanvasItem` already reads column count from context — it will be extended to read per-column enabled/width from a `QVariantList` property. This avoids a new C++ class and keeps the canvas driven by QML-side bindings.

Alternative considered: a dedicated `ColumnConfigModel : QAbstractListModel`. Rejected — overkill for a fixed max of ~20 columns; a simple `QVariantList` in `AppSettings` is sufficient.

### BPM Tools: Commands on EditorDocument
All BPM manipulation operations (ConstBPM, ×2/÷2, area-to-STOP) are implemented as `QUndoCommand` subclasses pushed onto the existing `QUndoStack`. This gives undo/redo for free and matches the existing command pattern. The dialog (`BpmToolsDialog.qml`) calls `AppController` slots which construct and push the commands.

### ibmsc Format: Separate IO class
`IbmscFormat` is a standalone `src/io/IbmscFormat.cpp/.h` that reads/writes the XML schema used by the original VB app. It does not touch `BmsParser`/`BmsSerializer`. The format stores: header fields, WAV/BMP tables, note list, column config, and visual settings. Undo history is not persisted (the VB app's snapshot approach was fragile).

### SM Import: Stateless parser function
`SmImporter::import(QFile&) → BmsDocument` is a pure function with no side effects. The dialog (`SmImportDialog.qml`) lets the user pick BPM rounding and lane mapping before calling the importer. This mirrors the VB `OpenSM` approach but without the in-place mutation of the global note array.

### Visual Theme Settings: Extended AppSettings + Theme.qml
Per-column colors and global background/line colors are stored as arrays in `AppSettings` and exposed to QML via existing context property. `Theme.qml` gains a `columnColors` array property read from `AppSettings`. `ChartCanvasItem` reads column colors from a C++ property already updated by `AppSettings`. No new singleton needed.

### Locale System: QTranslator + XML fallback
The VB locale system uses custom XML files. In Qt6, `QTranslator` with `.qm` files is the idiomatic approach, but to preserve compatibility with existing VB locale XML files, a lightweight `LocaleLoader` class reads the XML and calls `QQmlEngine::retranslate()` after populating a `QMap<QString,QString>`. QML strings use `qsTr()` throughout. This allows existing locale files to work without conversion.

## Risks / Trade-offs

- **SM import fidelity**: StepMania timing model (BPM changes, stops, warps) maps imperfectly to BMS. Complex charts may have timing drift. → Mitigation: warn user in dialog when unsupported SM features are detected; import best-effort.
- **ibmsc XML schema drift**: The VB app's XML schema is undocumented and may have version variations. → Mitigation: use lenient attribute reading (ignore unknown elements), test against real `.ibmsc` files from the iBMSC repo.
- **Column config canvas performance**: Reading per-column width from QML on every paint node update could cause extra property lookups. → Mitigation: cache column layout in `ChartCanvasItem::updatePaintNode` as a local `QVector<int>` computed once per frame.
- **Locale XML loading at runtime**: Changing locale requires re-binding all QML string properties. `QQmlEngine::retranslate()` triggers this but may cause a visible flash. → Mitigation: only allow locale change from settings dialog with an app restart prompt.

## Migration Plan

No data migration needed — all new features are additive. Existing `.bms` files and `AppSettings` persist unchanged. The `.ibmsc` format is a new file type; no existing user data is affected.

New files added:
- `src/io/IbmscFormat.cpp/.h`
- `src/io/SmImporter.cpp/.h`
- `src/app/LocaleLoader.cpp/.h`
- `src/editor/commands/BpmToolsCommands.cpp/.h`
- Six new QML dialog files under `qml/panels/`

`CMakeLists.txt` updated to include new source files.
