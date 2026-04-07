import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

/// Modal dialog for configuring column visibility and width.
Dialog {
    id: root
    title: "Column Configuration"
    modal: true
    width: 480
    height: 500
    x: parent ? (parent.width - width) / 2 : 0
    y: parent ? (parent.height - height) / 2 : 0

    background: Rectangle { color: "#1a1a1a"; border.color: "#333333"; border.width: 1 }

    property var appSettings: null

    onOpened: {
        listModel.clear()
        if (!appSettings) return
        var configs = appSettings.columnConfigs
        for (var i = 0; i < configs.length; i++) {
            listModel.append({
                colIndex: i,
                colEnabled: configs[i].enabled,
                colWidth: configs[i].width,
                colTitle: configs[i].title,
                colChannel: configs[i].bmsChannel,
                colIsNote: configs[i].isNoteColumn
            })
        }
    }

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
                Text { anchors.centerIn: parent; text: "Apply"; color: "#0a0a0a"; font.pixelSize: 11; font.weight: Font.Medium }
                MouseArea { id: okMouse; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: {
                    if (!appSettings) return
                    for (var i = 0; i < listModel.count; i++) {
                        var item = listModel.get(i)
                        appSettings.setColumnEnabled(item.colIndex, item.colEnabled)
                        appSettings.setColumnWidth(item.colIndex, item.colWidth)
                    }
                    root.accept()
                } }
            }
        }
    }

    ListModel { id: listModel }

    ColumnLayout {
        anchors.fill: parent
        spacing: 8

        // Header row
        RowLayout {
            Layout.fillWidth: true
            spacing: 4
            Label { text: "On"; font.pixelSize: 10; color: "#777"; Layout.preferredWidth: 30; horizontalAlignment: Text.AlignHCenter }
            Label { text: "Title"; font.pixelSize: 10; color: "#777"; Layout.preferredWidth: 80 }
            Label { text: "Channel"; font.pixelSize: 10; color: "#777"; Layout.preferredWidth: 60 }
            Label { text: "Width"; font.pixelSize: 10; color: "#777"; Layout.fillWidth: true }
        }

        Rectangle { Layout.fillWidth: true; height: 1; color: "#333" }

        ListView {
            id: colList
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: listModel
            clip: true
            spacing: 2

            delegate: Rectangle {
                width: colList.width
                height: 32
                color: index % 2 === 0 ? "#222" : "#1e1e1e"
                radius: 2

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 4
                    anchors.rightMargin: 4
                    spacing: 4

                    CheckBox {
                        Layout.preferredWidth: 30
                        checked: model.colEnabled
                        enabled: model.colIsNote
                        onCheckedChanged: listModel.setProperty(index, "colEnabled", checked)
                        indicator: Rectangle {
                            implicitWidth: 16; implicitHeight: 16
                            x: parent.leftPadding; y: parent.height / 2 - height / 2
                            border.color: "#555"; color: parent.checked ? "#ff764d" : "#2a2a2a"; radius: 2
                            Text { anchors.centerIn: parent; text: parent.parent.checked ? "\u2713" : ""; color: "#0a0a0a"; font.pixelSize: 11 }
                        }
                    }

                    Label {
                        text: model.colTitle
                        font.pixelSize: 12
                        color: model.colEnabled ? "#ddd" : "#666"
                        Layout.preferredWidth: 80
                    }

                    Label {
                        text: model.colChannel
                        font.pixelSize: 11
                        font.family: Qt.platform.os === "osx" ? "Menlo" : "Consolas"
                        color: "#888"
                        Layout.preferredWidth: 60
                    }

                    SpinBox {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 24
                        from: 10; to: 200; stepSize: 5
                        value: model.colWidth
                        editable: true
                        onValueModified: listModel.setProperty(index, "colWidth", value)
                        font.pixelSize: 11

                        background: Rectangle { color: "#2a2a2a"; border.color: "#444"; radius: 2 }
                        contentItem: TextInput {
                            text: parent.textFromValue(parent.value, parent.locale)
                            font: parent.font; color: "#ddd"
                            horizontalAlignment: Qt.AlignHCenter; verticalAlignment: Qt.AlignVCenter
                            readOnly: !parent.editable; validator: parent.validator
                            inputMethodHints: Qt.ImhFormattedNumbersOnly
                        }
                    }
                }
            }
        }
    }
}