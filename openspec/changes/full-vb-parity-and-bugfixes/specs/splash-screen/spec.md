## ADDED Requirements

### Requirement: Splash screen on startup
The application SHALL display a splash screen overlay for 1.5 seconds on startup, showing the iBMSC logo/title and version, with a fade-out animation.

#### Scenario: Application launches
- **WHEN** the application starts
- **THEN** a full-window splash overlay appears with branding for 1.5s then fades out

### Requirement: Splash screen matches VB.NET appearance
The splash screen SHALL replicate the VB.NET SplashScreen1 layout (logo, version, author) in flat Ableton-style design.

#### Scenario: Visual appearance
- **WHEN** splash screen is visible
- **THEN** it displays app name, version number, and author credit in flat dark style
