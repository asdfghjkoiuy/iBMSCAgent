## ADDED Requirements

### Requirement: ConstBPM normalization
The system SHALL provide a "Normalize to Constant BPM" operation that converts all BPM change events into adjusted note positions, resulting in a chart with a single constant BPM. The operation SHALL be undoable.

#### Scenario: Normalize multi-BPM chart
- **WHEN** the user invokes ConstBPM with target BPM 130 on a chart with BPM changes
- **THEN** all BPM change notes are removed and remaining notes are repositioned to preserve their absolute timing at BPM 130

#### Scenario: Undo ConstBPM
- **WHEN** the user undoes after a ConstBPM operation
- **THEN** the chart is restored to its pre-normalization state

### Requirement: BPM halve and double
The system SHALL provide operations to halve or double all BPM values and note positions simultaneously, preserving absolute timing. Both operations SHALL be undoable.

#### Scenario: Double BPM
- **WHEN** the user invokes "BPM ×2"
- **THEN** all BPM values are doubled and note VPositions are doubled accordingly

#### Scenario: Halve BPM
- **WHEN** the user invokes "BPM ÷2"
- **THEN** all BPM values are halved and note VPositions are halved accordingly

### Requirement: Area-to-STOP conversion
The system SHALL allow the user to select a time range and convert it to a STOP event, removing notes in that range and inserting a STOP of equivalent duration. The operation SHALL be undoable.

#### Scenario: Convert selected area to STOP
- **WHEN** the user selects a beat range and invokes "Convert to STOP"
- **THEN** notes in the range are removed, a STOP event is inserted, and subsequent notes shift accordingly

### Requirement: BPM change by ratio
The system SHALL allow the user to multiply all BPM values by a user-specified ratio (e.g. 1.5×). Note positions SHALL be adjusted to preserve absolute timing. The operation SHALL be undoable.

#### Scenario: Scale BPM by ratio
- **WHEN** the user enters ratio 0.75 and applies
- **THEN** all BPM values are multiplied by 0.75 and note positions adjusted to match
