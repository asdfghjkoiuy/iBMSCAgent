## ADDED Requirements

### Requirement: Long Notes Visual Representation
The system SHALL visualize sustained notes differently from single-point notes, specifically rendering a continuous colored rectangular bar from the start beat to the end beat within a channel.

#### Scenario: Rendering an LN node
- **WHEN** the canvas redraws a note structure with `durationInBeats > 0`
- **THEN** it generates a corresponding top node for the start beat, bottom node for the end beat, and connects them visually using an intersecting rect node representing the sustain track.

### Requirement: Long Notes Conversion via `#LNOBJ`
The system SHALL map `#LNOBJ` references to internal dual-note bindings upon load and reserialize them upon save.

#### Scenario: File read parser interpreting `#LNOBJ`
- **WHEN** the parsed file encounters a standard note and a subsequent note matching the loaded `#LNOBJ` index in the same channel
- **THEN** the parser merges them into a single `Model::Note` entity with the calculated `durationInBeats`, discarding the tail node.