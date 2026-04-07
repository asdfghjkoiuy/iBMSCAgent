## 1. Column Configuration

- [x] 1.1 Add `ColumnConfig` struct to `src/model/` with fields: enabled, width, title, bmsChannel
- [x] 1.2 Add `QList<ColumnConfig> columns` to `AppSettings` with default 7P/5P presets; persist to QSettings
- [x] 1.3 Expose `columnConfigs` as a `QVariantList` property on `AppSettings` accessible from QML
- [x] 1.4 Update `ChartCanvasItem::updatePaintNode` to read per-column enabled/width from `AppSettings::columnConfigs`, caching layout as a local `QVector<int>` per frame
- [x] 1.5 Update `BmsSerializer` to use per-column BMS channel assignment from `AppSettings` when writing key tracks
- [x] 1.6 Create `panels/ColumnConfigDialog.qml`: table of columns with enable toggle, width spinbox, channel field, title field
- [x] 1.7 Add "Column Config…" action to `OverflowMenu.qml` and wire to `AppController::openColumnConfig()` slot
- [x] 1.8 Add column header click-to-toggle-enable on `CanvasHeader.qml`

## 2. BPM Tools

- [x] 2.1 Create `src/editor/commands/BpmToolsCommands.h/.cpp` with `ConstBpmCommand`, `ScaleBpmCommand`, `AreaToStopCommand`
- [x] 2.2 Implement `ConstBpmCommand`: compute absolute time for each note using `TimingMap`, remove all BPM change notes, reposition remaining notes to target BPM
- [x] 2.3 Implement `ScaleBpmCommand`: multiply all BPM note values and all note VPositions by ratio
- [x] 2.4 Implement `AreaToStopCommand`: remove notes in beat range, insert STOP event of equivalent duration, shift subsequent notes
- [x] 2.5 Add `AppController` slots: `constBpm(int targetBpm)`, `scaleBpm(double ratio)`, `areaToStop()`
- [x] 2.6 Create `panels/BpmToolsDialog.qml`: tabs for ConstBPM (target BPM input), Scale (ratio input), ×2/÷2 buttons, Area-to-STOP button
- [x] 2.7 Add "BPM Tools…" action to `OverflowMenu.qml` and wire to `AppController::openBpmTools()`

## 3. ibmsc Native Format

- [ ] 3.1 Create `src/io/IbmscFormat.h/.cpp` with `IbmscFormat::save(const BmsDocument&, const AppSettings&, QFile&)` and `IbmscFormat::load(QFile&) → IbmscLoadResult{BmsDocument, AppSettings delta}`
- [ ] 3.2 Implement XML writer: header fields, WAV/BMP tables, note list (channel, vposition, value, length), column config, color settings
- [ ] 3.3 Implement XML reader with lenient parsing (ignore unknown elements); restore all fields written by the writer
- [ ] 3.4 Add `.ibmsc` to supported file extensions in `AppController` open/save dialogs
- [ ] 3.5 Wire "Save as ibmsc" and "Open ibmsc" through existing `AppController` file actions
- [ ] 3.6 Write round-trip unit test: create document, save to ibmsc, reload, assert note count and header equality

## 4. StepMania Import

- [ ] 4.1 Create `src/io/SmImporter.h/.cpp` with `SmImporter::import(QFile&) → SmImportResult{difficulties, warnings}`
- [ ] 4.2 Implement SM parser: extract `#BPMs`, `#STOPS`, and all `#NOTES` blocks with difficulty labels
- [ ] 4.3 Implement note conversion: map SM 4-key/8-key rows to BMS channels using configurable lane map; convert beat positions to VPosition
- [ ] 4.4 Implement BPM/STOP event conversion to BMS BPM change notes and STOP notes
- [ ] 4.5 Detect unsupported features (warps, negative BPMs, rolls) and populate `warnings` list
- [ ] 4.6 Create `panels/SmImportDialog.qml`: difficulty selector list, lane mapping table, warnings display, import button
- [ ] 4.7 Add "Import SM…" to `OverflowMenu.qml` and wire to `AppController::importSm()`

## 5. Note Statistics

- [ ] 5.1 Add `noteCount()` and `selectedNoteCount()` computed properties to `EditorDocument`, emitting change signals
- [ ] 5.2 Add note count and selected count display to the status bar strip in `Main.qml`, bound to `EditorDocument` properties
- [ ] 5.3 Create `panels/StatisticsDialog.qml`: `ListView` of channel rows (channel name + count) sorted by channel index, total row at bottom
- [ ] 5.4 Add "Statistics…" action to `OverflowMenu.qml` and wire to `AppController::openStatistics()`

## 6. O2Mania Keysound Adjustment

- [ ] 6.1 Create `src/editor/commands/O2AdjustCommand.h/.cpp` implementing replace, swap, and shift modes on note WAV indices
- [ ] 6.2 Implement replace mode: iterate all notes, replace matching source index with target index
- [ ] 6.3 Implement swap mode: swap source↔target indices across all notes in one pass
- [ ] 6.4 Implement shift mode: add offset to all note WAV indices, clamping to 1–1295
- [ ] 6.5 Create `panels/O2AdjustDialog.qml`: source/target index spinboxes, mode radio buttons (Replace/Swap/Shift), apply button
- [ ] 6.6 Add "O2 Adjustment…" to `OverflowMenu.qml` and wire to `AppController::openO2Adjust()`

## 7. Visual Theme Settings

- [ ] 7.1 Add color fields to `AppSettings`: `bgColor`, `beatLineColor`, `measureLineColor`, `subBeatLineColor`, `QList<QColor> columnNoteColors`; persist all to QSettings
- [ ] 7.2 Extend `Theme.qml` with `columnColors` array and global line color properties, reading from `AppSettings`
- [ ] 7.3 Update `ChartCanvasItem` to read per-column note colors and global line/bg colors from `AppSettings` properties
- [ ] 7.4 Create `panels/ColorPickerDialog.qml`: HSV sliders (hue, saturation, value), hex input field, live preview swatch; validate hex input
- [ ] 7.5 Create `panels/VisualSettingsDialog.qml`: grid of column color swatches (click to open ColorPickerDialog), global color rows for bg/lines, live canvas preview
- [ ] 7.6 Add "Visual Settings…" to `OverflowMenu.qml` and wire to `AppController::openVisualSettings()`

## 8. Locale System

- [ ] 8.1 Create `src/app/LocaleLoader.h/.cpp`: parse locale XML file into `QMap<QString,QString>`, install via `QQmlEngine::retranslate()` after populating a custom `QTranslator` subclass
- [ ] 8.2 Register `LocaleLoader` as a context property; expose `loadLocale(path)` slot callable from QML
- [ ] 8.3 Add locale file path setting to `AppSettings`; call `LocaleLoader::loadLocale()` on startup if path is set
- [ ] 8.4 Add locale file picker row to `SettingsDialog.qml` General tab with file browse button
- [ ] 8.5 Show restart-required dialog when locale setting changes (do not apply mid-session)
- [ ] 8.6 Audit all QML string literals in `qml/` and wrap with `qsTr()` where missing

## 9. CMake and Integration

- [ ] 9.1 Add all new `.cpp` files to `CMakeLists.txt` target sources
- [ ] 9.2 Add all new QML dialog files to `qml.qrc`
- [ ] 9.3 Verify clean Debug build on macOS with no new warnings
- [ ] 9.4 Smoke test: open a real `.bms`, run ConstBPM, save as `.ibmsc`, reopen, verify note count matches
