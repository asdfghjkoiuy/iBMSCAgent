## 1. Critical Bug Fixes

- [x] 1.1 Fix `closeConfirmDialog` positioning: add `anchors.centerIn: Overlay.overlay` or explicit x/y centering in Main.qml
- [x] 1.2 Fix all other dialogs (JumpToMeasure, FindReplace, Settings, ColumnConfig, BpmTools) to be centered
- [x] 1.3 Fix trackpad zoom: update `ChartCanvasItem::wheelEvent` to use `pixelDelta` for smooth pinch, `angleDelta` for discrete wheel
- [x] 1.4 Fix trackpad scroll: ensure two-finger scroll uses `pixelDelta.y()` for smooth scrolling

## 2. Note Model Extension

- [x] 2.1 Add `bool hidden = false` and `bool landmine = false` fields to `Note` struct in `Note.h`
- [x] 2.2 Update `BmsParser` to set `hidden`/`landmine` based on channel classification when parsing
- [x] 2.3 Update `BmsSerializer` to write hidden notes to channels 31-39/41-49 and landmine to D1-D9/E1-E9
- [x] 2.4 Update `ChartCanvasItem` note rendering to use distinct colors for hidden (semi-transparent) and landmine (red) notes

## 3. Splash Screen

- [x] 3.1 Create `qml/SplashOverlay.qml`: full-window dark overlay with app name, version, author, fade-out after 1.5s
- [x] 3.2 Add SplashOverlay to Main.qml, shown on Component.onCompleted, auto-hides with OpacityAnimator
- [x] 3.3 Add to qml.qrc

## 4. Note Conversion Commands

- [x] 4.1 Create `src/editor/commands/ConvertLongShortCommand.h/.cpp`: toggle selected notes long↔short
- [x] 4.2 Create `src/editor/commands/ConvertHiddenCommand.h/.cpp`: toggle selected notes hidden↔visible (channel shift)
- [x] 4.3 Create `src/editor/commands/MirrorNotesCommand.h/.cpp`: mirror column positions (A1↔A7, A2↔A6, A3↔A5)
- [x] 4.4 Create `src/editor/commands/StormCommand.h/.cpp`: randomize column assignments preserving beat positions
- [x] 4.5 Create `src/editor/commands/ModifyLabelsCommand.h/.cpp`: batch-set value on selected notes
- [x] 4.6 Add conversion slots to `EditorDocument`: `convertToLong()`, `convertToShort()`, `toggleLongShort()`, `convertToHidden()`, `convertToVisible()`, `toggleHidden()`, `mirrorNotes()`, `stormNotes()`, `modifyLabels(int value)`
- [x] 4.7 Add all new .cpp files to CMakeLists.txt

## 5. Time Selection Tools

- [x] 5.1 Add `TimeSelectMode = 3` to `ChartCanvasItem::EditMode` enum
- [x] 5.2 Implement time selection drag in canvas: click-drag creates horizontal beat range (stored as `timeSelectionStartBeat`/`timeSelectionEndBeat`)
- [x] 5.3 Create `src/editor/commands/ExpandByRatioCommand.h/.cpp`: scale note positions within time range by ratio
- [x] 5.4 Create `src/editor/commands/ReverseSelectionCommand.h/.cpp`: reverse note positions within time range
- [x] 5.5 Create `src/editor/commands/TransformToStopCommand.h/.cpp`: convert time range to STOP event
- [x] 5.6 Add time selection toolbar buttons to ControlBar.qml (Expand, Reverse, Transform-to-STOP)
- [x] 5.7 Wire F1 shortcut to TimeSelectMode in Main.qml

## 6. Advanced Find/Delete/Replace

- [x] 6.1 Rewrite `panels/FindReplaceDialog.qml` to match VB.NET diagFind: column checkboxes, measure/value range inputs, state filters
- [x] 6.2 Add Select/Unselect/Delete/Replace operation buttons
- [x] 6.3 Implement find logic in `EditorDocument`: `findNotes(filter)` returning matching note indices
- [x] 6.4 Implement batch select/unselect/delete/replace operations as undoable commands
- [x] 6.5 Add Select All / Select Inverse / Unselect All for column checkboxes

