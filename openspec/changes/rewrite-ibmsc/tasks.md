## 1. Project Scaffolding

- [x] 1.1 Create Qt 6 CMake project with directories: `src/model`, `src/io`, `src/audio`, `src/editor`, `src/app`, `qml/components`, `qml/canvas`, `qml/panels`
- [x] 1.2 Configure CMakeLists.txt with Qt6 components: Core, Quick, QuickControls2, Multimedia, Qml
- [x] 1.3 Set up CMakePresets.json for Debug/Release on macOS and Windows
- [x] 1.4 Add `.gitignore`, `compile_commands.json` export for clangd
- [x] 1.5 Configure Qt Quick Controls 2 with a custom dark style baseline (`qtquickcontrols2.conf`)

## 2. BMS Core Model

- [x] 2.1 Implement `Base36` utility: `int toInt(const char* s)`, `std::string fromInt(int n)`, covering 01–ZZ
- [x] 2.2 Define `Note` struct: channelIndex, measureIndex, numerator/denominator beat position, value
- [x] 2.3 Define `BmsDocument` class: header map, WAV table (1295 entries), BMP table, BPM table, STOP table, measure-length map, note list
- [x] 2.4 Implement channel-type classification: `ChannelType classifyChannel(int ch)` returning Normal/LN/Hidden/BGA/Sound/Mine/BPM/STOP
- [x] 2.5 Implement `TimingMap`: compute absolute time (seconds) for any (measure, beat) given BPM and STOP events
- [x] 2.6 Write unit tests for Base36 round-trip, TimingMap with BPM changes, and channel classification

## 3. BMS File I/O

- [x] 3.1 Implement encoding detection utility supporting UTF-8 BOM, Shift-JIS heuristic, EUC-JP heuristic
- [x] 3.2 Implement `BmsParser::parse(QFile&)` → `ParseResult{BmsDocument, warnings}` handling header lines and `#MMMMCC:data` rows
- [x] 3.3 Implement base-36 channel data row decoder: split data string into equal slots, map to note positions
- [x] 3.4 Implement `BmsSerializer::save(const BmsDocument&, QFile&, Encoding)` with UTF-8 and Shift-JIS output
- [x] 3.5 Write round-trip test: parse a real BMS file, save, re-parse, assert documents are equal
- [x] 3.6 Add parse warning types: missing BPM, duplicate channel row, invalid base-36 token, out-of-range slot

## 4. Editor State and Undo/Redo

- [x] 4.1 Create `EditorDocument` class wrapping `BmsDocument` + `QUndoStack`, owned by the app
- [x] 4.2 Implement `PlaceNoteCommand`: redo = insert note, undo = remove note
- [x] 4.3 Implement `DeleteNotesCommand`: redo = remove list of notes, undo = re-insert them
- [x] 4.4 Implement `MoveNotesCommand`: redo = shift beat positions, undo = shift back
- [x] 4.5 Implement `EditHeaderCommand`: redo/undo header field value swap
- [x] 4.6 Implement `EditWavSlotCommand` and `EditBmpSlotCommand`: redo/undo resource filename swap
- [x] 4.7 Implement `EditBpmCommand` and `EditStopCommand` for timing event edits
- [x] 4.8 Wire `QUndoStack::cleanChanged` signal to dirty-state tracking; expose `isDirty` property to QML
- [x] 4.9 Register `EditorDocument` as a QML singleton context property

## 5. Audio Engine

- [x] 5.1 Implement `AudioEngine` class: load keysound file (WAV/OGG/MP3) into decoded PCM buffer using `QAudioDecoder` or libsndfile
- [x] 5.2 Implement mixing loop: `QAudioSink` with callback that mixes up to 64 active sound instances into output buffer
- [x] 5.3 Implement `AudioEngine::play(int slotIndex)` — trigger a keysound from its decoded buffer
- [x] 5.4 Implement `AudioEngine::scheduleChart(const BmsDocument&, double startTimeSec)` — queue all BGM channel events
- [x] 5.5 Implement playback start/stop/seek with position reporting via Qt signal at ≥60Hz
- [x] 5.6 Implement audio device enumeration and selection, exposed as a QML-accessible list
- [ ] 5.7 Add fallback to miniaudio single-header if `QAudioSink` latency exceeds 30ms on target platform

## 6. Chart Canvas (QQuickItem)

- [x] 6.1 Create `ChartCanvasItem : QQuickItem` subclass registered as `ChartCanvas` QML element
- [x] 6.2 Implement `updatePaintNode`: build a `QSGNode` tree for column backgrounds, beat lines, measure lines, and ruler ticks — viewport-culled
- [x] 6.3 Implement note rendering nodes: `QSGGeometryNode` rectangles for normal notes, long notes (full-height bar), and mine notes (distinct color)
- [x] 6.4 Implement BPM/STOP marker rendering as horizontal lines with text labels
- [x] 6.5 Implement vertical scroll position (`scrollBeat` property) and zoom (`pxPerBeat` property) bound from QML
- [x] 6.6 Implement grid subdivision property and snap logic: snap candidate beat to nearest grid multiple
- [x] 6.7 Implement mouse press/move/release handlers for note placement (left-click place/delete) and rubber-band selection
- [x] 6.8 Implement drag-to-move selected notes with grid snap and delta clamping to valid range
- [x] 6.9 Implement playback cursor line as a separate `QSGGeometryNode` updated from audio position signal
- [x] 6.10 Implement auto-scroll during playback: adjust `scrollBeat` to keep cursor in view
- [ ] 6.11 Performance test: load 50,000-note chart, assert 60fps scroll with ≤5% CPU overhead

