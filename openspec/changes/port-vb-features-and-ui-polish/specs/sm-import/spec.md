## ADDED Requirements

### Requirement: SM file parsing
The system SHALL parse StepMania `.sm` files and extract BPM events, STOP events, and note data for a user-selected difficulty. The parser SHALL handle multiple `#NOTES` blocks and let the user choose which to import.

#### Scenario: Open SM file
- **WHEN** the user opens a `.sm` file
- **THEN** the import dialog shows available difficulties for selection

#### Scenario: Select difficulty
- **WHEN** the user selects a difficulty and confirms
- **THEN** the notes from that difficulty are converted and loaded as a new BmsDocument

### Requirement: SM-to-BMS lane mapping
The system SHALL map StepMania 4-key and 8-key lane layouts to BMS channels using a configurable mapping shown in the import dialog.

#### Scenario: Default lane mapping applied
- **WHEN** the user imports with default mapping
- **THEN** SM lanes are assigned to BMS channels 11–18 in order

### Requirement: Unsupported SM feature warning
The system SHALL display a warning when the imported SM file contains features that cannot be represented in BMS (e.g. warps, negative BPMs, rolls).

#### Scenario: Warp detected
- **WHEN** the SM file contains a WARP segment
- **THEN** the import dialog shows a warning that warps are not supported and will be skipped
