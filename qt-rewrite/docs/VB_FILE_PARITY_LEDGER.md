# VB File Parity Ledger

This ledger tracks one-to-one implementation parity from VB source files to Qt rewrite files.

## Status Legend
- `DONE`: behavior and UI parity implemented.
- `PARTIAL`: baseline exists but VB behavior has gaps.
- `MISSING`: not implemented.

## Core Mapping
| VB Source File | Qt Target File(s) | Status | Acceptance Criteria |
|---|---|---|---|
| `MainWindow.designer.vb` | `src/ui/main_window.cpp` | PARTIAL | PO stack layout, switches, expanders, resizers, menu/toolbar parity |
| `MainWindow.vb` | `src/ui/main_window.cpp` | PARTIAL | menu/toolbar commands, option toggles, data panel choreography |
| `PanelDraw.vb` | `src/ui/chart_editor_widget.cpp` | PARTIAL | draw layers, hover, waveform overlay, channel filter parity |
| `PanelEvents.vb` | `src/ui/chart_editor_widget.cpp` | PARTIAL | write/select/time-select edge behavior parity |
| `PanelKeyStates.vb` | `src/ui/chart_editor_widget.cpp` | PARTIAL | key state transitions parity |
| `ChartIO.vb` | `src/io/bms_parser.cpp` | PARTIAL | parser/save branch parity and edge-case compatibility |
| `BMS.vb` | `src/core/bms_document.cpp` | PARTIAL | channel semantics, measure math, table behavior parity |
| `UndoRedo.vb` | `src/core/undo/*` | MISSING | command-style undo stack with VB command classes parity |
| `EditorUndoRedo.vb` | `src/core/undo/*`, `src/ui/main_window.cpp` | MISSING | editor-integrated command dispatch parity |
| `EditorPersistent.vb` | `src/core/settings/*`, `src/ui/main_window.cpp` | PARTIAL | full settings schema, locale/theme injection, panel state parity |
| `VisualSettings.vb` | `src/theme/theme_loader.cpp` | PARTIAL | all visual fields mapped and applied |
| `Waveform.vb` | `src/ui/chart_editor_widget.cpp`, `src/audio/*` | MISSING | waveform cache/render/options parity |
| `Audio.vb` | `src/audio/audio_preview_service.cpp` | PARTIAL | full playback fallback and external player parity |
| `MyO2.vb` | `src/ui/main_window.cpp` | PARTIAL | full algorithm and edge-case parity |
| `Dialogs/diagFind.vb` | `src/ui/dialogs/diag_find_dialog.*` | MISSING | full filter logic and UI parity |
| `Dialogs/dgStatistics.vb` | `src/ui/dialogs/statistics_dialog.*` | MISSING | full table parity |
| `Dialogs/dgMyO2.vb` | `src/ui/dialogs/myo2_dialog.*` | MISSING | full data grid and workflow parity |
| `Dialogs/dgImportSM.vb` | `src/ui/dialogs/import_sm_dialog.*` | MISSING | import pipeline parity |
| `Dialogs/ColorPicker.vb` | `src/ui/dialogs/color_picker_dialog.*` | MISSING | HSL/RGB/A precision UI parity |
| `Dialogs/AboutBox1.vb` | `src/ui/dialogs/about_dialog.*` | MISSING | visual and copy behavior parity |
| `Dialogs/SplashScreen1.vb` | `src/ui/dialogs/splash_dialog.*` | MISSING | startup splash parity |
| `Dialogs/fLoadFileProgress.vb` | `src/ui/dialogs/load_progress_dialog.*` | MISSING | progress/cancel behavior parity |
| `SCompareMethod.vb` | `src/ui/dialogs/compare_method_dialog.*` | MISSING | compare strategy selection parity |
| `Option Windows/OpGeneral.vb` | `src/ui/dialogs/op_general_dialog.*` | MISSING | options and persistence parity |
| `Option Windows/OpVisual.vb` | `src/ui/dialogs/op_visual_dialog.*` | MISSING | visual settings editor parity |
| `Option Windows/OpPlayer.vb` | `src/ui/dialogs/op_player_dialog.*` | MISSING | external player profile parity |

## Resource Mapping
| VB Resource Root | Qt Target | Status |
|---|---|---|
| `My Project/Resources.resx` | `resources/ibmsc.qrc`, `src/ui/resource_catalog.*` | PARTIAL |
| `ImageButtonsSmall/*` | `resources/ibmsc.qrc` | PARTIAL |
| `ImageButtons/*` | `resources/ibmsc.qrc` | MISSING |
| `MiscButtons/*` | `resources/ibmsc.qrc` | MISSING |

## Validation Backlog
- [ ] Add IO golden tests under `tests/parity/io_golden/`.
- [ ] Add interaction replay scripts under `tests/parity/interaction/`.
- [ ] Add screenshot baseline set under `tests/parity/snapshots/`.
