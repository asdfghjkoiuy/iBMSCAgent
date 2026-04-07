## ADDED Requirements

### Requirement: Persistent application settings
The system SHALL persist user preferences using `QSettings` in a platform-appropriate location. Settings SHALL survive application restarts.

#### Scenario: Settings persist across restart
- **WHEN** the user changes a setting and restarts the application
- **THEN** the setting SHALL be restored to the value set before restart

### Requirement: Recent files list
The system SHALL maintain a list of the 10 most recently opened BMS files. The list SHALL be accessible from the File menu and the welcome screen.

#### Scenario: Recent file added
- **WHEN** the user opens a BMS file
- **THEN** that file's path SHALL appear at the top of the recent files list

#### Scenario: Recent file removed if missing
- **WHEN** a file in the recent list no longer exists on disk
- **THEN** it SHALL be removed from the list automatically on next app launch

### Requirement: Editor grid and visual preferences
The user SHALL be able to configure: default grid subdivision, vertical scroll speed, column width preset (compact / normal / wide), note color scheme, and whether beat numbers are shown on the ruler.

#### Scenario: Grid subdivision persists
- **WHEN** the user sets the grid to 1/16 and restarts
- **THEN** the grid SHALL still be 1/16 on next launch

### Requirement: Audio device setting
The user SHALL be able to select the preferred audio output device name. The setting SHALL be applied at application start and when changed in the settings dialog.

#### Scenario: Apply audio device change
- **WHEN** the user changes the audio device in settings and clicks Apply
- **THEN** the audio engine SHALL reinitialize with the new device without restarting the application

### Requirement: Key binding configuration
The user SHALL be able to view all keyboard shortcuts in a settings panel. Key bindings SHALL be configurable and persisted.

#### Scenario: View shortcuts
- **WHEN** the user opens the key bindings settings page
- **THEN** a list of all actions and their current key bindings SHALL be shown
