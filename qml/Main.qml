import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs
import iBMSC.Canvas 1.0
import "components/"
import "panels/"

ApplicationWindow {
    id: root
    visible: true
    width: 1400
    height: 900
    minimumWidth: 900
    minimumHeight: 600
    color: "#0a0a0a"

    Material.theme: Material.Dark
    Material.accent: "#ff764d"

    title: {
        var base = "iBMSC"
        if (editorDoc.filePath !== "") base = editorDoc.filePath.split("/").pop() + " — " + base
        if (editorDoc.isDirty) base = "\u2022 " + base
        return base
    }

    // ── File dialogs ──────────────────────────────────────────────────────────
    FileDialog {
        id: openFileDialog
        title: "Open BMS File"
        nameFilters: ["BMS Files (*.bms *.bme *.bml *.pms)", "All Files (*)"]
        onAccepted: appController.openFile(selectedFile.toString().replace("file://", ""))
    }

    FileDialog {
        id: saveAsDialog
        title: "Save BMS File"
        fileMode: FileDialog.SaveFile
        nameFilters: ["BMS Files (*.bms)", "All Files (*)"]
        onAccepted: appController.saveFileTo(selectedFile.toString().replace("file://", ""))
    }

    Connections {
        target: appController
        function onOpenDialogRequested() { openFileDialog.open() }
        function onSaveAsDialogRequested(currentPath) {
            if (currentPath !== "") saveAsDialog.currentFile = "file://" + currentPath
            saveAsDialog.open()
        }
    }

    // ── Drop area ─────────────────────────────────────────────────────────────
    DropArea {
        anchors.fill: parent
        keys: ["text/uri-list"]
        onDropped: {
            if (drop.hasUrls) {
                var path = drop.urls[0].toString().replace("file://", "")
                appController.openFile(path)
            }
        }
    }

    property int writeValue: 1

    // ── Close confirmation ────────────────────────────────────────────────────
    property bool forceClose: false

    onClosing: function(close) {
        if (!forceClose && editorDoc.isDirty) {
            close.accepted = false
            closeConfirmDialog.open()
        }
    }

    Dialog {
        id: closeConfirmDialog
        title: "Unsaved Changes"
        modal: true
        x: (root.width - width) / 2
        y: (root.height - height) / 2

        background: Rectangle { color: "#1a1a1a"; border.color: "#333333"; border.width: 1 }

        Label { text: "Save changes before closing?"; color: "#ffffff" }

        footer: Rectangle {
            color: "#1a1a1a"
            height: 40
            RowLayout {
                anchors.fill: parent
                anchors.rightMargin: 12
                anchors.leftMargin: 12
                spacing: 8
                Item { Layout.fillWidth: true }

                Rectangle {
                    Layout.preferredWidth: 60; Layout.preferredHeight: 24
                    color: cancelMouse.containsMouse ? "#3d3d3d" : "#2a2a2a"
                    border.color: "#333333"; border.width: 1; radius: 1
                    Text { anchors.centerIn: parent; text: "Cancel"; color: "#ffffff"; font.pixelSize: 11 }
                    MouseArea { id: cancelMouse; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: closeConfirmDialog.reject() }
                }

                Rectangle {
                    Layout.preferredWidth: 60; Layout.preferredHeight: 24
                    color: discardMouse.containsMouse ? "#3d3d3d" : "#2a2a2a"
                    border.color: "#333333"; border.width: 1; radius: 1
                    Text { anchors.centerIn: parent; text: "Discard"; color: "#ffffff"; font.pixelSize: 11 }
                    MouseArea { id: discardMouse; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: { root.forceClose = true; closeConfirmDialog.reject(); Qt.quit() } }
                }

                Rectangle {
                    Layout.preferredWidth: 60; Layout.preferredHeight: 24
                    color: saveMouse.containsMouse ? "#ff8f6d" : "#ff764d"
                    radius: 1
                    Text { anchors.centerIn: parent; text: "Save"; color: "#0a0a0a"; font.pixelSize: 11; font.weight: Font.Medium }
                    MouseArea { id: saveMouse; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: { appController.saveFile(); root.forceClose = true; closeConfirmDialog.accept(); Qt.quit() } }
                }
            }
        }
    }

    // ── Utility dialogs ───────────────────────────────────────────────────────
    SettingsDialog { id: settingsDialog }
    JumpToMeasureDialog {
        id: jumpDialog
        onJumpRequested: function(beat) { canvas.scrollBeat = beat }
    }
    FindReplaceDialog { id: findReplaceDialog }
    ColumnConfigDialog { id: columnConfigDialog; appSettings: appSettings }
    BpmToolsDialog { id: bpmToolsDialog }
    StatisticsDialog { id: statisticsDialog }
    EditValueDialog { id: editValueDialog }

    // ── Keyboard shortcuts (previously in MenuBar) ────────────────────────────
    Shortcut { sequence: "Ctrl+N";       onActivated: appController.newFile() }
    Shortcut { sequence: "Ctrl+O";       onActivated: appController.requestOpenDialog() }
    Shortcut { sequence: "Ctrl+S";       onActivated: appController.saveFile() }
    Shortcut { sequence: "Ctrl+Shift+S"; onActivated: appController.requestSaveAsDialog() }
    Shortcut { sequence: "Ctrl+Q";       onActivated: Qt.quit() }
    Shortcut { sequence: "Ctrl+Z";       onActivated: { if (editorDoc.canUndo) editorDoc.undo() } }
    Shortcut { sequence: "Ctrl+Y";       onActivated: { if (editorDoc.canRedo) editorDoc.redo() } }
    Shortcut { sequence: "Ctrl+C";       onActivated: editorDoc.copySelection() }
    Shortcut { sequence: "Ctrl+X";       onActivated: editorDoc.cutSelection() }
    Shortcut { sequence: "Ctrl+V";       onActivated: editorDoc.pasteFromClipboard(canvas.scrollBeat) }
    Shortcut { sequence: "Ctrl+=";       onActivated: canvas.setPxPerBeat(canvas.pxPerBeat * 1.2) }
    Shortcut { sequence: "Ctrl+-";       onActivated: canvas.setPxPerBeat(canvas.pxPerBeat / 1.2) }
    Shortcut { sequence: "Ctrl+G";       onActivated: jumpDialog.open() }
    Shortcut { sequence: "Ctrl+H";       onActivated: findReplaceDialog.open() }
    Shortcut { sequence: "Ctrl+T";       onActivated: statisticsDialog.open() }
    Shortcut { sequence: "F8";           onActivated: appSettings.ntInputMode = !appSettings.ntInputMode }
    Shortcut { sequence: "F1";           onActivated: canvas.editMode = 3 }
    Shortcut { sequence: "F2";           onActivated: canvas.editMode = 0 }
    Shortcut { sequence: "F3";           onActivated: canvas.editMode = 1 }
    Shortcut { sequence: "F4";           onActivated: canvas.editMode = 2 }
    Shortcut { sequence: "F5";           onActivated: appController.playFromPosition(0.0) }
    Shortcut { sequence: "F6";           onActivated: appController.playFromPosition(canvas.scrollBeat / 4.0) }
    Shortcut { sequence: "F7";           onActivated: appController.stopPlayback() }
    Shortcut { sequence: "Ctrl+[";       onActivated: sidebarVisible = !sidebarVisible }
    Shortcut { sequence: "Ctrl+]";       onActivated: sidebarVisible = !sidebarVisible }
    Shortcut {
        sequence: "Space"
        onActivated: audioEngine.isPlaying
            ? appController.stopPlayback()
            : appController.playFromPosition(canvas.scrollBeat / 4.0)
    }

    // ── Sidebar state ─────────────────────────────────────────────────────────
    property bool sidebarVisible: true

    // ── Main layout ───────────────────────────────────────────────────────────
    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // ── Control bar ───────────────────────────────────────────────────────
        ControlBar {
            Layout.fillWidth: true
            editMode: canvas.editMode
            showWaveform: canvas.showWaveform
            onEditModeChanged: canvas.editMode = editMode
            onShowWaveformChanged: canvas.showWaveform = enabled
            onSidebarToggled: root.sidebarVisible = !root.sidebarVisible
        }

        // ── Content area ──────────────────────────────────────────────────────
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            // Central area: welcome or chart canvas
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                WelcomeScreen {
                    anchors.fill: parent
                    visible: !appController.hasDocument
                }

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 0
                    visible: appController.hasDocument

                    CanvasHeader { Layout.fillWidth: true }

                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true

                        ChartCanvas {
                            id: canvas
                            anchors.fill: parent
                            document: editorDoc
                            audioEngine: audioEngine

                            onNotePlaced: function(ch, measure, beatNum, beatDen, value) {
                                editorDoc.placeNote(ch, measure, beatNum, beatDen, root.writeValue)
                                if (appSettings.wavAutoIncrease) root.writeValue = Math.min(1295, root.writeValue + 1)
                            }
                            onNotePlacedWithDuration: function(ch, measure, beatNum, beatDen, value, durationInBeats) {
                                editorDoc.placeNoteWithDuration(ch, measure, beatNum, beatDen, root.writeValue, durationInBeats)
                                if (appSettings.wavAutoIncrease) root.writeValue = Math.min(1295, root.writeValue + 1)
                            }
                            onNoteDeleted: function(ch, measure, beatNum, beatDen) {
                                editorDoc.deleteNote(ch, measure, beatNum, beatDen)
                            }
                            onNoteDoubleClicked: function(ch, measure, beatNum, beatDen, currentValue) {
                                editValueDialog.noteChannel = ch
                                editValueDialog.noteMeasure = measure
                                editValueDialog.noteBeatNum = beatNum
                                editValueDialog.noteBeatDen = beatDen
                                editValueDialog.currentVal = currentValue
                                editValueDialog.open()
                            }
                        }

                        DropArea {
                            anchors.fill: parent
                            keys: ["text/uri-list"]
                            enabled: appController.hasDocument
                            onDropped: function(drop) {
                                if (!drop.hasUrls || drop.urls.length === 0)
                                    return
                                var p = drop.urls[0].toString().replace("file://", "")
                                var lower = p.toLowerCase()
                                if (!(lower.endsWith(".wav") || lower.endsWith(".ogg") || lower.endsWith(".mp3")))
                                    return
                                var ch = canvas.channelAt(drop.x)
                                if (ch < 0)
                                    return
                                appController.importAudioToBgmLane(p, ch, canvas.beatAt(drop.y))
                            }
                        }

                        BpmOverlay {
                            anchors.fill: parent
                            scrollBeat:   canvas.scrollBeat
                            pxPerBeat:    canvas.pxPerBeat
                            canvasHeight: canvas.height
                        }
                    }
                }

                Connections {
                    target: audioEngine
                    function onPositionChanged(sec) {
                        var bpm = parseFloat(editorDoc.headerValue("BPM")) || 130
                        canvas.playbackBeat = sec * bpm / 60.0 * 4.0
                    }
                }
            }

            // ── Sidebar (collapsible sections) ────────────────────────────────
            Rectangle {
                id: sidebar
                Layout.fillHeight: true
                Layout.preferredWidth: root.sidebarVisible ? 280 : 0
                color: "#1a1a1a"
                clip: true

                Behavior on Layout.preferredWidth {
                    NumberAnimation { duration: 150; easing.type: Easing.InOutQuad }
                }

                // Left border
                Rectangle {
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    width: 1
                    color: "#333333"
                }

                ScrollView {
                    anchors.fill: parent
                    anchors.leftMargin: 1
                    contentWidth: availableWidth
                    clip: true

                    ColumnLayout {
                        width: parent.width
                        spacing: 0

                        SidebarSection {
                            title: "Header"
                            Layout.fillWidth: true
                            HeaderPanel { width: parent.width; implicitHeight: 500 }
                        }

                        SidebarSection {
                            title: "WAV"
                            expanded: false
                            Layout.fillWidth: true
                            WavResourcePanel { width: parent.width; implicitHeight: 400 }
                        }

                        SidebarSection {
                            title: "Selected Note"
                            expanded: true
                            Layout.fillWidth: true
                            SelectedNotePanel { width: parent.width; implicitHeight: 260 }
                        }

                        SidebarSection {
                            title: "Waveform"
                            expanded: false
                            Layout.fillWidth: true
                            WaveformInspector { width: parent.width; implicitHeight: 200 }
                        }

                        SidebarSection {
                            title: "BMP"
                            expanded: false
                            Layout.fillWidth: true
                            BmpResourcePanel { width: parent.width; implicitHeight: 400 }
                        }

                        SidebarSection {
                            title: "Measures"
                            expanded: false
                            Layout.fillWidth: true
                            MeasureLengthPanel { width: parent.width; implicitHeight: 300 }
                        }

                        SidebarSection {
                            title: "Expansion Code"
                            expanded: false
                            Layout.fillWidth: true
                            TextArea {
                                width: parent.width
                                implicitHeight: 200
                                font.pixelSize: 11
                                font.family: "Menlo"
                                color: "#ddd"
                                placeholderText: "Custom BMS expansion code..."
                                wrapMode: TextEdit.Wrap
                                background: Rectangle { color: "#1a1a1a"; border.color: "#333"; border.width: 1 }
                            }
                        }
                    }
                }
            }
        }

        // ── Info strip (thin status bar) ──────────────────────────────────────
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 22
            color: "#1a1a1a"

            Rectangle {
                anchors.top: parent.top
                width: parent.width
                height: 1
                color: "#222222"
            }

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 8
                anchors.rightMargin: 8

                Text {
                    text: audioEngine.isPlaying
                        ? "Playing \u2014 " + audioEngine.playbackPosition.toFixed(2) + "s"
                        : (editorDoc.isDirty ? "Modified" : "Ready")
                    color: audioEngine.isPlaying ? "#ff764d" : "#555555"
                    font.pixelSize: 10
                }

                Item { Layout.fillWidth: true }

                Text {
                    text: editorDoc.filePath !== "" ? editorDoc.filePath.split("/").pop() : ""
                    color: "#555555"
                    font.pixelSize: 10
                    visible: editorDoc.filePath !== ""
                }
            }
        }
    }

    // ── Splash screen overlay ────────────────────────────────────────────────
    SplashOverlay { id: splashOverlay }
}
