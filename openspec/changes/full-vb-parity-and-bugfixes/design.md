## Context

iBMSC is a BMS chart editor originally written in VB.NET. The Qt/C++ rewrite has core editing, file I/O, audio preview, and basic UI working. However it's missing ~60% of the original features and has two critical bugs. The VB.NET codebase at `iBMSC/` serves as the reference implementation.

Current state:
- Chart canvas with note placement, selection, drag-move works
- BMS parse/save works
- Audio engine works (with Qt 6.11 API fix applied)
- Column config, BPM tools partially implemented
- Missing: most dialogs, note conversion tools, time selection, keyboard shortcuts, theme system, splash screen

## Goals / Non-Goals

**Goals:**
- Fix the two reported bugs (dialog positioning, trackpad zoom)
- Achieve 1:1 feature parity with VB.NET iBMSC for all user-facing functionality
- Match VB.NET interaction patterns (keyboard shortcuts, mouse behaviors, tool modes)
- Flat Ableton Live / terminal-aesthetic visual design throughout
- All dialogs functional and centered

**Non-Goals:**
- Windows build verification (deferred)
- Performance optimization beyond 60fps baseline
- Plugin/extension system
- Network features
- Replicating VB.NET internal architecture — only external behavior matters

## Decisions

**1. Dialog centering: use `anchors.centerIn: Overlay.overlay`**
All modal dialogs will use `anchors.centerIn: Overlay.overlay` or explicit `x: (parent.width - width) / 2; y: (parent.height - height) / 2` positioning. The current bug is that `Dialog {}` without positioning defaults to (0,0).

**2. Trackpad zoom: use `pixelDelta` for smooth pinch, `angleDelta` for discrete scroll**
macOS trackpad sends `pixelDelta` for pinch gestures. Current code only reads `angleDelta` which gives jerky/wrong behavior. Fix: check `pixelDelta` first, fall back to `angleDelta` for mouse wheels.

**3. Note model extension: add Hidden, Landmine, LNPair fields to Note struct**
VB.NET Note has: Value, Length, LongNote, Hidden, Landmine, LNPair, Selected, HasError. Current C++ Note is missing Hidden, Landmine, LNPair. Add these as bool/int fields.

**4. Tool modes: TimeSelection (F1), Select (F2), Write (F3) — match VB.NET exactly**
Add TimeSelection mode to the existing EditMode enum. Time selection draws a horizontal range on the canvas and enables time-based operations (expand, relocate, reverse, transform-to-stop).

**5. Conversion commands as QUndoCommand subclasses**
Each note conversion (Long↔Short, Hidden↔Visible, Mirror, Storm, Modify Labels) is a separate QUndoCommand for full undo/redo support.

**6. Theme system: JSON-based color scheme files**
Store theme as JSON with named color keys. Load/save via AppSettings. Default dark theme built-in. Compatible with VB.NET theme import (parse XML format from iBMSC 2.x).

**7. Splash screen: QML overlay shown for 1.5s on startup**
Not a native splash — a full-window QML overlay with fade-out animation, matching VB.NET splash appearance but in flat style.

## Risks / Trade-offs

- [Large scope] → Implement in priority order: bugs first, then core editor features, then polish features. Each group is independently useful.
- [Note model changes] → Adding fields to Note struct may affect serialization. Ensure BMS round-trip tests still pass after changes.
- [VB.NET behavior ambiguity] → Some VB.NET interactions are complex (NT vs BMSE LN mode). Implement NT mode first as primary, BMSE as secondary.
- [Theme compatibility] → VB.NET themes use .NET Color serialization. JSON import will need a color format converter.
