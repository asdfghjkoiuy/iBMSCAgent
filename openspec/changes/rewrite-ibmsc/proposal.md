## Why

iBMSC is a BMS chart editor written in VB.NET (Windows Forms), which is Windows-only, visually dated, and architecturally monolithic. Rewriting it in C++ with Qt Quick/QML enables a cross-platform, GPU-accelerated UI with fluid animations and a modern design language, while also dramatically improving performance for large charts.

## What Changes

- **BREAKING**: Replace the entire VB.NET codebase with a C++17 implementation
- New Qt Quick/QML UI with fluid animations, smooth scrolling, and modern visual design
- GPU-accelerated chart canvas using Qt Quick's scene graph renderer
- Cross-platform support (macOS, Windows, Linux) from the same codebase
- Clean layered architecture separating model, I/O, audio, and UI concerns
- Retain full BMS format compatibility (bms/bme/bml/pms, bmson) and all editor features from the original

## Capabilities

### New Capabilities

- `bms-core`: BMS data model including notes, measures, BPM/STOP events, WAV/BMP resource tables, and channel definitions. Also covers base-36 encoding utilities and BMS-format-specific logic.
- `bms-file-io`: BMS/BME/BML/PMS file parser and serializer. Handles encoding detection, header fields, channel data decoding, and round-trip save fidelity.
- `chart-editor-canvas`: Qt Quick canvas widget for the main editing area — renders columns, notes (regular, long-note, mine, hidden), beat lines, measure markers, and BPM change markers. Supports smooth GPU-accelerated scrolling, zoom, note placement, selection, and drag operations.
- `audio-engine`: BMS audio preview engine. Loads WAV/OGG keysounds, schedules playback by chart position, and supports play/stop/seek. Integrates with Qt Multimedia or miniaudio.
- `resource-panel`: WAV/BMP resource table editor panel. Lists all 36×36 resource slots, supports assign/clear/preview operations.
- `undo-redo`: Command pattern undo/redo stack. All editor mutations (place note, delete note, move note, change BPM, edit header, edit resource) are encapsulated as reversible commands.
- `settings-and-prefs`: Application settings (theme, key bindings, editor preferences, recent files) persisted via QSettings.
- `qml-app-shell`: Top-level Qt Quick application shell — main window layout, menu bar, toolbar, status bar, side panels, keyboard shortcuts, and drag-and-drop file open.

### Modified Capabilities

## Impact

- Replaces the entire `iBMSC/` VB.NET project
- No runtime dependency on .NET/Mono; requires Qt 6.5+ and a C++17 compiler
- Pulsus (C# BMS player) would be rewrited using QML after rewrite of ibmsc 
