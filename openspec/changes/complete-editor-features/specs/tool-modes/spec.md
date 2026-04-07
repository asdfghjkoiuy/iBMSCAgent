## ADDED Requirements

### Requirement: Independent Tool Handling
The editor interaction state machine MUST split generic interaction into separate explicit classes (Write, Erase, Select).

#### Scenario: Write Mode Behavior
- **WHEN** the user is in "Write" mode and left-clicks empty channel space
- **THEN** a note is instantly inserted at the snapped cursor resolution.

#### Scenario: Select Mode Behavior
- **WHEN** the user is in "Select" mode and left-clicks empty channel space and drags
- **THEN** a rubber band selection box forms, gathering elements without mutating or placing any accidental new notes.

#### Scenario: Erase Mode Behavior
- **WHEN** the target interacts with a note block while "Eraser" mode is active
- **THEN** it instantly triggers a `DeleteNoteCommand` covering exactly that hitblock.