import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

/// Ableton-style unified top control bar replacing MenuBar + ToolBar.
Rectangle {
    id: root
    height: 40
    color: "#1a1a1a"
    property int editMode: 1
    property bool showWaveform: true

    // Signals to parent
    signal sidebarToggled()
    signal editModeRequested(int mode)
    signal waveformToggled(bool enabled)

    // ── Bottom border ─────────────────────────────────────────────────────────
    Rectangle {
        anchors.bottom: parent.bottom
        width: parent.width
        height: 1
        color: "#333333"
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 8
        anchors.rightMargin: 8
        spacing: 0

        // ── App zone ──────────────────────────────────────────────────────────
        Text {
            text: "iBMSC"
            color: "#ff764d"
            font.pixelSize: 14
            font.weight: Font.Bold
            font.letterSpacing: 1
            Layout.rightMargin: 12
        }

        FlatButton { icon: "\u2795"; tooltip: "New";  onClicked: appController.newFile() }       // +
        FlatButton { icon: "\uD83D\uDCC2"; tooltip: "Open"; onClicked: appController.requestOpenDialog() }
        FlatButton { icon: "\uD83D\uDCBE"; tooltip: "Save"; enabled: editorDoc.isDirty; onClicked: appController.saveFile() }

        BarSeparator {}

        // ── Transport zone ────────────────────────────────────────────────────
        Rectangle {
            width: 32; height: 28
            radius: 2
            color: audioEngine.isPlaying ? "#ff764d" : (transportMouse.containsMouse ? "#3d3d3d" : "#2a2a2a")
            Layout.alignment: Qt.AlignVCenter

            Behavior on color { ColorAnimation { duration: 80 } }

            Text {
                anchors.centerIn: parent
                text: audioEngine.isPlaying ? "\u25A0" : "\u25B6"
                color: audioEngine.isPlaying ? "#0a0a0a" : "#ffffff"
                font.pixelSize: 13
            }

            MouseArea {
                id: transportMouse
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: audioEngine.isPlaying
                    ? appController.stopPlayback()
                    : appController.playFromPosition(canvas.scrollBeat / 4.0)
            }
        }

        // BPM display
        Text {
            text: {
                var bpm = editorDoc.headerValue("BPM")
                return bpm !== "" ? bpm + " BPM" : "--- BPM"
            }
            color: "#999999"
            font.pixelSize: 11
            font.family: Qt.platform.os === "osx" ? "Menlo" : "Consolas"
            Layout.leftMargin: 8
        }

        BarSeparator {}

        // ── Grid zone ─────────────────────────────────────────────────────────
        Text {
            text: "GRID"
            color: "#555555"
            font.pixelSize: 9
            font.letterSpacing: 1.0
            Layout.rightMargin: 4
        }

        FlatComboBox {
            id: gridCombo
            model: ["1/4","1/8","1/16","1/32","1/48","1/192"]
            currentIndex: 2
            implicitWidth: 80

            onCurrentIndexChanged: {
                var vals = [4,8,16,32,48,192]
                canvas.gridSubdivision = vals[currentIndex]
            }
        }

        FlatButton { icon: "+"; tooltip: "Zoom In";  onClicked: canvas.setPxPerBeat(canvas.pxPerBeat * 1.2) }
        FlatButton { icon: "\u2212"; tooltip: "Zoom Out"; onClicked: canvas.setPxPerBeat(canvas.pxPerBeat / 1.2) }

        BarSeparator {}

        // ── Tool mode zone ───────────────────────────────────────────────────
        Text {
            text: "TOOL"
            color: "#555555"
            font.pixelSize: 9
            font.letterSpacing: 1.0
            Layout.rightMargin: 4
        }

        FlatButton {
            icon: "T"
            tooltip: "Time Selection (F1)"
            active: root.editMode === 3
            onClicked: {
                root.editMode = 3
                root.editModeRequested(3)
            }
        }
        FlatButton {
            icon: "S"
            tooltip: "Select Mode (F2)"
            active: root.editMode === 0
            onClicked: {
                root.editMode = 0
                root.editModeRequested(0)
            }
        }
        FlatButton {
            icon: "W"
            tooltip: "Write Mode"
            active: root.editMode === 1
            onClicked: {
                root.editMode = 1
                root.editModeRequested(1)
            }
        }
        FlatButton {
            icon: "E"
            tooltip: "Eraser Mode"
            active: root.editMode === 2
            onClicked: {
                root.editMode = 2
                root.editModeRequested(2)
            }
        }

        BarSeparator {}

        // ── Utility zone ──────────────────────────────────────────────────────
        FlatButton { icon: "\u21A9"; tooltip: "Undo"; enabled: editorDoc.canUndo; onClicked: editorDoc.undo() }
        FlatButton { icon: "\u21AA"; tooltip: "Redo"; enabled: editorDoc.canRedo; onClicked: editorDoc.redo() }

        FlatButton {
            icon: "I"
            tooltip: "Insert Measure"
            onClicked: {
                var m = Math.max(0, Math.floor(canvas.scrollBeat / 4.0))
                canvas.timeSelectionStartBeat = m * 4.0
                canvas.timeSelectionEndBeat = (m + 1) * 4.0
                editorDoc.insertEmptyMeasure(m, 1)
            }
        }
        FlatButton {
            icon: "D"
            tooltip: "Delete Measure"
            onClicked: {
                var m = Math.max(0, Math.floor(canvas.scrollBeat / 4.0))
                canvas.timeSelectionStartBeat = m * 4.0
                canvas.timeSelectionEndBeat = (m + 1) * 4.0
                editorDoc.deleteMeasure(m, 1)
            }
        }
        FlatButton {
            icon: "CM"
            tooltip: "Copy Measure"
            onClicked: {
                var m = Math.max(0, Math.floor(canvas.scrollBeat / 4.0))
                canvas.timeSelectionStartBeat = m * 4.0
                canvas.timeSelectionEndBeat = (m + 1) * 4.0
                editorDoc.copyMeasure(m, 1)
            }
        }
        FlatButton {
            icon: "PM"
            tooltip: "Paste Measure"
            onClicked: {
                var m = Math.max(0, Math.floor(canvas.scrollBeat / 4.0))
                canvas.timeSelectionStartBeat = m * 4.0
                canvas.timeSelectionEndBeat = (m + 1) * 4.0
                editorDoc.pasteMeasure(m)
            }
        }

        FlatButton {
            icon: "WF"
            tooltip: "Toggle Waveform"
            active: root.showWaveform
            onClicked: {
                root.showWaveform = !root.showWaveform
                root.waveformToggled(root.showWaveform)
            }
        }

        BarSeparator {}

        // ── Mode toggles ─────────────────────────────────────────────────────
        FlatButton {
            icon: "NT"
            tooltip: "LN Input: NT Mode (F8)"
            active: appSettings.ntInputMode
            onClicked: appSettings.ntInputMode = !appSettings.ntInputMode
        }
        FlatButton {
            icon: "Er"
            tooltip: "Error Check"
            active: appSettings.errorCheck
            onClicked: appSettings.errorCheck = !appSettings.errorCheck
        }
        FlatButton {
            icon: "Pv"
            tooltip: "Preview on Click"
            active: appSettings.previewOnClick
            onClicked: appSettings.previewOnClick = !appSettings.previewOnClick
        }
        FlatButton {
            icon: "\u25B6\u25B6"
            tooltip: "Launch SDL Preview (from current measure)"
            enabled: appController.hasDocument
            onClicked: appController.launchPreview(Math.max(0, Math.floor(canvas.scrollBeat / 4.0)))
        }
        FlatButton {
            icon: "Fn"
            tooltip: "Show Filename"
            active: appSettings.showFilename
            onClicked: appSettings.showFilename = !appSettings.showFilename
        }
        FlatButton {
            icon: "W+"
            tooltip: "WAV Auto-Increase"
            active: appSettings.wavAutoIncrease
            onClicked: appSettings.wavAutoIncrease = !appSettings.wavAutoIncrease
        }

        BarSeparator {}

        Item { Layout.fillWidth: true }

        FlatButton { icon: "\u2630"; tooltip: "Sidebar"; onClicked: root.sidebarToggled() }

        FlatButton {
            icon: "\u2261"
            tooltip: "Menu"
            onClicked: overflowMenu.open()

            OverflowMenu {
                id: overflowMenu
                x: parent.width - width
                y: parent.height + 4
            }
        }
    }

    // ── Reusable inline components ────────────────────────────────────────────

    component FlatButton: Rectangle {
        property string icon
        property string tooltip: ""
        property bool enabled: true
        property bool active: false
        signal clicked()

        width: 28; height: 28
        radius: 2
        color: active ? "#ff764d" : (flatMouse.containsMouse && enabled ? "#3d3d3d" : "transparent")
        opacity: enabled ? 1.0 : 0.3
        Layout.alignment: Qt.AlignVCenter

        Behavior on color { ColorAnimation { duration: 60 } }

        Text {
            anchors.centerIn: parent
            text: icon
            color: active ? "#0a0a0a" : "#ffffff"
            font.pixelSize: 13
        }

        MouseArea {
            id: flatMouse
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
            onClicked: if (parent.enabled) parent.clicked()
        }

        ToolTip.visible: flatMouse.containsMouse && tooltip !== ""
        ToolTip.text: tooltip
        ToolTip.delay: 600
    }

    component BarSeparator: Rectangle {
        width: 1
        height: 20
        color: "#333333"
        Layout.alignment: Qt.AlignVCenter
        Layout.leftMargin: 8
        Layout.rightMargin: 8
    }
}
