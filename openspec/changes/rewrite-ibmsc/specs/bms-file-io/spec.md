## ADDED Requirements

### Requirement: BMS file parsing
The system SHALL parse .bms, .bme, .bml, and .pms files into a BmsDocument. The parser SHALL handle UTF-8, Shift-JIS, and EUC-JP encodings, auto-detecting encoding from byte-order marks or heuristic detection.

#### Scenario: Parse header fields
- **WHEN** a BMS file with "#TITLE My Song" is parsed
- **THEN** the document header SHALL contain key "TITLE" with value "My Song"

#### Scenario: Parse WAV resource definition
- **WHEN** a line "#WAV01 kick.wav" is parsed
- **THEN** slot 1 of the WAV table SHALL contain filename "kick.wav"

#### Scenario: Parse channel data row
- **WHEN** a line "#00111:0101020000" is parsed
- **THEN** measure 1 channel 17 (0x11) SHALL contain two notes at beat positions 0/4 and 1/4 with slot values 1 and 2

#### Scenario: Handle unknown lines gracefully
- **WHEN** a line with an unrecognized directive is encountered
- **THEN** the parser SHALL skip it without throwing and MAY record a warning

#### Scenario: Encoding detection Shift-JIS
- **WHEN** a file without BOM contains Shift-JIS encoded Japanese text
- **THEN** the parser SHALL correctly decode the text to Unicode

### Requirement: BMS file serialization
The system SHALL serialize a BmsDocument back to a valid BMS text file. The output SHALL be byte-for-byte round-trippable for files that were loaded without modification.

#### Scenario: Save round-trip
- **WHEN** a file is loaded and immediately saved to a new path
- **THEN** re-parsing the saved file SHALL produce an identical BmsDocument

#### Scenario: Save encoding choice
- **WHEN** saving a file, the user MAY choose UTF-8 (with BOM) or Shift-JIS
- **THEN** the saved file SHALL use the chosen encoding throughout

### Requirement: File validity reporting
The system SHALL report parse warnings and errors to the caller via a result type, including: missing required header fields, slot index out of range, duplicate channel row in same measure, and invalid base-36 tokens.

#### Scenario: Missing #BPM header
- **WHEN** a BMS file has no #BPM directive
- **THEN** the parser SHALL succeed with a warning and use 130 BPM as default

#### Scenario: Out-of-range slot index
- **WHEN** a channel data row references slot "00"
- **THEN** the parser SHALL record a warning and skip that note object
