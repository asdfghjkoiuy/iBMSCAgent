## ADDED Requirements

### Requirement: Right-click to delete note
The system SHALL delete a note when the user right-clicks on it, regardless of current tool mode.

#### Scenario: Right-click on note
- **WHEN** user right-clicks on a note
- **THEN** the note is deleted, undoable

### Requirement: Double-click to edit note value
The system SHALL open an input dialog to edit a note's value (WAV slot) when the user double-clicks on it.

#### Scenario: Double-click note
- **WHEN** user double-clicks on a note
- **THEN** an input dialog appears with the current value; on confirm, the value is updated, undoable

### Requirement: Arrow key note movement
The system SHALL move selected notes by grid units using arrow keys: Up/Down move in beat axis, Left/Right move across columns. Ctrl+Arrow moves by 1 minimal unit.

#### Scenario: Move notes up
- **WHEN** user presses Up arrow with notes selected
- **THEN** selected notes move up by one grid subdivision, undoable

#### Scenario: Move notes left
- **WHEN** user presses Left arrow with notes selected
- **THEN** selected notes move one column to the left, undoable

### Requirement: Number key column assignment
The system SHALL move selected notes to a specific column when the user presses number keys 1-8 (matching A1-A8) or 0 (scratch).

#### Scenario: Press 3 with notes selected
- **WHEN** user presses 3 with notes selected
- **THEN** all selected notes are moved to column A3, undoable

### Requirement: Drag to resize long notes
The system SHALL allow resizing long notes by dragging their top or bottom edge.

#### Scenario: Drag bottom edge of long note
- **WHEN** user drags the bottom edge of a long note downward
- **THEN** the note's duration increases, snapped to grid, undoable

### Requirement: L/S/H keyboard toggles
The system SHALL toggle note properties with keyboard: L = toggle long/short, S = convert to short, H = toggle hidden/visible.

#### Scenario: Press L with notes selected
- **WHEN** user presses L with notes selected
- **THEN** selected notes toggle between long and short, undoable
