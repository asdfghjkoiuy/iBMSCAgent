# Ableton Control Bar Spec

## Overview
A single unified top control bar (40px height) replacing both the traditional MenuBar and ToolBar.

## Requirements

### Layout (left to right)
1. **App zone** (left): "iBMSC" text logo (small), file action icon buttons (New, Open, Save)
2. **Transport zone** (center-left): Play/Stop button, BPM display (read from editorDoc)
3. **Grid zone** (center): Grid subdivision ComboBox, Zoom +/- buttons
4. **Utility zone** (right): Undo/Redo buttons, sidebar toggle button, overflow menu button (hamburger)

### Overflow Menu
- Popup/Drawer triggered by hamburger button
- Contains all actions previously in MenuBar: File operations, Edit, View, Chart actions, Settings
- Organized in sections with separators
- Supports keyboard shortcuts display

### Behavior
- All existing keyboard shortcuts remain functional (bound at ApplicationWindow level via Shortcut items)
- Transport button toggles play/stop via `audioEngine` / `appController`
- Undo/Redo enabled state bound to `editorDoc.canUndo` / `editorDoc.canRedo`

### Visual Style
- Flat background matching `bgSurface`
- No elevation/shadow
- Icon buttons: 28x28, no border, hover highlight only
- Bottom 1px border separator
