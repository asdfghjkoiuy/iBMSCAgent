## ADDED Requirements

### Requirement: Keysound index remapping
The system SHALL provide a dialog to remap WAV slot indices across all notes in the document. The user SHALL specify source and target index ranges and an apply mode (replace all, swap, or shift).

#### Scenario: Replace all occurrences
- **WHEN** the user sets source index 5 → target index 10 with mode "Replace"
- **THEN** all notes referencing WAV slot 5 are updated to reference slot 10

#### Scenario: Swap two indices
- **WHEN** the user sets source 5 and target 10 with mode "Swap"
- **THEN** notes referencing slot 5 become slot 10 and vice versa

#### Scenario: Shift index range
- **WHEN** the user sets a shift offset of +3 with mode "Shift"
- **THEN** all note WAV indices are incremented by 3, clamped to valid range 1–1295

### Requirement: O2 adjustment is undoable
All keysound remapping operations SHALL be pushed onto the undo stack as a single command.

#### Scenario: Undo remapping
- **WHEN** the user undoes after an O2 adjustment
- **THEN** all note WAV indices are restored to their pre-adjustment values
