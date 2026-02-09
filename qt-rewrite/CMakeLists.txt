cmake_minimum_required(VERSION 3.20)
project(iBMSCQtRewrite VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia Xml)

add_executable(iBMSCQt
    src/main.cpp
    resources/ibmsc.qrc
    src/core/bms_document.h
    src/core/bms_document.cpp
    src/core/undo/i_undo_command.h
    src/core/undo/commands.h
    src/core/undo/commands.cpp
    src/core/settings/persistent_profile.h
    src/core/settings/persistent_profile.cpp
    src/io/bms_parser.h
    src/io/bms_parser.cpp
    src/theme/theme_loader.h
    src/theme/theme_loader.cpp
    src/audio/audio_preview_service.h
    src/audio/audio_preview_service.cpp
    src/ui/chart_editor_widget.h
    src/ui/chart_editor_widget.cpp
    src/ui/resource_catalog.h
    src/ui/resource_catalog.cpp
    src/ui/po_panel_manager.h
    src/ui/po_panel_manager.cpp
    src/ui/dialogs/diag_find_dialog.h
    src/ui/dialogs/diag_find_dialog.cpp
    src/ui/dialogs/statistics_dialog.h
    src/ui/dialogs/statistics_dialog.cpp
    src/ui/dialogs/myo2_dialog.h
    src/ui/dialogs/myo2_dialog.cpp
    src/ui/dialogs/color_picker_dialog.h
    src/ui/dialogs/color_picker_dialog.cpp
    src/ui/dialogs/import_sm_dialog.h
    src/ui/dialogs/import_sm_dialog.cpp
    src/ui/dialogs/compare_method_dialog.h
    src/ui/dialogs/compare_method_dialog.cpp
    src/ui/dialogs/load_progress_dialog.h
    src/ui/dialogs/load_progress_dialog.cpp
    src/ui/dialogs/about_dialog.h
    src/ui/dialogs/about_dialog.cpp
    src/ui/dialogs/splash_dialog.h
    src/ui/dialogs/splash_dialog.cpp
    src/ui/dialogs/op_general_dialog.h
    src/ui/dialogs/op_general_dialog.cpp
    src/ui/dialogs/op_visual_dialog.h
    src/ui/dialogs/op_visual_dialog.cpp
    src/ui/dialogs/op_player_dialog.h
    src/ui/dialogs/op_player_dialog.cpp
    src/ui/main_window.h
    src/ui/main_window.cpp
)

target_link_libraries(iBMSCQt PRIVATE Qt6::Widgets Qt6::Multimedia Qt6::Xml)
target_include_directories(iBMSCQt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
