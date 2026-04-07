## Why

The Qt/C++ rewrite (rewrite-ibmsc) and UI redesign (ableton-ui-redesign) have completed the core scaffolding, BMS I/O, audio engine, canvas rendering, and Ableton-inspired shell. However, the original VB iBMSC contains a significant set of advanced editing features and dialogs that have not yet been ported — leaving the new app functionally incomplete compared to the original. This change closes that gap and adds UI polish to make the app production-ready.

## What Changes

- Port **column configuration system**: per-column enable/disable, width, title, and BMS channel mapping (from `EditorColumns.vb`)
- Port **BPM manipulation tools**: ConstBPM conversion, BPM×2/÷2, BPM change by value, area-to-STOP conversion (from `MyO2.vb`, `TimeSelectionOps.vb`)
- Port **statistics dialog**: note count per channel, total note count (from `dgStatistics.vb`)
- Port **O2Mania/MyO2 adjustment dialog**: keysound index remapping with apply modes (from `dgMyO2.vb`)
- Port **SM import dialog**: StepMania `.sm` file import with BPM/note conversion (from `ChartIO.vb` `OpenSM`, `dgImportSM.vb`)
- Port **ibmsc native format**: `.ibmsc` XML save/load round-trip including column config, visual settings, and undo history snapshot (from `ChartIO.vb` `OpeniBMSC`/`SaveiBMSC`)
- Port **visual/theme settings**: per-column note colors, background colors, sub-beat line colors, font settings (from `VisualSettings.vb`, `EditorPersistent.vb`)
- Port **locale/language system**: XML-driven UI string substitution for multi-language support (from `EditorPersistent.vb` `LoadLocale`)
- Implement **color picker dialog** (from `ColorPicker.vb`) for note/background color customization
- Implement **note statistics overlay** on canvas: show note count per visible measure
- UI polish: keyboard shortcut completeness audit, status bar note-count display, column header click-to-toggle-enable

## Capabilities

### New Capabilities
- `column-config`: Per-column enable/disable, width, BMS channel assignment, and title — persisted in AppSettings and reflected live on canvas
- `bpm-tools`: Advanced BPM manipulation: ConstBPM normalization, halve/double, area-to-STOP, BPM change by ratio
- `ibmsc-format`: Native `.ibmsc` XML format save/load including column config, visual settings, and note data
- `sm-import`: StepMania `.sm` file import converting timing and note data to BMS format
- `note-statistics`: Per-channel and total note count dialog; status bar live count
- `o2-adjustment`: O2Mania keysound index remapping dialog with three apply modes
- `visual-theme-settings`: Per-column and global color/font customization with live preview, persisted to settings XML
- `locale-system`: XML locale file loading for UI string internationalization

### Modified Capabilities
<!-- none — all new -->

## Impact

- `src/app/AppSettings`: extend with column config, color scheme, locale path
- `src/app/AppController`: add slots for new menu actions (BPM tools, SM import, statistics, O2 adjust)
- `src/io/BmsParser` / `BmsSerializer`: no changes needed
- New `src/io/IbmscFormat.cpp/.h`: ibmsc XML read/write
- New `src/io/SmImporter.cpp/.h`: SM file parser
- `qml/canvas/ChartCanvasItem`: column enable/width driven by column config
- New QML dialogs: `panels/BpmToolsDialog.qml`, `panels/StatisticsDialog.qml`, `panels/O2AdjustDialog.qml`, `panels/SmImportDialog.qml`, `panels/ColorPickerDialog.qml`, `panels/VisualSettingsDialog.qml`
- `qml/components/ControlBar.qml`: add column header toggle buttons
- `qml/components/Theme.qml`: extend with per-column color arrays
