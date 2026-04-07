## ADDED Requirements

### Requirement: Undo/redo stack for all editor mutations
The system SHALL maintain a `QUndoStack` per open document. Every user-initiated mutation SHALL be encapsulated as a `QUndoCommand` and pushed onto the stack. Undo SHALL reverse the mutation; redo SHALL reapply it.

#### Scenario: Undo note placement
- **WHEN** the user places a note and then presses Ctrl+Z
- **THEN** the note SHALL be removed and the document SHALL be in its pre-placement state

#### Scenario: Redo note placement
- **WHEN** the user undoes a note placement and then presses Ctrl+Y
- **THEN** the note SHALL be re-added

### Requirement: Covered mutation types
The undo/redo system SHALL cover: place single note, delete single note, place multiple notes (batch), delete multiple notes (batch), move notes, change BPM value, change STOP value, change measure length, edit WAV/BMP slot filename, edit header field value.

#### Scenario: Batch delete undo
- **WHEN** the user selects 10 notes, deletes them, then undoes
- **THEN** all 10 notes SHALL reappear in their original positions

#### Scenario: Header edit undo
- **WHEN** the user changes the #TITLE field and undoes
- **THEN** the title SHALL revert to its previous value

### Requirement: Dirty state tracking
The document SHALL track whether it has unsaved changes. The window title SHALL show a modified indicator (e.g. "•") when the document is dirty. Saving SHALL clear the dirty flag.

#### Scenario: Modified indicator shown
- **WHEN** the user places a note in a freshly opened file
- **THEN** the window title SHALL show a modified indicator

#### Scenario: Indicator cleared after save
- **WHEN** the user saves the document
- **THEN** the modified indicator SHALL disappear from the title

### Requirement: Close confirmation
The system SHALL prompt the user to save or discard changes when closing a dirty document.

#### Scenario: Close dirty document
- **WHEN** the user tries to close or open a new file with unsaved changes
- **THEN** a dialog SHALL ask "Save changes?", offering Save, Discard, and Cancel options
