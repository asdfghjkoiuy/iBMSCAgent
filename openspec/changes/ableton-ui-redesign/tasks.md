# Tasks: Ableton UI Redesign

## Phase 1: Theme Foundation
- [x] Update `Theme.qml` with Ableton-inspired color palette, flat geometry tokens, and typography constants

## Phase 2: New Components
- [x] Create `components/SidebarSection.qml` — collapsible section widget with header bar + expand/collapse animation
- [x] Create `components/ControlBar.qml` — unified top control bar with transport, grid, zoom, and overflow menu
- [x] Create `components/OverflowMenu.qml` — popup menu containing all actions from the old MenuBar

## Phase 3: Main Layout
- [x] Rewrite `Main.qml` — remove menuBar/header/footer, integrate ControlBar + new sidebar layout + thin info strip; keep all Shortcuts at ApplicationWindow level

## Phase 4: Restyle Existing Components
- [x] Restyle `components/CanvasHeader.qml` with new theme colors
- [x] Restyle `components/BpmOverlay.qml` with new accent colors
- [x] Restyle `panels/HeaderPanel.qml` with flat field styling
- [x] Restyle `panels/WavResourcePanel.qml` with flat list styling
- [x] Restyle `panels/BmpResourcePanel.qml` with flat list styling
- [x] Restyle `panels/MeasureLengthPanel.qml` with flat list styling

## Phase 5: Dialogs & Screens
- [x] Restyle `panels/SettingsDialog.qml` — flat buttons, dark background, no Material elevation
- [x] Restyle `panels/FindReplaceDialog.qml` — flat styling
- [x] Restyle `panels/JumpToMeasureDialog.qml` — flat styling
- [x] Redesign `WelcomeScreen.qml` — minimal Ableton-inspired landing page with new palette
