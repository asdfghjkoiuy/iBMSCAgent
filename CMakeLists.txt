cmake_minimum_required(VERSION 3.21)

project(iBMSC VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Widgets Quick QuickControls2 Multimedia Qml)
qt_standard_project_setup(REQUIRES 6.5)

# ── Model library (no Qt dependency) ──────────────────────────────────────────
add_library(ibmsc_model STATIC
    src/model/Note.h
    src/model/Note.cpp
    src/model/BmsDocument.h
    src/model/BmsDocument.cpp
    src/model/Base36.h
    src/model/Base36.cpp
    src/model/ChannelType.h
    src/model/ChannelType.cpp
    src/model/TimingMap.h
    src/model/TimingMap.cpp
)
target_include_directories(ibmsc_model PUBLIC src)

# ── I/O library ───────────────────────────────────────────────────────────────
add_library(ibmsc_io STATIC
    src/io/EncodingDetector.h
    src/io/EncodingDetector.cpp
    src/io/BmsParser.h
    src/io/BmsParser.cpp
    src/io/BmsSerializer.h
    src/io/BmsSerializer.cpp
)
target_include_directories(ibmsc_io PUBLIC src)
target_link_libraries(ibmsc_io PUBLIC ibmsc_model Qt6::Core)

# ── Audio library ─────────────────────────────────────────────────────────────
add_library(ibmsc_audio STATIC
    src/audio/AudioEngine.h
    src/audio/AudioEngine.cpp
)
target_include_directories(ibmsc_audio PUBLIC src)
target_link_libraries(ibmsc_audio PUBLIC ibmsc_model Qt6::Core Qt6::Multimedia)
if(APPLE)
    target_link_libraries(ibmsc_audio PUBLIC "-framework CoreAudio" "-framework AudioToolbox" "-framework CoreFoundation")
elseif(WIN32)
    target_link_libraries(ibmsc_audio PUBLIC winmm ole32)
endif()

# ── Editor library ────────────────────────────────────────────────────────────
add_library(ibmsc_editor STATIC
    src/editor/EditorDocument.h
    src/editor/EditorDocument.cpp
    src/editor/commands/PlaceNoteCommand.h
    src/editor/commands/PlaceNoteCommand.cpp
    src/editor/commands/DeleteNotesCommand.h
    src/editor/commands/DeleteNotesCommand.cpp
    src/editor/commands/RemoveNoteCommand.h
    src/editor/commands/RemoveNoteCommand.cpp
    src/editor/commands/MoveNoteCommand.h
    src/editor/commands/MoveNoteCommand.cpp
    src/editor/commands/MoveNotesCommand.h
    src/editor/commands/MoveNotesCommand.cpp
    src/editor/commands/EditHeaderCommand.h
    src/editor/commands/EditHeaderCommand.cpp
    src/editor/commands/EditResourceCommand.h
    src/editor/commands/EditResourceCommand.cpp
    src/editor/commands/EditTimingCommand.h
    src/editor/commands/EditTimingCommand.cpp
    src/editor/commands/TimeSelectionCommand.h
    src/editor/commands/TimeSelectionCommand.cpp
    src/editor/commands/DeleteMeasureCommand.h
    src/editor/commands/DeleteMeasureCommand.cpp
    src/editor/commands/CopyMeasureCommand.h
    src/editor/commands/CopyMeasureCommand.cpp
    src/editor/commands/PasteMeasureCommand.h
    src/editor/commands/PasteMeasureCommand.cpp
    src/editor/commands/BpmToolsCommands.h
    src/editor/commands/BpmToolsCommands.cpp
    src/editor/commands/ConvertLongShortCommand.h
    src/editor/commands/ConvertLongShortCommand.cpp
    src/editor/commands/ConvertHiddenCommand.h
    src/editor/commands/ConvertHiddenCommand.cpp
    src/editor/commands/MirrorNotesCommand.h
    src/editor/commands/MirrorNotesCommand.cpp
    src/editor/commands/StormCommand.h
    src/editor/commands/StormCommand.cpp
    src/editor/commands/ModifyLabelsCommand.h
    src/editor/commands/ModifyLabelsCommand.cpp
    src/editor/commands/TimeSelectionTools.h
    src/editor/commands/TimeSelectionTools.cpp
    src/editor/commands/SetMeasureLengthCommand.h
    src/editor/commands/SetMeasureLengthCommand.cpp
)
target_include_directories(ibmsc_editor PUBLIC src)
target_link_libraries(ibmsc_editor PUBLIC ibmsc_model ibmsc_io Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Qml)