## 7. Editor Modes and Toggles

- [x] 7.1 Add `bool ntInputMode`, `bool errorCheck`, `bool previewOnClick`, `bool showFilename`, `bool wavAutoIncrease` properties to `AppSettings`
- [x] 7.2 Add toggle buttons to ControlBar.qml for each mode (NT, Error, Preview, Filename, WAV++)
- [x] 7.3 Implement preview-on-click: play WAV slot via AudioEngine when note is clicked/placed
- [x] 7.4 Implement show-filename: render WAV filename text on note rectangles in `updatePaintNode`
- [x] 7.5 Implement WAV auto-increase: increment write value after each note placement
- [x] 7.6 Implement error check overlay: detect duplicate positions, render red indicator
- [x] 7.7 Wire F8 shortcut to toggle NT/BMSE mode

## 8. Note Interactions

- [x] 8.1 Implement right-click delete: in `mousePressEvent`, right-click on note emits `noteDeleted`
- [x] 8.2 Implement double-click edit: detect double-click, open value input dialog, push EditValueCommand
- [x] 8.3 Implement arrow key movement: `keyPressEvent` for Up/Down/Left/Right moving selected notes by grid/column
- [x] 8.4 Implement number key column assignment: keys 0-8 move selected notes to corresponding column
- [x] 8.5 Implement L/S/H keyboard toggles: L = toggle long/short, S = to short, H = toggle hidden
- [x] 8.6 Implement drag-to-resize long notes: detect drag on top/bottom edge of LN, adjust duration

## 9. Statistics Dialog

- [x] 9.1 Create `panels/StatisticsDialog.qml`: table showing note counts by channel group and type
- [x] 9.2 Add `Q_INVOKABLE QVariantMap noteStatistics()` to `EditorDocument`
- [x] 9.3 Wire Ctrl+T shortcut and menu item

## 10. Theme System

- [x] 10.1 Create `src/app/ThemeManager.h/.cpp`: load/save JSON theme files, apply colors to QML via context properties
- [x] 10.2 Create default dark theme JSON with all color keys
- [x] 10.3 Add theme menu to OverflowMenu: Default, Save Theme, Load Theme, Import iBMSC 2.x
- [x] 10.4 Implement VB.NET XML theme import parser
- [x] 10.5 Update all QML color references to use theme properties instead of hardcoded values

## 11. Options Panel Completion

- [x] 11.1 Add missing header fields to HeaderPanel.qml: Player, Difficulty, ExRank, Total, Comment, LnObj
- [x] 11.2 Add file browse buttons for StageFile, Banner, BackBMP fields
- [x] 11.3 Create Expansion Code section in options panel with TextArea
- [x] 11.4 Enhance Beat section: measure list with beat fraction editor, context menu for insert/remove measure

## 12. Conversion Menu and Context Menu

- [x] 12.1 Add Conversion submenu to OverflowMenu: Long, Short, Long↔Short, Hidden, Visible, Hidden↔Visible, Modify Labels, Mirror
- [x] 12.2 Create right-click context menu on canvas with: Cut, Copy, Paste, Delete, Convert submenu
- [x] 12.3 Wire all conversion actions to EditorDocument slots

## 13. Integration and Polish

- [x] 13.1 Add all new QML files to qml.qrc
- [x] 13.2 Add all new .cpp/.h files to CMakeLists.txt
- [x] 13.3 Verify all keyboard shortcuts match VB.NET: F1-F8, Ctrl+N/O/S/Z/Y/X/C/V/A/G/F/T
- [x] 13.4 Verify clean Debug build on macOS with no new errors
- [x] 13.5 Smoke test: open BMS file, use all tools, save, verify output
