## MODIFIED Requirements

### Requirement: Editor Selection Mechanics modifying objects
The generic multi-object bounding mechanism currently drags and drops single instance beats correctly, but MUST be updated to accommodate dragging elements spanning durations.

#### Scenario: Dragging a Long Note head
- **WHEN** the selection focuses explicitly on the starting boundary of a Long Note item instead of the center
- **THEN** the dragging cursor mutates standard measure locations instead by contracting/flexing the `durationInBeats` variable keeping the tail locked statically on the grid.