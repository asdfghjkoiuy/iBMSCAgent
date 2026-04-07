## ADDED Requirements

### Requirement: Per-column enable/disable
The system SHALL allow the user to enable or disable each editor column independently. Disabled columns SHALL be hidden from the canvas and excluded from BMS channel output.

#### Scenario: Disable a column
- **WHEN** the user toggles a column off in column config
- **THEN** the column disappears from the canvas and its notes are not rendered

#### Scenario: Re-enable a column
- **WHEN** the user toggles a disabled column back on
- **THEN** the column reappears with its notes intact

### Requirement: Per-column width adjustment
The system SHALL allow the user to set a custom pixel width for each column. Width changes SHALL be reflected immediately on the canvas.

#### Scenario: Change column width
- **WHEN** the user sets a column width value in settings
- **THEN** the canvas redraws with the new column width

### Requirement: Per-column BMS channel assignment
The system SHALL allow the user to assign a BMS channel string (e.g. "11", "12", "1A") to each column. The serializer SHALL use this assignment when writing BMS output.

#### Scenario: Custom channel assignment saved
- **WHEN** the user assigns channel "1A" to column 3 and saves the file
- **THEN** the BMS output uses channel 1A for notes in column 3

### Requirement: Column config persistence
Column configuration (enabled state, width, channel assignment, title) SHALL be persisted in AppSettings and restored on next launch.

#### Scenario: Config survives restart
- **WHEN** the user configures columns and restarts the app
- **THEN** the same column config is restored
