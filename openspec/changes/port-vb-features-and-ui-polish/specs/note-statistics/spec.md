## ADDED Requirements

### Requirement: Note count dialog
The system SHALL provide a statistics dialog showing the total note count and per-channel note count for the current document.

#### Scenario: Open statistics dialog
- **WHEN** the user opens the statistics dialog
- **THEN** a table is shown with each active channel and its note count, plus a total row

### Requirement: Status bar live note count
The system SHALL display the total note count in the status bar, updated whenever notes are added, removed, or the document changes.

#### Scenario: Note count updates on edit
- **WHEN** the user places or deletes a note
- **THEN** the status bar note count updates immediately

### Requirement: Selected note count display
The system SHALL display the count of currently selected notes in the status bar alongside the total count.

#### Scenario: Selection count shown
- **WHEN** the user selects a group of notes
- **THEN** the status bar shows both selected count and total count
