## ADDED Requirements

### Requirement: Static Background Waveform
The canvas engine SHALL be capable of rendering a transparent histogram of a specific assigned background audio file mapped against the Y-axis timeline.

#### Scenario: Visual audio synchronization check
- **WHEN** the user assigns an audio file to a global preview track AND enables the "Show Waveform" toggle
- **THEN** a semi-transparent set of amplitude rectangles is painted vertically down the center backing columns, scaling according to the current `pxPerBeat` zoom level correctly.