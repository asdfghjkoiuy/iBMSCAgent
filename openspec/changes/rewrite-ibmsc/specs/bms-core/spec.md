## ADDED Requirements

### Requirement: BMS document model
The system SHALL represent a BMS document as a structured in-memory object containing: a header map (key→value string pairs), a WAV resource table (slots 01–ZZ, 1295 entries), a BMP resource table (slots 01–ZZ), a BPM table (named BPMs), a STOP table, measure length overrides, and a list of notes organized by measure and channel.

#### Scenario: Create empty document
- **WHEN** a new BMS document is created
- **THEN** it SHALL contain a default BPM of 130, 1000 empty measures, and no notes

#### Scenario: Access header field
- **WHEN** code reads a header key such as "TITLE"
- **THEN** the document SHALL return the string value or an empty string if absent

### Requirement: Note representation
The system SHALL represent each note as a value type with: channel index (integer), measure index (integer), beat position within measure (rational number as numerator/denominator pair to avoid floating-point drift), and a value (resource slot index or BPM/STOP value depending on channel type).

#### Scenario: Long note pair
- **WHEN** a long-note start and end are stored
- **THEN** they SHALL be stored as two separate notes with channels in the LN range, linked by matching slot value

#### Scenario: BPM change note
- **WHEN** a BPM change is stored
- **THEN** it SHALL use channel index 3 (BPM channel) with the BPM value encoded as the note value

### Requirement: Channel type classification
The system SHALL classify channel indices into: Normal, LongNoteStart, LongNoteEnd, Hidden, HiddenLongNoteStart, BGA, Layer, PoorBGA, Sound, Mine, BGAExtended.

#### Scenario: Classify visible note channels
- **WHEN** channel index is in 11–19 or 21–29 (base-36: 11–19, 21–29)
- **THEN** it SHALL be classified as Normal player note channel

#### Scenario: Classify long note channels
- **WHEN** channel index is in the LN range (51–59, 61–69 in base-36)
- **THEN** it SHALL be classified as LongNote channel

### Requirement: Base-36 encoding utilities
The system SHALL provide utilities to convert between integer indices (1–1295) and base-36 two-character strings (01–ZZ, digits 0–9 then A–Z).

#### Scenario: Integer to base-36
- **WHEN** integer 1 is converted
- **THEN** the result SHALL be "01"

#### Scenario: Integer to base-36 upper bound
- **WHEN** integer 1295 is converted
- **THEN** the result SHALL be "ZZ"

#### Scenario: Base-36 to integer
- **WHEN** string "A0" is converted
- **THEN** the result SHALL be the correct integer value (10 × 36 = 360)

### Requirement: Measure timing computation
The system SHALL compute absolute timing (in seconds) for any beat position given the BPM and STOP event sequence, supporting variable BPM and STOP channels.

#### Scenario: Constant BPM timing
- **WHEN** BPM is constant at 120
- **THEN** beat 1.0 in measure 0 SHALL be at 0.5 seconds (one beat = 60/120s)

#### Scenario: BPM change mid-measure
- **WHEN** a BPM change note exists at beat 0.5 in measure 0
- **THEN** timing for beat positions after the change SHALL reflect the new BPM
