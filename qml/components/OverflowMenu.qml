import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

/// Flat popup menu replacing the traditional MenuBar.
/// Contains all application actions organized in sections.
Popup {
    id: root
    width: 260
    height: Math.min(menuContent.implicitHeight + 8, 600)
    padding: 4
    clip: true

    background: Rectangle {
        color: "#1a1a1a"
        border.color: "#333333"
        border.width: 1
    }

    ScrollView {
        anchors.fill: parent
        contentWidth: availableWidth
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

    ColumnLayout {
        id: menuContent
        width: parent.width
        spacing: 0

        // ── File ──────────────────────────────────────────────────────────────
        MenuLabel { text: "FILE" }
        MenuAction { label: "New";         shortcut: "Ctrl+N"; onClicked: { root.close(); appController.newFile() } }
        MenuAction { label: "Open...";     shortcut: "Ctrl+O"; onClicked: { root.close(); appController.requestOpenDialog() } }
        MenuAction { label: "Save";        shortcut: "Ctrl+S"; onClicked: { root.close(); appController.saveFile() } }
        MenuAction { label: "Save As...";  shortcut: "Ctrl+Shift+S"; onClicked: { root.close(); appController.requestSaveAsDialog() } }
        MenuDivider {}

        // ── Edit ──────────────────────────────────────────────────────────────
        MenuLabel { text: "EDIT" }
        MenuAction { label: "Undo";        shortcut: "Ctrl+Z"; enabled: editorDoc.canUndo; onClicked: { root.close(); editorDoc.undo() } }
        MenuAction { label: "Redo";        shortcut: "Ctrl+Y"; enabled: editorDoc.canRedo; onClicked: { root.close(); editorDoc.redo() } }
        MenuAction { label: "Copy";        shortcut: "Ctrl+C"; onClicked: { root.close(); editorDoc.copySelection() } }
        MenuAction { label: "Cut";         shortcut: "Ctrl+X"; onClicked: { root.close(); editorDoc.cutSelection() } }
        MenuAction { label: "Paste";       shortcut: "Ctrl+V"; onClicked: { root.close(); editorDoc.pasteFromClipboard(canvas.scrollBeat) } }
        MenuAction { label: "Select All";  shortcut: "Ctrl+A"; onClicked: root.close() }
        MenuDivider {}

        // ── View ──────────────────────────────────────────────────────────────
        MenuLabel { text: "VIEW" }
        MenuAction { label: "Zoom In";     shortcut: "Ctrl+="; onClicked: { root.close(); canvas.setPxPerBeat(canvas.pxPerBeat * 1.2) } }
        MenuAction { label: "Zoom Out";    shortcut: "Ctrl+-"; onClicked: { root.close(); canvas.setPxPerBeat(canvas.pxPerBeat / 1.2) } }
        MenuDivider {}

        // ── Chart ─────────────────────────────────────────────────────────────
        MenuLabel { text: "CHART" }
        MenuAction { label: "Jump to Measure..."; shortcut: "Ctrl+G"; onClicked: { root.close(); jumpDialog.open() } }
        MenuAction { label: "Find / Replace...";  shortcut: "Ctrl+H"; onClicked: { root.close(); findReplaceDialog.open() } }
        MenuAction { label: "Column Config...";   onClicked: { root.close(); columnConfigDialog.open() } }
        MenuAction { label: "BPM Tools...";       onClicked: { root.close(); bpmToolsDialog.open() } }
        MenuDivider {}

        // ── Convert ──────────────────────────────────────────────────────────
        MenuLabel { text: "CONVERT" }
        MenuAction { label: "To Long Note";       shortcut: "L"; onClicked: { root.close(); editorDoc.convertToLong() } }
        MenuAction { label: "To Short Note";      shortcut: "S"; onClicked: { root.close(); editorDoc.convertToShort() } }
        MenuAction { label: "Toggle Long/Short";  onClicked: { root.close(); editorDoc.toggleLongShort() } }
        MenuAction { label: "To Hidden";          onClicked: { root.close(); editorDoc.convertToHidden() } }
        MenuAction { label: "To Visible";         onClicked: { root.close(); editorDoc.convertToVisible() } }
        MenuAction { label: "Toggle Hidden";      shortcut: "H"; onClicked: { root.close(); editorDoc.toggleHidden() } }
        MenuAction { label: "Mirror";             onClicked: { root.close(); editorDoc.mirrorNotes() } }
        MenuAction { label: "Storm (Shuffle)";    onClicked: { root.close(); editorDoc.stormNotes() } }
        MenuDivider {}

        MenuAction { label: "Statistics...";      shortcut: "Ctrl+T"; onClicked: { root.close(); statisticsDialog.open() } }
        MenuDivider {}
        MenuDivider {}

        // ── Theme ────────────────────────────────────────────────────────────
        MenuLabel { text: "THEME" }
        MenuAction { label: "Reset to Default"; onClicked: { root.close(); themeManager.resetToDefault() } }
        MenuDivider {}

        MenuAction { label: "Settings..."; onClicked: { root.close(); settingsDialog.open() } }
        MenuAction { label: "Quit";        shortcut: "Ctrl+Q"; onClicked: Qt.quit() }
    }
    } // ScrollView

    // ── Reusable inline components ────────────────────────────────────────────

    component MenuLabel: Item {
        property string text
        Layout.fillWidth: true
        height: 22
        Text {
            text: parent.text
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: 9
            font.letterSpacing: 1.5
            font.weight: Font.Medium
            color: "#666666"
        }
    }

    component MenuAction: Rectangle {
        property string label
        property string shortcut: ""
        property bool enabled: true
        signal clicked()

        Layout.fillWidth: true
        height: 28
        color: menuMouse.containsMouse && enabled ? "#333333" : "transparent"
        opacity: enabled ? 1.0 : 0.4

        Behavior on color { ColorAnimation { duration: 60 } }

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            spacing: 8

            Text {
                text: label
                color: "#ffffff"
                font.pixelSize: 12
                Layout.fillWidth: true
            }
            Text {
                text: shortcut
                color: "#555555"
                font.pixelSize: 10
                font.family: Qt.platform.os === "osx" ? "Menlo" : "Consolas"
                visible: shortcut !== ""
            }
        }

        MouseArea {
            id: menuMouse
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
            onClicked: if (parent.enabled) parent.clicked()
        }
    }

    component MenuDivider: Rectangle {
        Layout.fillWidth: true
        height: 1
        color: "#222222"
        Layout.topMargin: 2
        Layout.bottomMargin: 2
    }
}
