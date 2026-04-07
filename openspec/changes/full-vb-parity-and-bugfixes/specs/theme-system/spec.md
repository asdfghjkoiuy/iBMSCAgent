## ADDED Requirements

### Requirement: Save and load visual themes
The system SHALL support saving the current color scheme as a named JSON theme file and loading themes from file.

#### Scenario: Save current theme
- **WHEN** user selects "Save Theme" from theme menu
- **THEN** current colors are saved to a JSON file in the themes directory

#### Scenario: Load theme
- **WHEN** user selects a theme from the theme menu
- **THEN** all editor colors update to match the loaded theme

### Requirement: Import VB.NET iBMSC 2.x themes
The system SHALL parse and import theme files from the VB.NET iBMSC 2.x XML format.

#### Scenario: Import legacy theme
- **WHEN** user selects "Load Theme File from iBMSC 2.x"
- **THEN** the XML theme is parsed and applied as current colors

### Requirement: Default dark theme
The system SHALL ship with a built-in dark theme matching the Ableton Live flat aesthetic.

#### Scenario: First launch
- **WHEN** application launches with no saved theme preference
- **THEN** the default dark theme is applied
