## Why

The Qt/C++ rewrite of iBMSC is functional but has critical bugs (save dialog positioning, trackpad zoom) and is missing many features from the original VB.NET version. Users need a 1:1 feature-complete port with all dialogs, interactions, and workflows preserved — styled in a flat Ableton Live / terminal-aesthetic design language.

## What Changes

- Fix save-confirmation dialog centering (currently appears at top-left instead of window center)
- Fix macOS trackpad pinch-to-zoom in chart editor (currently broken zoom logic in wheelEvent)
- Add splash screen on startup matching VB.NET behavior
- Add Statistics dialog (note counts by type)
- Add MyO2 ToolBox (constant BPM, grid check, grid adjust)
- Add full Find/Delete/Replace with column filters, note/measure/value ranges
- Add note conversion tools: Long↔Short, Hidden↔Visible, Mirror, Storm, Modify Labels
- Add time selection tool (F1) with expand-by-ratio, relocate-by-value, reverse, transform-to-stop
- Add LN input mode toggle (NT vs BMSE style)
- Add error checking overlay on notes
- Add preview-on-click (play keysound when placing/clicking notes)
- Add show-filename-on-notes toggle
- Add WAV auto-increase when writing
- Add Options Panel sections: Expansion Code editor, full Beat editor with insert/remove measure
- Add theme system (save/load visual themes)
- Add complete keyboard shortcut set matching VB.NET (arrow key note movement, number key column assignment, L/S/H toggles)
- Add right-click context menu on notes
- Add double-click to edit note value
- Add multi-file loading progress dialog
- Add drag-to-resize long notes (upper/lower edge)
- Add clipboard interop (iBMSC format + BMSE format)

## Capabilities

### New Capabilities
- `fix-dialog-and-trackpad`: Fix save dialog centering and macOS trackpad zoom
- `splash-screen`: Startup splash screen with branding
- `statistics-dialog`: Note count statistics by type
- `note-conversions`: Long↔Short, Hidden↔Visible, Mirror, Storm, Modify Labels
- `time-selection-tools`: Expand-by-ratio, relocate-by-value, reverse, transform-to-stop
- `advanced-find-replace`: Full find/delete/replace with column filters and ranges
- `editor-modes`: LN input mode toggle, error check, preview-on-click, show-filename, WAV auto-increase
- `note-interactions`: Right-click delete, double-click edit, arrow key movement, number key column assign, drag-resize LN
- `theme-system`: Save/load visual themes
- `options-panel-complete`: Expansion code editor, full beat editor with insert/remove measure

### Modified Capabilities
- (none — all changes are additive to the existing Qt/C++ codebase)

## Impact

- `qml/Main.qml` — dialog positioning fix, splash screen integration
- `qml/canvas/ChartCanvasItem.cpp` — trackpad zoom fix, note interactions, time selection, error overlay
- `qml/panels/` — new dialogs (Statistics, MyO2, Find/Replace, etc.)
- `qml/components/` — theme system, control bar updates
- `src/editor/` — new commands for conversions, time selection operations
- `src/model/Note.h` — Hidden, Landmine, LNPair fields
- `src/app/AppController` — new slots for all tools
- `src/app/AppSettings` — theme persistence, editor mode flags
