## ADDED Requirements

### Requirement: WAV resource table display
The system SHALL display the WAV resource table as a scrollable list showing all 1295 slots (01–ZZ). Each row SHALL show the slot index, filename (or empty indicator), and a preview button.

#### Scenario: List populated slots
- **WHEN** a BMS file with 20 defined WAV slots is loaded
- **THEN** those 20 rows SHALL show their filenames; the remaining rows SHALL show as empty

#### Scenario: Empty slot indicator
- **WHEN** a slot has no filename assigned
- **THEN** the row SHALL be visually dimmed or show a placeholder like "—"

### Requirement: WAV slot editing
The user SHALL be able to assign a filename to a WAV slot by double-clicking the row and either typing a filename or using a file-picker dialog. Clearing the filename SHALL remove the assignment.

#### Scenario: Assign via file picker
- **WHEN** the user double-clicks an empty WAV slot
- **THEN** a file-picker dialog SHALL open filtered to audio file types

#### Scenario: Clear assignment
- **WHEN** the user clears the filename field for a slot
- **THEN** the slot SHALL become empty and any notes referencing it SHALL retain the slot index but play silence

### Requirement: WAV preview playback
The user SHALL be able to preview a WAV slot's audio by clicking a play button on the row.

#### Scenario: Preview plays sound
- **WHEN** the user clicks the play button on a WAV row with a valid file
- **THEN** the audio engine SHALL play that sound once

### Requirement: BMP resource table display and editing
The system SHALL display and allow editing of the BMP resource table identically to the WAV table, with image thumbnails shown in each row where the file exists.

#### Scenario: Image thumbnail
- **WHEN** a BMP slot points to a valid image file
- **THEN** a small thumbnail of that image SHALL be shown in the row

### Requirement: Resource usage highlighting
The system SHALL indicate which WAV/BMP slots are actually used by notes in the current chart, and which are defined but unused.

#### Scenario: Used slot highlight
- **WHEN** a WAV slot is referenced by one or more notes in the chart
- **THEN** the row SHALL be visually highlighted differently from unused-but-defined slots
