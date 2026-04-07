## Design: Ableton Live-Inspired UI Redesign

### Architecture Overview

The redesign is purely at the QML layer. No C++ model/controller changes are needed. The existing `appController`, `editorDoc`, `audioEngine`, and resource models remain untouched.

### Layout Structure

**Current:**
```
┌─ MenuBar ──────────────────────────────────┐
├─ ToolBar (header) ─────────────────────────┤
├─ LeftPanel │ Canvas+Header │ RightPanel(Tab)┤
├─ StatusBar (footer) ───────────────────────┤
└────────────────────────────────────────────┘
```

**New (Ableton-style):**
```
┌─ ControlBar (transport│grid│zoom│menu) ────┐
├─ CanvasHeader ─────────────────┤ Sidebar ──┤
├─ ChartCanvas                   │ (sections)┤
├─ InfoStrip (thin bottom bar) ──┤───────────┤
└────────────────────────────────────────────┘
```

### Key Design Decisions

1. **No MenuBar** — All actions accessible via ControlBar buttons + overflow popup menu (hamburger icon). Keyboard shortcuts remain identical.

2. **ControlBar** — Single 40px-high bar:
   - Left zone: Logo/app name, file actions (new/open/save as icon buttons)
   - Center zone: Transport (play/stop), BPM display, grid subdivision selector
   - Right zone: Zoom controls, sidebar toggle, overflow menu button

3. **Sidebar replaces TabBar** — Instead of tabs (Header/WAV/BMP/Measures), use vertically stacked collapsible sections. Each section has a thin header that toggles expand/collapse. Only one section expanded at a time (accordion behavior) or freely expandable.

4. **Color Palette** — Shift from blue accent (#61afef) to warm Ableton-style:
   - Background: #0a0a0a (deepest), #1a1a1a (surface), #2a2a2a (panel), #333 (card)
   - Accent: #ff764d (Ableton orange) for active/selected states
   - Text: #fff (primary), #999 (secondary), #555 (disabled)
   - Borders: #333 (primary), #222 (faint)

5. **Flat buttons** — No Material elevation. Buttons are rectangles with subtle hover highlight, no border by default.

6. **Typography** — Keep existing font families, reduce font sizes slightly. Use letter-spacing for section headers.

### File Changes

| File | Change |
|------|--------|
| `components/Theme.qml` | New color palette, Ableton-inspired tokens |
| `Main.qml` | Remove menuBar/header/footer, add ControlBar + InfoStrip + new layout |
| `components/ControlBar.qml` | **NEW** — unified top bar |
| `components/OverflowMenu.qml` | **NEW** — popup menu replacing MenuBar |
| `components/SidebarSection.qml` | **NEW** — collapsible section widget |
| `components/CanvasHeader.qml` | Restyle with new colors |
| `components/BpmOverlay.qml` | Update overlay colors |
| `panels/HeaderPanel.qml` | Restyle fields |
| `panels/WavResourcePanel.qml` | Restyle list |
| `panels/BmpResourcePanel.qml` | Restyle list |
| `panels/MeasureLengthPanel.qml` | Restyle list |
| `panels/SettingsDialog.qml` | Flatten dialog style |
| `panels/FindReplaceDialog.qml` | Flatten dialog style |
| `panels/JumpToMeasureDialog.qml` | Flatten dialog style |
| `WelcomeScreen.qml` | Minimal redesign with new palette |

### Risks & Mitigations

- **Discoverability** — Removing the menu bar hides actions. Mitigation: overflow menu contains all actions; keyboard shortcuts unchanged.
- **Visual regression** — Palette change affects everything. Mitigation: Theme.qml is the single source of truth; systematic replacement.
