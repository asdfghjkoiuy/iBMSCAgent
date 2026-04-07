## ADDED Requirements

### Requirement: Column and note rendering
The system SHALL render the chart as a vertical scrollable canvas with one column per active BMS channel. Each note SHALL be drawn as a colored rectangle whose height corresponds to its beat duration (for long notes) or a fixed minimum height (for normal notes). Column backgrounds SHALL be visually distinct between player-side note columns, BGA columns, sound columns, and control columns (BPM/STOP).

#### Scenario: Normal note rendered
- **WHEN** a note exists at measure 5 beat 0.25 in channel 11
- **THEN** a colored rectangle SHALL appear at the correct vertical position in column 11

#### Scenario: Long note rendered
- **WHEN** a long-note start and end note exist in the same channel
- **THEN** a tall rectangle spanning from start to end beat position SHALL be rendered

#### Scenario: BPM change marker
- **WHEN** a BPM change event exists
- **THEN** a horizontal marker line with the new BPM value SHALL be drawn across all columns

### Requirement: Smooth GPU-accelerated scrolling and zoom
The canvas SHALL scroll vertically at 60fps using Qt Quick's scene graph. Vertical zoom (beats-per-pixel) SHALL be adjustable via mouse wheel or keyboard shortcut. Horizontal zoom per column SHALL be configurable.

#### Scenario: Mouse wheel scroll
- **WHEN** the user scrolls the mouse wheel over the canvas
- **THEN** the viewport SHALL smoothly scroll up or down proportional to the scroll delta

#### Scenario: Zoom in
- **WHEN** the user presses Ctrl+scroll-up
- **THEN** the vertical resolution SHALL increase (more pixels per beat) keeping the beat under the cursor stationary

### Requirement: Note placement
The user SHALL be able to place a note by clicking at a beat position in a column. The beat position SHALL snap to the current grid subdivision (e.g. 1/4, 1/8, 1/16, 1/32, 1/48, 1/192, or custom).

#### Scenario: Click to place note
- **WHEN** the user left-clicks on an empty beat cell in a column
- **THEN** a note SHALL be created at the snapped beat position in that column

#### Scenario: Click to delete note
- **WHEN** the user left-clicks on an existing note
- **THEN** the note SHALL be deleted

#### Scenario: Grid snap
- **WHEN** the current grid is set to 1/8
- **THEN** placing a note SHALL snap to the nearest 1/8-beat boundary

### Requirement: Note selection and multi-edit
The user SHALL be able to select notes via rubber-band drag selection or Shift+click. Selected notes SHALL be movable by dragging and deletable via the Delete key.

#### Scenario: Rubber-band select
- **WHEN** the user drags a selection rectangle over a region of the canvas
- **THEN** all notes within the rectangle SHALL become selected (highlighted)

#### Scenario: Move selected notes
- **WHEN** selected notes are dragged vertically
- **THEN** their beat positions SHALL change by the drag delta, snapping to the grid

#### Scenario: Delete selected notes
- **WHEN** the Delete key is pressed with notes selected
- **THEN** all selected notes SHALL be removed and the action SHALL be undoable

### Requirement: Playback cursor display
The canvas SHALL show a horizontal playback cursor line at the current playback position during audio preview. The canvas SHALL auto-scroll to keep the cursor visible during playback.

#### Scenario: Cursor moves during playback
- **WHEN** audio playback is active
- **THEN** the cursor line SHALL advance upward in sync with the audio position

#### Scenario: Auto-scroll during playback
- **WHEN** the cursor reaches within 20% of the bottom of the viewport
- **THEN** the canvas SHALL scroll to keep the cursor centered

### Requirement: Viewport culling
The chart canvas SHALL only submit scene graph nodes for notes and beat lines that are within or near the current viewport, regardless of total chart size.

#### Scenario: Performance with large chart
- **WHEN** a chart with 2000 measures and 50,000 notes is loaded
- **THEN** scrolling SHALL remain at 60fps with no perceptible lag
