# iBMSC VB -> Qt Full Gap Matrix

Updated: 2026-02-09

Scope: compare `iBMSC/iBMSC/*.vb` feature set with `qt-rewrite/src/*` current implementation.

Legend:
- `DONE`: behavior implemented and wired in UI.
- `PARTIAL`: has basic equivalent but not full parity.
- `MISSING`: no implementation yet.

## 1. MainWindow / Designer / Panel System
- `PARTIAL` Main menus/toolbars: core items exist, but many original commands and icon/toolstrip semantics are missing.
- `PARTIAL` 3-panel editor (L/C/R): split + scroll sync exists, but original splitter resize behavior and PO panel choreography are not fully replicated.
- `MISSING` Full PO collapsible stack behavior (`POHeader/POGrid/POWaveForm/POWAV/POBMP/POBeat/POExpansion`) including original resizer interactions.
- `PARTIAL` status bar fields: core selection/time text exists; full original status fields and live values are incomplete.

## 2. Chart IO / BMS Compatibility (ChartIO.vb, BMS.vb)
- `PARTIAL` parse/save for common tags/channels implemented.
- `PARTIAL` LN/NT semantics: NT length model now exists and long-channel pairing/save is implemented, but edge-case parity (overlap conflict policy, special channel interactions) is incomplete.
- `MISSING` full ibmsc binary/session I/O compatibility.
- `MISSING` full SM import/export parity (`dgImportSM` pipeline).
- `MISSING` original rare channel/pathological file handling compatibility.

## 3. Editor Interaction (PanelEvents.vb, PanelDraw.vb, PanelKeyStates.vb)
- `DONE` write/select/time-select base modes, snapping, click add/remove.
- `DONE` mac trackpad scroll + pinch zoom support.
- `PARTIAL` write tool parity: drag write/erase, modifiers, NT drag are present; exact BMSE/NT micro-behavior still differs.
- `PARTIAL` select tool parity: move, copy-drag, label multi-select implemented; full original temp-state and all edge guards still differ.
- `PARTIAL` time selection parity: start/mid/end drag implemented; full BPM-change post-effects parity not yet complete.
- `PARTIAL` keyboard parity: most core shortcuts exist, but not all original tool/command mappings.
- `PARTIAL` drawing parity: theme colors and core grid/notes exist; full original rendering styles/effects/hover variants are incomplete.

## 4. Audio / Waveform (Audio.vb, Waveform.vb)
- `PARTIAL` WAV/OGG preview and fallback implemented.
- `MISSING` waveform import/clear/lock/transparency/width/offset/precision controls.
- `MISSING` waveform overlay render parity and interaction.
- `MISSING` complete player option profiles and external player integration parity (`OpPlayer` behavior).

## 5. BGI/BMP/Media
- `PARTIAL` BMP table and static image preview implemented.
- `MISSING` video BGI playback/preview behavior parity.
- `MISSING` all BGI behavior edge-cases from original panel interactions.

## 6. Theme / Visual Settings (VisualSettings.vb, Theme XML)
- `PARTIAL` theme XML loading and core column colors/widths implemented.
- `MISSING` full visual parameter mapping (all pens/brushes/fonts/k-series fields and behavior).
- `MISSING` full theme save/export and compatibility with original parameter surface.
- `MISSING` option windows parity for visual/general/player settings UI and persistence details.

## 7. Undo/Redo and Edit Commands (UndoRedo.vb, EditorUndoRedo.vb)
- `PARTIAL` snapshot-based undo/redo exists.
- `MISSING` command-granular undo model parity (typed operations like note add/remove/move/long-modify/time-selection ops).
- `MISSING` full redo chain behavior parity for complex time/NT edits.

## 8. MyO2
- `PARTIAL` `Tools -> MyO2` now exists in Qt with:
  - constant BPM conversion,
  - grid check,
  - grid adjust dialog flow.
- `MISSING` exact algorithm parity and all edge-case behaviors with VB `MyO2.vb`.
- `MISSING` parity-level validation against original outputs on real charts.

## 9. Utility Dialogs / Tools
- `PARTIAL` statistics/find/replace basic tools exist.
- `MISSING` full `diagFind` behavior parity.
- `MISSING` compare method dialog (`SCompareMethod`) parity.
- `MISSING` color picker / option dialogs parity (`ColorPicker`, `OpGeneral`, `OpVisual`, `OpPlayer` full behavior).
- `MISSING` load progress/splash/about integration parity.

## 10. Persistent Settings / Recent / Session Behavior
- `PARTIAL` recent files and core settings exist.
- `MISSING` full original persistent setting coverage and schema parity.
- `MISSING` all toolbar/menu localization/state restoration parity.

## 11. Performance / Regression Coverage
- `MISSING` large-chart performance parity validation.
- `MISSING` automated regression tests comparing VB vs Qt read/write and interaction behavior.
- `MISSING` golden corpus diff tooling for IO and behavioral parity.

## Current Priority Order (Execution)
1. Complete waveform panel + controls parity.
2. Complete PO panel/resizer and option windows parity.
3. Upgrade undo/redo from snapshots to command model.
4. Close remaining NT/BMSE edge-case behavior gaps.
5. Finish MyO2 parity and verify against VB outputs.
6. Complete media/BGI and external player option parity.
