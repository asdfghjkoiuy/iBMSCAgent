import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

/// Modal dialog that accepts a measure number and emits jumpRequested(beat).
Dialog {
    id: root
    title: "Jump to Measure"
    modal: true
    width: 320
    x: parent ? (parent.width - width) / 2 : 0
    y: parent ? (parent.height - height) / 2 : 0

    background: Rectangle { color: "#1a1a1a"; border.color: "#333333"; border.width: 1 }

    signal jumpRequested(double beat)

    onOpened: {
        measureInput.forceActiveFocus()
        measureInput.selectAll()
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
                Text { anchors.centerIn: parent; text: "OK"; color: "#0a0a0a"; font.pixelSize: 11; font.weight: Font.Medium }
                MouseArea { id: okMouse; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: {
                    var m = parseInt(measureInput.text, 10)
                    if (!isNaN(m) && m >= 0 && m <= 999) {
                        root.jumpRequested(m * 4.0)
                        root.accept()
                    }
                } }
            }
        }
    }

    ColumnLayout {
        width: parent.width
        spacing: 12

        Label {
            text: "Measure (0 \u2013 999):"
            font.pixelSize: 12
            color: "#999999"
        }

        TextField {
            id: measureInput
            Layout.fillWidth: true
            placeholderText: "0"
            text: "0"
            font.pixelSize: 15
            font.family: Qt.platform.os === "osx" ? "Menlo" : "Consolas"
            color: "#ffffff"
            inputMethodHints: Qt.ImhDigitsOnly
            validator: IntValidator { bottom: 0; top: 999 }
            selectByMouse: true

            background: Rectangle {
                color: "#2a2a2a"
                border.color: measureInput.activeFocus ? "#ff764d" : "#333333"
                border.width: 1
                radius: 1
            }

            Keys.onReturnPressed: root.accept()
            Keys.onEnterPressed:  root.accept()
        }

        Label {
            text: "Tip: Use Ctrl+G to open this dialog quickly."
            font.pixelSize: 10
            color: "#555555"
            wrapMode: Text.Wrap
            Layout.fillWidth: true
        }
    }
}
