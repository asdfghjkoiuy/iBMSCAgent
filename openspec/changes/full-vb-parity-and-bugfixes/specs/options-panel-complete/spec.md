## ADDED Requirements

### Requirement: Expansion Code editor
The Options Panel SHALL include an Expansion Code section with a free-text editor for custom BMS expansion codes.

#### Scenario: Edit expansion code
- **WHEN** user types in the expansion code text area
- **THEN** the expansion code is stored in the document and saved with the file

### Requirement: Full Beat editor with insert/remove measure
The Beat section SHALL display a measure list with beat numerator/denominator, and support insert/remove measure via context menu.

#### Scenario: Insert measure
- **WHEN** user right-clicks measure list and selects "Insert Measure" at measure 5
- **THEN** a new empty measure is inserted at position 5, shifting subsequent measures, undoable

#### Scenario: Remove measure
- **WHEN** user right-clicks measure list and selects "Remove Measure" at measure 5
- **THEN** measure 5 and its notes are removed, subsequent measures shift down, undoable

#### Scenario: Edit beat fraction
- **WHEN** user changes the beat numerator/denominator for a measure
- **THEN** the measure length is updated accordingly, undoable

### Requirement: Complete header fields
The Header section SHALL include all VB.NET fields: Title, Artist, Genre, BPM, Player, Rank, PlayLevel, SubTitle, SubArtist, StageFile (with browse), Banner (with browse), BackBMP (with browse), Difficulty, ExRank, Total, Comment, LnObj.

#### Scenario: Edit all header fields
- **WHEN** user modifies any header field
- **THEN** the change is pushed to the undo stack and reflected in the document
