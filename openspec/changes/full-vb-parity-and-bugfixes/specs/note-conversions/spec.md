## ADDED Requirements

### Requirement: Convert selected notes to Long Note
The system SHALL convert all selected short notes to long notes with a default length of 1 grid unit.

#### Scenario: Convert short to long
- **WHEN** user selects notes and invokes "Convert to Long Note"
- **THEN** all selected short notes become long notes, undoable

### Requirement: Convert selected notes to Short Note
The system SHALL convert all selected long notes to short notes by removing their duration.

#### Scenario: Convert long to short
- **WHEN** user selects long notes and invokes "Convert to Short Note"
- **THEN** all selected long notes become short notes (duration = 0), undoable

### Requirement: Toggle Long/Short
The system SHALL toggle each selected note between long and short.

#### Scenario: Toggle mixed selection
- **WHEN** user selects a mix of long and short notes and invokes "Toggle Long/Short"
- **THEN** short notes become long and long notes become short, undoable

### Requirement: Convert to Hidden Note
The system SHALL mark selected notes as hidden (channel shift to hidden range 31-39/41-49).

#### Scenario: Hide notes
- **WHEN** user selects visible notes and invokes "Convert to Hidden"
- **THEN** notes are moved to hidden channels, undoable

### Requirement: Convert to Visible Note
The system SHALL mark selected hidden notes as visible (channel shift back to normal range).

#### Scenario: Unhide notes
- **WHEN** user selects hidden notes and invokes "Convert to Visible"
- **THEN** notes are moved back to visible channels, undoable

### Requirement: Mirror notes
The system SHALL mirror selected notes across the column axis (swap P1 columns symmetrically).

#### Scenario: Mirror P1 notes
- **WHEN** user selects A-group notes and invokes "Mirror"
- **THEN** column positions are mirrored (A1↔A7, A2↔A6, A3↔A5, A4 stays), undoable

### Requirement: Storm (shuffle) notes
The system SHALL randomly shuffle the column assignments of selected notes while preserving their beat positions.

#### Scenario: Storm selected notes
- **WHEN** user selects notes and invokes "Storm"
- **THEN** note column assignments are randomized, beat positions unchanged, undoable

### Requirement: Modify Labels
The system SHALL allow batch modification of note values (WAV slot labels) for selected notes.

#### Scenario: Batch relabel
- **WHEN** user selects notes and invokes "Modify Labels" with a target value
- **THEN** all selected notes have their value changed to the target, undoable
