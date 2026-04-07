# Ableton Sidebar Spec

## Overview
Replace the tab-based right panel with Ableton-style vertically stacked collapsible sections.

## Requirements

### Sections
1. **Header** — BMS header fields (TITLE, ARTIST, BPM, etc.)
2. **WAV** — WAV resource slots
3. **BMP** — BMP resource slots
4. **Measures** — Measure length editor

### Section Widget (SidebarSection)
- Thin header bar (24px) with section title (uppercase, small font) and expand/collapse chevron
- Click header to toggle expand/collapse
- Smooth height animation on toggle
- Collapsed state shows only the header bar
- Expanded state shows full content

### Behavior
- Multiple sections can be expanded simultaneously
- Sidebar width: 280px (same as current)
- Sidebar toggle button in ControlBar shows/hides entire sidebar

### Visual Style
- Section headers: `bgPanel` background, uppercase title, `textSecondary` color
- Section content: `bgSurface` background
- 1px border between sections
- No tab bar chrome
