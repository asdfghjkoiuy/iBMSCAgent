## ADDED Requirements

### Requirement: Time Selection tool mode (F1)
The system SHALL provide a Time Selection tool that draws a horizontal beat range on the canvas, independent of columns.

#### Scenario: Activate time selection
- **WHEN** user presses F1 or selects Time Selection tool
- **THEN** cursor changes to time-selection mode; click-drag creates a horizontal range highlight

### Requirement: Expand by ratio
The system SHALL scale note positions within the time selection by a user-specified ratio.

#### Scenario: Double tempo of selection
- **WHEN** user has a time selection and invokes "Expand by Ratio" with ratio 2.0
- **THEN** all notes in the range have their positions scaled by 2x relative to selection start, undoable

### Requirement: Relocate by value (BPM-based)
The system SHALL reposition notes in the time selection based on a BPM conversion calculation.

#### Scenario: Relocate notes
- **WHEN** user specifies source and target BPM and invokes "Relocate by Value"
- **THEN** notes are repositioned as if the BPM changed from source to target, undoable

### Requirement: Reverse selection
The system SHALL reverse the order of notes within the time selection range.

#### Scenario: Reverse notes in range
- **WHEN** user has a time selection with notes and invokes "Reverse"
- **THEN** note positions are mirrored within the selection range, undoable

### Requirement: Transform into STOP
The system SHALL convert a time selection range into a STOP event of equivalent duration, removing notes in the range.

#### Scenario: Create stop from selection
- **WHEN** user has a time selection and invokes "Transform into STOP"
- **THEN** a STOP note is inserted at the selection start with duration matching the range, notes in range are removed, undoable
