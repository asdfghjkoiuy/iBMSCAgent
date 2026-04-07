## ADDED Requirements

### Requirement: Save to ibmsc format
The system SHALL save the current document as a `.ibmsc` XML file containing: all header fields, WAV/BMP tables, note list with channel and position data, column configuration, and visual settings. Undo history SHALL NOT be persisted.

#### Scenario: Save ibmsc file
- **WHEN** the user saves as `.ibmsc`
- **THEN** an XML file is written containing all document data and column/visual config

### Requirement: Load from ibmsc format
The system SHALL open a `.ibmsc` XML file and restore the document state including notes, header, WAV/BMP tables, column config, and visual settings. Unknown XML elements SHALL be silently ignored for forward compatibility.

#### Scenario: Open ibmsc file
- **WHEN** the user opens a `.ibmsc` file
- **THEN** the document is fully restored including column config and colors

#### Scenario: Unknown elements ignored
- **WHEN** a `.ibmsc` file contains elements not in the current schema
- **THEN** the file loads successfully without error

### Requirement: ibmsc round-trip fidelity
A document saved to `.ibmsc` and re-opened SHALL produce an identical document (same notes, header, WAV/BMP tables, column config).

#### Scenario: Round-trip test
- **WHEN** a document is saved to ibmsc and re-opened
- **THEN** note count, positions, header values, and WAV table match the original
