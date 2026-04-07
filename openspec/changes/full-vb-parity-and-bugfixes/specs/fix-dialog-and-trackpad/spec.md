## ADDED Requirements

### Requirement: Save dialog SHALL be centered in window
The close-confirmation dialog SHALL appear centered within the application window, not at the default (0,0) position.

#### Scenario: User closes with unsaved changes
- **WHEN** user closes the window with unsaved changes
- **THEN** the save confirmation dialog appears centered in the application window

### Requirement: Trackpad pinch-to-zoom SHALL work smoothly on macOS
The chart editor SHALL handle macOS trackpad pinch gestures using pixelDelta for smooth zoom, falling back to angleDelta for discrete mouse wheel events.

#### Scenario: Pinch zoom on trackpad
- **WHEN** user performs a pinch gesture on macOS trackpad over the chart canvas
- **THEN** the canvas zooms smoothly in/out proportional to the gesture magnitude

#### Scenario: Mouse wheel zoom
- **WHEN** user scrolls with Ctrl held on a discrete mouse wheel
- **THEN** the canvas zooms in/out in discrete steps as before

### Requirement: All modal dialogs SHALL be centered
Every modal Dialog in the application SHALL be positioned at the center of its parent window.

#### Scenario: Any dialog opens
- **WHEN** any modal dialog is opened (settings, jump-to-measure, find/replace, column config, BPM tools, etc.)
- **THEN** it appears centered in the application window
