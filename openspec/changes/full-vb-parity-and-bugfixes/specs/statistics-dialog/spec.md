## ADDED Requirements

### Requirement: Statistics dialog shows note counts by type
The Statistics dialog SHALL display counts of notes grouped by: BPM, STOP, SCROLL, A-group (P1), D-group (P2), BGM, with sub-counts for short/long/hidden/landmine/error notes.

#### Scenario: User opens statistics
- **WHEN** user selects Edit > Statistics (Ctrl+T)
- **THEN** a modal dialog shows a table of note counts by channel group and note type

### Requirement: Statistics update from current document
The statistics SHALL reflect the current state of the loaded document at the time the dialog opens.

#### Scenario: After editing notes
- **WHEN** user places several notes then opens Statistics
- **THEN** the counts reflect all placed notes including the most recent edits
