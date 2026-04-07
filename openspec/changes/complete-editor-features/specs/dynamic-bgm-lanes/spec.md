## ADDED Requirements

### Requirement: Expandable Audio Channels
The visual graph framework SHALL expand laterally whenever a `.bms` file requires rendering tracks beyond a hardcoded base number of Background Music Lanes.

#### Scenario: Rendering an overloaded chart
- **WHEN** the `Model::BmsDocument` receives notes destined for extended channels (`BGM 04` through `BGM 32`)
- **THEN** the `ChartCanvasItem` adjusts the `kPlayerChannels` mapping loop dynamically to spawn and append parallel columns to the view width horizontally.