import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Dialog {
    id: root
    title: "Edit Note Value"
    modal: true
    width: 280
    x: parent ? (parent.width - width) / 2 : 0
    y: parent ? (parent.height - height) / 2 : 0

    background: Rectangle { color: "#1a1a1a"; border.color: "#333333"; border.width: 1 }

    property int noteChannel: 0
    property int noteMeasure: 0
    property int noteBeatNum: 0
    property int noteBeatDen: 1
    property int currentVal: 1

    onOpened: { valInput.text = currentVal.toString(); valInput.forceActiveFocus(); valInput.selectAll() }

    footer: Rectangle {
        color: "#1a1a1a"; height: 40
        RowLayout {
            anchors.fill: parent; anchors.rightMargin: 12; anchors.leftMargin: 12; spacing: 8
            Item { Layout.fillWidth: true }
            Rectangle {
                Layout.preferredWidth: 60; Layout.preferredHeight: 24
                color: cMouse.containsMouse ? "#3d3d3d" : "#2a2a2a"
                border.color: "#333"; border.width: 1; radius: 1
                Text { anchors.centerIn: parent; text: "Cancel"; color: "#fff"; font.pixelSize: 11 }
                MouseArea { id: cMouse; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: root.reject() }
            }
            Rectangle {
                Layout.preferredWidth: 60; Layout.preferredHeight: 24
                color: oMouse.containsMouse ? "#ff8f6d" : "#ff764d"; radius: 1
                Text { anchors.centerIn: parent; text: "OK"; color: "#0a0a0a"; font.pixelSize: 11; font.weight: Font.Medium }
                MouseArea { id: oMouse; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        var v = parseInt(valInput.text)
                        if (!isNaN(v) && v >= 1 && v <= 1295) {
                            editorDoc.modifyLabels(v)
                            root.accept()
                        }
                    }
                }
            }
        }
    }

    ColumnLayout {
        width: parent.width; spacing: 12
        Label { text: "Value (1-1295):"; font.pixelSize: 12; color: "#999" }
        TextField {
            id: valInput; Layout.fillWidth: true
            font.pixelSize: 15; font.family: "Menlo"; color: "#fff"
            inputMethodHints: Qt.ImhDigitsOnly
            validator: IntValidator { bottom: 1; top: 1295 }
            selectByMouse: true
            background: Rectangle { color: "#2a2a2a"; border.color: valInput.activeFocus ? "#ff764d" : "#333"; border.width: 1; radius: 1 }
            Keys.onReturnPressed: oMouse.clicked(null)
            Keys.onEnterPressed: oMouse.clicked(null)
        }
    }
}
