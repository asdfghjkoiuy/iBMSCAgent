## ADDED Requirements

### Requirement: Advanced Find/Delete/Replace dialog
The system SHALL provide a Find/Delete/Replace dialog matching VB.NET diagFind with column checkboxes, note/measure/value range filters, and select/unselect/delete/replace operations.

#### Scenario: Find notes by value range
- **WHEN** user opens Find dialog, sets value range 01-0Z, and clicks Select
- **THEN** all notes with values in that range are selected in the editor

#### Scenario: Delete notes in measure range
- **WHEN** user sets measure range 10-20 and clicks Delete
- **THEN** all notes in measures 10-20 matching the column filter are deleted, undoable

#### Scenario: Replace note labels
- **WHEN** user sets a filter and clicks Replace with target label
- **THEN** matching notes have their value changed to the target, undoable

### Requirement: Column filter checkboxes
The dialog SHALL have per-column checkboxes (all A-group, D-group, BGA, BGM columns) with Select All / Select Inverse / Unselect All buttons.

#### Scenario: Filter by specific columns
- **WHEN** user unchecks all columns except A1 and A3
- **THEN** only notes in A1 and A3 columns are affected by the operation

### Requirement: State filters
The dialog SHALL filter by note state: Selected/Unselected, Short/Long, Hidden/Visible.

#### Scenario: Find only long notes
- **WHEN** user checks "Long" filter and clicks Select
- **THEN** only long notes matching other criteria are selected
