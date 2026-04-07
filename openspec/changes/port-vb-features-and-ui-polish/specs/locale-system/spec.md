## ADDED Requirements

### Requirement: XML locale file loading
The system SHALL load UI strings from an XML locale file at runtime, replacing default English strings with localized equivalents. The locale file path SHALL be configurable in settings.

#### Scenario: Load locale file
- **WHEN** the user selects a locale XML file in settings
- **THEN** all UI strings that have translations in the file are updated to the localized text

#### Scenario: Missing translation falls back to English
- **WHEN** a locale file is loaded but a string key is absent
- **THEN** the original English string is displayed

### Requirement: Locale change requires restart
The system SHALL prompt the user to restart the application after changing the locale setting, as full re-translation requires a fresh QML engine load.

#### Scenario: Locale change prompt
- **WHEN** the user selects a new locale and closes settings
- **THEN** a dialog informs the user that the change will take effect after restart

### Requirement: No locale file = English default
The system SHALL function normally with all English strings when no locale file is configured.

#### Scenario: Default English UI
- **WHEN** no locale file is set in settings
- **THEN** all UI strings display in English without errors
