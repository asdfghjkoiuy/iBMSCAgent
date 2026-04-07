## ADDED Requirements

### Requirement: Main window layout
The application SHALL present a main window with: a menu bar, a toolbar with common actions, a central chart canvas area, a left sidebar (column visibility / quick actions), a right panel (header fields, resource table, measure list), a bottom panel (playback controls, status), and a status bar.

#### Scenario: Default layout on first launch
- **WHEN** the application launches for the first time
- **THEN** the main window SHALL show the chart canvas occupying the majority of screen space with sidebars visible

#### Scenario: Panel toggle
- **WHEN** the user presses the keyboard shortcut to hide the right panel
- **THEN** the right panel SHALL collapse with a smooth animation and the canvas SHALL expand to fill the space

### Requirement: Menu bar
The menu bar SHALL contain: File (New, Open, Open Recent, Save, Save As, Close, Quit), Edit (Undo, Redo, Cut, Copy, Paste, Select All, Find/Replace), View (Zoom In/Out, Grid subdivision, Column visibility), Chart (Play/Stop, Jump to measure, Chart properties), Help (About).

#### Scenario: File > Open triggers file dialog
- **WHEN** the user selects File > Open
- **THEN** a native file dialog SHALL open filtered to .bms, .bme, .bml, .pms files

### Requirement: Toolbar
The toolbar SHALL show buttons for: New, Open, Save, Undo, Redo, Play/Stop, grid subdivision selector, zoom controls.

#### Scenario: Toolbar play button state
- **WHEN** audio playback is active
- **THEN** the play button SHALL change to a stop button icon

### Requirement: Drag-and-drop file open
The application window SHALL accept drag-and-drop of BMS/BME/BML/PMS files from the OS file manager.

#### Scenario: Drop BMS file onto window
- **WHEN** the user drags a .bms file from Finder/Explorer onto the window
- **THEN** the file SHALL be opened in the editor

### Requirement: Dark theme and visual polish
The application SHALL use a dark color theme by default. UI transitions (panel open/close, dialog appear/disappear) SHALL use smooth QML animations. Font rendering SHALL be crisp on HiDPI/Retina displays.

#### Scenario: HiDPI rendering
- **WHEN** the application runs on a 2× Retina display
- **THEN** all UI elements and the chart canvas SHALL render at full native resolution without blurriness

#### Scenario: Panel animation
- **WHEN** a collapsible panel is toggled
- **THEN** it SHALL animate open/close over approximately 150ms with an ease-in-out curve

### Requirement: Welcome screen
When no document is open, the application SHALL display a welcome screen with buttons for New File, Open File, and a scrollable recent files list.

#### Scenario: Welcome screen shown on launch
- **WHEN** the application starts with no command-line arguments and no session to restore
- **THEN** the welcome screen SHALL be shown in the central area

### Requirement: Command-line file open
The application SHALL accept a BMS file path as a positional command-line argument and open it at startup. It SHALL also accept `-p` and `-m <measure>` arguments for preview-mode compatibility with Pulsus.

#### Scenario: Open from CLI
- **WHEN** the application is launched with `iBMSC chart.bms`
- **THEN** the file SHALL be opened immediately without showing the welcome screen