## 7. QML Application Shell

- [x] 7.1 Create `Main.qml` with `ApplicationWindow`, a `ColumnLayout` containing `MenuBar`, `ToolBar`, `SplitView`, and `StatusBar`
- [x] 7.2 Implement `MenuBar` with File / Edit / View / Chart / Help menus wired to C++ slots via `Connections`
- [x] 7.3 Implement `ToolBar` with New, Open, Save, Undo, Redo, Play/Stop, grid selector `ComboBox`, zoom `+/−` buttons
- [x] 7.4 Implement collapsible left sidebar panel with smooth height/width animation (150ms ease-in-out)
- [x] 7.5 Implement collapsible right panel with `TabView`: Header, WAV Resources, BMP Resources
- [x] 7.6 Implement `WelcomeScreen.qml`: New File button, Open File button, recent files `ListView`
- [x] 7.7 Implement drag-and-drop file open: `DropArea` over the main window accepting .bms/.bme/.bml/.pms
- [x] 7.8 Implement `Theme.qml` singleton with dark palette: background, surface, primary accent, text colors, corner radii, spacing constants
- [x] 7.9 Implement HiDPI-aware rendering: set `Qt.AA_EnableHighDpiScaling`, verify canvas at 2× scale
- [x] 7.10 Implement close confirmation dialog using `MessageDialog` when document is dirty
- [x] 7.11 Implement command-line argument parsing in `main.cpp`: positional file path, `-p`, `-m <measure>`

## 8. Resource Panel

- [x] 8.1 Create `ResourceTableModel : QAbstractListModel` exposing slot index, filename, usage count, thumbnail (for BMP)
- [x] 8.2 Implement `WavResourcePanel.qml`: `ListView` with slot index, filename label, play-preview button per row
- [x] 8.3 Implement `BmpResourcePanel.qml`: same as WAV panel with an additional `Image` thumbnail column
- [x] 8.4 Implement inline filename editor: double-click row → `TextField` becomes editable, Enter commits via `EditWavSlotCommand`
- [x] 8.5 Implement file-picker button per row: opens `FileDialog` filtered to audio/image types, sets filename on confirm
- [x] 8.6 Implement play-preview button: calls `AudioEngine::play(slotIndex)` for WAV slots
- [x] 8.7 Implement usage highlighting: rows used by notes get a distinct accent color; unused-but-defined rows get a muted style

## 9. Header and Chart Properties Panel

- [x] 9.1 Create `HeaderPanel.qml` with labeled `TextField` rows for: TITLE, SUBTITLE, ARTIST, SUBARTIST, GENRE, BPM, PLAYLEVEL, RANK, TOTAL, VOLWAV, STAGEFILE, BANNER, BACKBMP, LNOBJ, LNTYPE
- [x] 9.2 Each field change SHALL push an `EditHeaderCommand` onto the undo stack
- [x] 9.3 Add a measure-length editor: `ListView` of measures with non-default length highlighted; inline editor for measure length fraction

## 10. Settings and Preferences

- [x] 10.1 Create `AppSettings` singleton class (`QObject` + `QSettings`): grid default, scroll speed, column width preset, note color scheme, audio device, recent files list
- [x] 10.2 Register `AppSettings` as a QML context property
- [x] 10.3 Create `SettingsDialog.qml`: tabbed dialog with General, Editor, Audio, Key Bindings pages
- [x] 10.4 Implement recent files persistence: update list on open, trim to 10 entries, remove missing files on startup
- [x] 10.5 Implement audio device list in settings Audio tab, wired to `AudioEngine::setDevice()`
- [x] 10.6 Implement key bindings viewer table in settings Key Bindings tab (read-only display for v1)

## 11. Integration and Polish

- [x] 11.1 Wire file open/save/new actions end-to-end: menu → `BmsParser`/`BmsSerializer` → `EditorDocument` → canvas refresh
- [x] 11.2 Wire play/stop actions: toolbar → `AudioEngine::scheduleChart` with current scroll position as start time
- [x] 11.3 Wire undo/redo keyboard shortcuts (Ctrl+Z / Ctrl+Y) and verify menu items enable/disable correctly
- [x] 11.4 Implement "Jump to Measure" dialog: `InputDialog` accepting measure number 0–999, scrolls canvas
- [x] 11.5 Implement Find/Replace for header values and resource filenames
- [ ] 11.6 End-to-end smoke test: open a real BMS file, place/delete/move notes, save, diff output against original
- [ ] 11.7 Run on Windows and verify audio and rendering parity with macOS build
- [ ] 11.8 Package with `macdeployqt` / `windeployqt` and verify standalone launch without Qt installed
