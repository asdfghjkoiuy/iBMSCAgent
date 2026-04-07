## ADDED Requirements

### Requirement: LN input mode toggle (NT vs BMSE)
The system SHALL support two long-note input modes toggled via F8: NT mode (explicit length) and BMSE mode (paired start/end notes).

#### Scenario: Toggle LN mode
- **WHEN** user presses F8
- **THEN** the LN input mode toggles between NT and BMSE, reflected in toolbar state

### Requirement: Error check overlay
The system SHALL highlight notes with errors (duplicate positions, invalid values) with a distinct visual indicator when error checking is enabled.

#### Scenario: Enable error check
- **WHEN** user toggles error check on
- **THEN** notes with errors are rendered with a red error indicator overlay

### Requirement: Preview on click
The system SHALL play the keysound associated with a note when the user clicks or places it, when preview-on-click is enabled.

#### Scenario: Click note with preview enabled
- **WHEN** preview-on-click is on and user clicks a note with WAV slot 05
- **THEN** the audio engine plays WAV slot 05

### Requirement: Show filename on notes
The system SHALL display the WAV filename text on note rectangles when show-filename is enabled.

#### Scenario: Enable show filename
- **WHEN** user toggles show-filename on
- **THEN** each note rectangle displays its WAV slot filename as small text

### Requirement: WAV auto-increase when writing
The system SHALL automatically increment the WAV slot value for each consecutively placed note when WAV auto-increase is enabled.

#### Scenario: Place consecutive notes with auto-increase
- **WHEN** WAV auto-increase is on and user places a note with value 05
- **THEN** the next note placed automatically uses value 06