# ── SDL2 ─────────────────────────────────────────────────────────────────────
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)

# ── Player library (SDL2-based preview) ──────────────────────────────────────
add_library(ibmsc_player STATIC
    src/player/PlayerChart.h
    src/player/PlayerChart.cpp
    src/player/PlayerAudio.h
    src/player/PlayerAudio.cpp
    src/player/PlayerRenderer.h
    src/player/PlayerRenderer.cpp
    src/player/PlayerWindow.h
    src/player/PlayerWindow.cpp
    src/player/PlayerBridge.h
    src/player/PlayerBridge.cpp
)
target_include_directories(ibmsc_player PUBLIC src)
target_link_libraries(ibmsc_player PUBLIC ibmsc_model ibmsc_audio PkgConfig::SDL2)

# ── App settings ──────────────────────────────────────────────────────────────
add_library(ibmsc_app STATIC
    src/app/AppSettings.h
    src/app/AppSettings.cpp
    src/app/AppController.h
    src/app/AppController.cpp
    src/app/ResourceTableModel.h
    src/app/ResourceTableModel.cpp
    src/app/MeasureLengthModel.h
    src/app/MeasureLengthModel.cpp
    src/app/ThemeManager.h
    src/app/ThemeManager.cpp
)
target_include_directories(ibmsc_app PUBLIC src)
target_link_libraries(ibmsc_app PUBLIC ibmsc_editor ibmsc_audio ibmsc_player Qt6::Core Qt6::Qml)

# ── Chart canvas QQuickItem ───────────────────────────────────────────────────
add_library(ibmsc_canvas STATIC
    qml/canvas/ChartCanvasItem.h
    qml/canvas/ChartCanvasItem.cpp
)
target_include_directories(ibmsc_canvas PUBLIC qml/canvas src)
target_link_libraries(ibmsc_canvas PUBLIC ibmsc_editor ibmsc_audio Qt6::Quick)

# ── QML resources ─────────────────────────────────────────────────────────────
qt_add_resources(QML_RESOURCES qml.qrc)

# ── Main executable ───────────────────────────────────────────────────────────
qt_add_executable(iBMSC
    src/app/main.cpp
    ${QML_RESOURCES}
)
target_link_libraries(iBMSC PRIVATE
    ibmsc_model
    ibmsc_io
    ibmsc_audio
    ibmsc_editor
    ibmsc_app
    ibmsc_canvas
    Qt6::Core
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Multimedia
    Qt6::Qml
)

set_target_properties(iBMSC PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_NAME "iBMSC"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    WIN32_EXECUTABLE TRUE
)

# ── Unit tests ────────────────────────────────────────────────────────────────
enable_testing()
add_executable(ibmsc_tests
    tests/test_main.cpp
    tests/test_base36.cpp
    tests/test_timing_map.cpp
    tests/test_channel_type.cpp
    tests/test_bms_parser.cpp
    tests/test_editor_document.cpp
)
target_link_libraries(ibmsc_tests PRIVATE ibmsc_model ibmsc_io ibmsc_editor Qt6::Core Qt6::Gui Qt6::Qml)
add_test(NAME ibmsc_tests COMMAND ibmsc_tests)
