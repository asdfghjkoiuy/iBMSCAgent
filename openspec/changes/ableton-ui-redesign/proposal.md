## Why

The current iBMSC UI uses a traditional desktop application layout (menu bar + toolbar + panels) that feels dated compared to modern DAW-style editors. Ableton Live's flat, minimal UI has become a benchmark for music production tools — adopting a similar aesthetic will make iBMSC feel more professional and intuitive for BMS chart authors who are already familiar with DAW workflows.

## What Changes

- **Remove the traditional MenuBar** — replace with a minimal top control bar containing transport controls, grid settings, and a hamburger/overflow menu for less-used actions
- **Redesign the toolbar** into a unified "Control Bar" with Ableton-style flat buttons, transport section (play/stop/record-like), and BPM/grid display
- **Flatten the color palette** — reduce gradients and shadows; use Ableton-inspired muted grays (#1a1a1a to #333) with orange/warm accent instead of blue
- **Redesign the right sidebar** — use Ableton-style collapsible sections with minimal chrome instead of tab bar
- **Update the status bar** — make it thinner and more minimal, Ableton-style bottom info strip
- **Restyle all dialogs** — flatten buttons, reduce borders, use consistent dark theme tokens
- **Update Theme.qml** — new Ableton-inspired color tokens and spacing constants
- **Redesign WelcomeScreen** — cleaner, more minimal landing page matching the new aesthetic

## Capabilities

### New Capabilities
- `ableton-control-bar`: Unified top control bar replacing menu bar and toolbar — contains transport, grid, zoom, and hamburger menu
- `ableton-theme`: Flat, dark Ableton Live-inspired theme tokens (colors, spacing, typography)
- `ableton-sidebar`: Collapsible sidebar sections replacing tab-based right panel

### Modified Capabilities
<!-- No existing specs are being modified -->

## Impact

- All QML files under `qml/` will be modified
- `Theme.qml` color palette and spacing values change significantly
- `Main.qml` layout structure changes (menuBar removed, header replaced)
- No C++ backend changes required — this is purely a QML/UI layer refactor
- No functional behavior changes — all features remain identical
