## 1. Core Model Updates

- [x] 1.1 Support Long Notes in model by adding `durationInBeats` to `Model::Note` structure, default to 0.0 (short note).
- [x] 1.2 Update JSON parser and writer in `BMSParser` or IO layer to read/write `durationInBeats` (or LN channels in BMS format).
- [x] 1.3 Add custom Undo Command `PlaceNoteCommand` for placing an individual short or long note.
- [x] 1.4 Add custom Undo Command `RemoveNoteCommand` for erasing notes.
- [x] 1.5 Add custom Undo Command `MoveNoteCommand` to translate note time/channel.

## 2. Editor UI State & Tool Modes

- [x] 2.1 Introduce `EditMode` enum in QML / C++ context (Select, Write, Eraser).
- [x] 2.2 Re-implement ControlBar/Sidebar UI buttons to let user switch between Select, Write, and Eraser modes.
- [x] 2.3 Pass current `EditMode` to `ChartCanvasItem` so mouse events branch accordingly.
- [x] 2.4 Update cursor visuals on mouse hover depending on the active `EditMode`.

## 3. Note Editing & Canvas Interaction

- [x] 3.1 Implement Write Mode logic in `ChartCanvasItem::mousePressEvent` to place notes.
- [x] 3.2 Implement Eraser Mode logic in `ChartCanvasItem` to remove notes on click or drag.
- [x] 3.3 Implement Select Mode logic in `ChartCanvasItem` (click to select, drag to rubber-band multi-select).
- [x] 3.4 Support Long Note creation (click and drag vertically in Write Mode).
- [x] 3.5 Update QSG pipeline in `ChartCanvasItem` to render Long Note bodies spanning from start beat to `start + durationInBeats`.

## 4. Clipboard Operations

- [x] 4.1 Implement `copy` functionality to serialize selected notes into O2Mania/iBMSC format or custom JSON structure and put to `QClipboard`.
- [x] 4.2 Implement `cut` functionality (copy + `QUndoCommand` to remove selected notes).
- [x] 4.3 Implement `paste` functionality to parse clipboard payload and insert notes starting at current cursor/playback position.
- [x] 4.4 Expose Copy/Cut/Paste to Qt Shortcuts (Ctrl+C, Ctrl+X, Ctrl+V) and Edit menu.

## 5. Time Selection Operations

- [x] 5.1 Implement `TimeSelectionCommand` (Undoable) to insert blank measures at a specific beat, shifting all subsequent notes.
- [x] 5.2 Implement `DeleteMeasureCommand` to remove a beat range and shift subsequent notes back.
- [x] 5.3 Implement `CopyMeasureCommand` and `PasteMeasureCommand` to replicate entire beat bounds.
- [x] 5.4 Update canvas to highlight a selected time range (horizontal banding), allowing the user to pick bounds for time ops.
- [x] 5.5 Add context menu or top bar actions for "Insert Measure", "Delete Measure", etc.

## 6. Audio Waveform Display

- [x] 6.1 Create or integrate an audio decoding utility (e.g., using `miniaudio` or Qt Multimedia) to decode WAV/OGG into raw PCM downsampled data.
- [x] 6.2 Render waveform peaks onto a dedicated QSG geometry node in the canvas background, mapping audio time to beats via BPM.
- [x] 6.3 Add UI toggle to show/hide audio waveforms on the canvas.

## 7. Dynamic BGM Lanes

- [x] 7.1 Extend the grid rendering logic to draw dynamic columns on the right side of standard input channels.
- [x] 7.2 Implement logic to allocate new BGM lanes on demand if the user places an Audio note in the rightmost lane.
- [x] 7.3 Hide empty excess BGM lanes on project load or auto-cleanup.
- [x] 7.4 Support drag-and-drop of WAV/OGG files from OS Explorer onto BGM lanes to rapidly populate keysounds.