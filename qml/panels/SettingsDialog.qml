import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../components"

Dialog {
    id: root
    title: "Settings"
    width: 520
    height: 420
    modal: true
    x: parent ? (parent.width - width) / 2 : 0
    y: parent ? (parent.height - height) / 2 : 0

    background: Rectangle { color: "#1a1a1a"; border.color: "#333333"; border.width: 1 }

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
                MouseArea { id: cancelMouse; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: root.reject() }
            }
            
            Rectangle {
                Layout.preferredWidth: 60; Layout.preferredHeight: 24
                color: okMouse.containsMouse ? "#ff8f6d" : "#ff764d"
                radius: 1
                Text { anchors.centerIn: parent; text: "OK"; color: "#0a0a0a"; font.pixelSize: 11; font.weight: Font.Medium }
                MouseArea { id: okMouse; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: {
                    appSettings.gridSubdivision = gridSubCombo.currentValue
                    appSettings.audioDevice = audioDeviceCombo.currentText
                    root.accept()
                } }
            }
        }
    }

    TabBar {
        id: tabBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        background: Rectangle { color: "#0a0a0a" }

        TabButton {
            text: "General"
            background: Rectangle { color: parent.checked ? "#2a2a2a" : "#0a0a0a" }
            contentItem: Text { text: parent.text; color: parent.checked ? "#ffffff" : "#555555"; font.pixelSize: 11; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
        }
        TabButton {
            text: "Editor"
            background: Rectangle { color: parent.checked ? "#2a2a2a" : "#0a0a0a" }
            contentItem: Text { text: parent.text; color: parent.checked ? "#ffffff" : "#555555"; font.pixelSize: 11; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
        }
        TabButton {
            text: "Audio"
            background: Rectangle { color: parent.checked ? "#2a2a2a" : "#0a0a0a" }
            contentItem: Text { text: parent.text; color: parent.checked ? "#ffffff" : "#555555"; font.pixelSize: 11; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
        }
        TabButton {
            text: "Key Bindings"
            background: Rectangle { color: parent.checked ? "#2a2a2a" : "#0a0a0a" }
            contentItem: Text { text: parent.text; color: parent.checked ? "#ffffff" : "#555555"; font.pixelSize: 11; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
        }
    }

    StackLayout {
        anchors.top: tabBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: 8
        height: parent.height - tabBar.height - 16
        currentIndex: tabBar.currentIndex

        ColumnLayout {
            spacing: 12
            Label { text: "General settings"; color: "#999999"; font.pixelSize: 12 }
        }

        ColumnLayout {
            spacing: 12

            RowLayout {
                Label { text: "Default grid:"; Layout.preferredWidth: 120; color: "#999999"; font.pixelSize: 12 }
                FlatComboBox {
                    id: gridSubCombo
                    model: [
                        { text: "1/4",   value: 4   },
                        { text: "1/8",   value: 8   },
                        { text: "1/16",  value: 16  },
                        { text: "1/32",  value: 32  },
                        { text: "1/48",  value: 48  },
                        { text: "1/192", value: 192 }
                    ]
                    textRole: "text"
                    valueRole: "value"

                    Component.onCompleted: {
                        for (var i = 0; i < model.length; ++i) {
                            if (model[i].value === appSettings.gridSubdivision) {
                                currentIndex = i; break
                            }
                        }
                    }
                }
            }
        }

        ColumnLayout {
            spacing: 12

            RowLayout {
                Label { text: "Output device:"; Layout.preferredWidth: 120; color: "#999999"; font.pixelSize: 12 }
                FlatComboBox {
                    id: audioDeviceCombo
                    model: audioEngine.deviceNames

                    Component.onCompleted: {
                        var saved = appSettings.audioDevice
                        for (var i = 0; i < model.length; ++i) {
                            if (model[i] === saved) { currentIndex = i; break }
                        }
                    }
                }
            }
        }

        ScrollView {
            ListView {
                model: [
                    { action: "New File",      key: "Ctrl+N" },
                    { action: "Open File",     key: "Ctrl+O" },
                    { action: "Save",          key: "Ctrl+S" },
                    { action: "Save As",       key: "Ctrl+Shift+S" },
                    { action: "Undo",          key: "Ctrl+Z" },
                    { action: "Redo",          key: "Ctrl+Y" },
                    { action: "Play/Stop",     key: "Space"  },
                    { action: "Zoom In",       key: "Ctrl++" },
                    { action: "Zoom Out",      key: "Ctrl+-" },
                ]

                delegate: RowLayout {
                    width: ListView.view.width
                    height: 28
                    Label { text: modelData.action; Layout.fillWidth: true; font.pixelSize: 11; color: "#ffffff" }
                    Label { text: modelData.key; color: "#555555"; font.pixelSize: 11; font.family: Qt.platform.os === "osx" ? "Menlo" : "Consolas" }
                }
            }
        }
    }
}
