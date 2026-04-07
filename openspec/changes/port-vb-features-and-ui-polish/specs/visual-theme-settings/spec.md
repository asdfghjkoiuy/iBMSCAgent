## ADDED Requirements

### Requirement: Per-column note colors
The system SHALL allow the user to assign a custom color to each column's notes. Colors SHALL be applied immediately on the canvas and persisted in settings.

#### Scenario: Change column note color
- **WHEN** the user picks a new color for column 2
- **THEN** all notes in column 2 render with the new color immediately

### Requirement: Global background and line colors
The system SHALL allow the user to customize the canvas background color, beat line color, measure line color, and sub-beat line color.

#### Scenario: Change background color
- **WHEN** the user sets a new background color in visual settings
- **THEN** the canvas background updates immediately

### Requirement: Color picker dialog
The system SHALL provide an inline color picker dialog with hue/saturation/value sliders and a hex input field for precise color entry.

#### Scenario: Enter hex color
- **WHEN** the user types a valid hex color code in the color picker
- **THEN** the sliders update to reflect the entered color

#### Scenario: Invalid hex rejected
- **WHEN** the user types an invalid hex string
- **THEN** the field shows an error state and the color is not applied

### Requirement: Visual settings persistence
All color and font customizations SHALL be saved to the settings XML file and restored on next launch.

#### Scenario: Colors restored on restart
- **WHEN** the user customizes colors and restarts the app
- **THEN** the same colors are applied to the canvas
