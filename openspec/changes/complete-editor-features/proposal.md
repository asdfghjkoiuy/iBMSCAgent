## Why

The basic C++/QML architecture of the rewrite (rendering engine, generic undo/redo stack, parsers) is complete and functional. However, it lacks the specialized domain logic required of a fully featured BMS level editor (the equivalent to what the original VB.NET version had). Currently, only single point notes can be placed and dragged. Features like long notes (which are continuous strips over a specific duration), clipboard operations for patterns across time, proper tool mode toggles (Draw/Select/Erase), empty measure manipulation, dynamic multi-BGM channel management, and statistics tracking remain missing. We need to close this gap so that this project reaches feature parity with the classic iBMSC editor.

## What Changes

- Add Long Note (LN) support, allowing users to draw and interact with sustained notes.
- Establish a proper copy, cut, and paste pipeline using a custom clipboard format integrated with the Undo framework.
- Implement time-selection utilities (insert empty measure, cut measure, scale section lengths).
- Enhance the canvas drawing to show audio waveforms underneath the chart.
- Migrate to discrete Tool Operations (Write Tool, Select Tool, Eraser Tool) with dedicated shortcuts.
- Add dynamic row allocation for background (BGM) sounds so creators aren't bottlenecked by hardcoded lanes.
- Integrate the missing statistics dialog and bottom status-bar coordinate indicators.

## Capabilities

### New Capabilities
- `long-notes`: Defines the logic to represent, render, and manipulate start and end nodes of sustained LN objects in the timing map.
- `clipboard-operations`: Formalizes the serialization, caching, and deselection logic for the Cut/Copy/Paste operations within the Undo/Redo framework.
- `time-selection-ops`: Outlines commands that act on grid timeframes (inserting measure blank space, reducing or proportionally scaling selected measures).
- `tool-modes`: Specifies the state machine for editor modes (Selection Cursor, Pen, Eraser) replacing the combined click-and-drag logic.
- `dynamic-bgm-lanes`: Specifies how the visual layout will instantiate or collapse BGM channels to support complex sound layers.
- `audio-waveform`: Outlines parameters around mapping a visual histogram graph matching the synchronized audio data to the grid.

### Modified Capabilities
- `note-editing`: The existing note modification command will change to support multi-select manipulation of Long Notes simultaneously with singular point notes.

## Impact

This will touch nearly all components:
- `ChartCanvasItem.cpp`: Overhaul of the node drawing pipeline algorithms (`QSG` graph generation) to handle continuous bodies (LNs) and waveforms.
- `EditorDocument`/`MoveNotesCommand`/`PlaceNoteCommand`: Must be augmented to accommodate pairs of fractional beat data referencing contiguous properties rather than isolated points.
- Core Data `Note` struct (`model/BmsDocument.h`): Might require adjustments to mark and identify the duration/pairing.
- `ControlBar.qml`: Refactoring for discrete Tool buttons.
- Substantial UI expansion for the grid timeline operations and the statistics viewing model.