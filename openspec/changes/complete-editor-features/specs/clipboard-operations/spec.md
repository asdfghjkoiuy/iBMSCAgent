## ADDED Requirements

### Requirement: Editor Clipboard Interaction
The system SHALL support standard OS keyboard clipboard patterns (Ctrl+C, Ctrl+V, Ctrl+X) to move sequences of selected notes seamlessly.

#### Scenario: Copying notes
- **WHEN** the user presses `Ctrl+C` while notes are highlighted in the `ChartCanvas`
- **THEN** an internal serialize function translates the selected array into the system clipboard buffer, keeping track of the channel offsets and minimum measure beat offsets.

#### Scenario: Pasting notes
- **WHEN** the user presses `Ctrl+V`
- **THEN** the system reads the formatted clipboard string and invokes a mass `PlaceNoteCommand` via the undo stack, plotting the instances starting precisely at the currently scrolled viewing measure offset.