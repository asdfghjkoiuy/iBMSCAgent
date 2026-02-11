# VB Function-Level Parity Ledger

This ledger tracks VB -> Qt parity at function/event granularity.

## Status Legend
- `DONE`: Qt behavior/effect matches VB.
- `IN_PROGRESS`: implemented partially, parity verification pending.
- `MISSING`: not implemented.

## P0 Blockers
| VB Source | VB Function/Event | Qt Target | Status | Acceptance Case |
|---|---|---|---|---|
| `iBMSC/iBMSC/EditorColumns.vb` | `BMSChannelToColumn`, `GetBMSChannelBy` | `qt-rewrite/src/core/bms_document.cpp` (`channelToColumn`, `columnToDefaultChannel`) | IN_PROGRESS | `COL-001` |
| `iBMSC/iBMSC/EditorColumns.vb` | `nTitle`, `nEnabled`, `IsColumnSound`, `IsColumnNumeric` | `qt-rewrite/src/core/bms_document.cpp`, `qt-rewrite/src/theme/theme_loader.cpp`, `qt-rewrite/src/ui/chart_editor_widget.cpp` | IN_PROGRESS | `COL-002` |
| `iBMSC/iBMSC/ChartIO.vb` | `#WAV/#BMP` parse/save | `qt-rewrite/src/io/bms_parser.cpp` | IN_PROGRESS | `IO-001` |
| `iBMSC/iBMSC/MainWindow.vb` | `Form1_FormClosing` | `qt-rewrite/src/ui/main_window.cpp` (`closeEvent`) | IN_PROGRESS | `SAVE-001` |
| `iBMSC/iBMSC/MainWindow.vb` | `POWAV_Resize`, `POBMP_Resize` | `qt-rewrite/src/ui/main_window.cpp`, `qt-rewrite/src/ui/po_panel_manager.cpp` | IN_PROGRESS | `UI-001` |

## Input/Editing Core
| VB Source | VB Function/Event | Qt Target | Status | Acceptance Case |
|---|---|---|---|---|
| `iBMSC/iBMSC/PanelEvents.vb` | `PMainInMouseDown`, `OnWriteModeMouseMove` | `qt-rewrite/src/ui/chart_editor_widget.cpp` (`mousePressEvent`, `mouseMoveEvent`) | IN_PROGRESS | `EDIT-001` |
| `iBMSC/iBMSC/PanelEvents.vb` | `GetClickedNote`, `MouseInNote` | `qt-rewrite/src/ui/chart_editor_widget.cpp` (`hitTestNote`, NT hit logic) | IN_PROGRESS | `EDIT-002` |
| `iBMSC/iBMSC/PanelEvents.vb` | `PMainInMouseUp` overwrite/remove rules | `qt-rewrite/src/ui/chart_editor_widget.cpp` (`applyWriteAt`, release handlers) | IN_PROGRESS | `EDIT-003` |
| `iBMSC/iBMSC/PanelKeyStates.vb` | delete and modifier behavior | `qt-rewrite/src/ui/chart_editor_widget.cpp` (`keyPressEvent`) | IN_PROGRESS | `KEY-001` |
| `iBMSC/iBMSC/PanelEvents.vb` | Ctrl-wheel zoom and scroll coupling | `qt-rewrite/src/ui/chart_editor_widget.cpp` (`wheelEvent`, `event`) | IN_PROGRESS | `ZOOM-001` |

## Theme/Visual
| VB Source | VB Function/Event | Qt Target | Status | Acceptance Case |
|---|---|---|---|---|
| `iBMSC/iBMSC/VisualSettings.vb` | visual fields load/apply | `qt-rewrite/src/theme/theme_loader.cpp` | IN_PROGRESS | `THEME-001` |
| `iBMSC/iBMSC/PanelDraw.vb` | column captions and lane colors | `qt-rewrite/src/ui/chart_editor_widget.cpp` (`paintEvent`) | IN_PROGRESS | `DRAW-001` |

## Dialog/Tooling Backlog
| VB Source | Qt Target | Status |
|---|---|---|
| `Dialogs/dgStatistics.vb` | `qt-rewrite/src/ui/dialogs/statistics_dialog.cpp` | MISSING |
| `Dialogs/dgMyO2.vb` | `qt-rewrite/src/ui/dialogs/myo2_dialog.cpp` | MISSING |
| `Dialogs/dgImportSM.vb` | `qt-rewrite/src/ui/dialogs/import_sm_dialog.cpp` | MISSING |
| `Option Windows/OpGeneral.vb` | `qt-rewrite/src/ui/dialogs/op_general_dialog.cpp` | MISSING |
| `Option Windows/OpVisual.vb` | `qt-rewrite/src/ui/dialogs/op_visual_dialog.cpp` | MISSING |
| `Option Windows/OpPlayer.vb` | `qt-rewrite/src/ui/dialogs/op_player_dialog.cpp` | MISSING |
| `Dialogs/AboutBox1.vb` | `qt-rewrite/src/ui/dialogs/about_dialog.cpp` | MISSING |
| `Dialogs/SplashScreen1.vb` | `qt-rewrite/src/ui/dialogs/splash_dialog.cpp` | MISSING |

## Immediate Acceptance Checklist
- `COL-001`: same BMS channel hits same lane index as VB.
- `COL-002`: lane labels/titles use VB naming semantics (`Measure/BPM/STOP/SCROLL/A*/D*/B*`).
- `IO-001`: `#WAV/#BMP` remains stable after open/save/open.
- `SAVE-001`: close without explicit save never overwrites source file.
- `UI-001`: `WAV/OGG`, `BGI/BMP` panel heights do not collapse after reopen.
- `EDIT-001`: write/select/NT operations maintain VB conflict and overwrite behavior.
- `EDIT-002`: clicking LN body edits existing LN rather than creating stacked note.
- `KEY-001`: mac keyboard delete works with both `Delete` and `Backspace`.
- `ZOOM-001`: pinch and modifier zoom are viewport zoom, not scrollbar-range scaling.
