## ADDED Requirements

### Requirement: Measure Manipulation Grid Expansion
The system SHALL offer advanced utility commands allowing the creator to shift large groups of timing variables simultaneously without individual object dragging.

#### Scenario: Inserting an empty measure
- **WHEN** the user invokes "Insert Empty Measure" at `Measure X` via menu
- **THEN** all notes, BPM changes, and STOP blocks from `Measure X` onward have their internal indices bumped by `+1`, pushing the entire composition down while introducing blank space correctly.

#### Scenario: Halving Time Signature
- **WHEN** the user runs "Scale Beats by 0.5"
- **THEN** all targeted entity durations and absolute starting points subdivide by half, dynamically rewriting measure parameters.