import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

/// Modal dialog for BPM manipulation tools.
Dialog {
    id: root
    title: "BPM Tools"
    modal: true
    width: 360
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
                color: closeMouse.containsMouse ? "#3d3d3d" : "#2a2a2a"
                border.color: "#333333"; border.width: 1; radius: 1
                Text { anchors.centerIn: parent; text: "Close"; color: "#ffffff"; font.pixelSize: 11 }
                MouseArea { id: closeMouse; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: root.reject() }
            }
        }
    }

    ColumnLayout {
        width: parent.width
        spacing: 16

        // ── Constant BPM ─────────────────────────────────────────────────
        Label { text: "CONSTANT BPM"; font.pixelSize: 9; font.letterSpacing: 1.5; color: "#666" }

        RowLayout {
            spacing: 8
            Label { text: "Target BPM:"; font.pixelSize: 12; color: "#aaa" }
            TextField {
                id: constBpmInput
                Layout.preferredWidth: 80
                text: editorDoc ? editorDoc.headerValue("BPM") : "130"
                font.pixelSize: 13; color: "#fff"
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator { bottom: 1; top: 9999 }
                selectByMouse: true
                background: Rectangle { color: "#2a2a2a"; border.color: constBpmInput.activeFocus ? "#ff764d" : "#333"; border.width: 1; radius: 1 }
            }
            Rectangle {
                Layout.preferredWidth: 70; Layout.preferredHeight: 28
                color: constMouse.containsMouse ? "#ff8f6d" : "#ff764d"; radius: 1
                Text { anchors.centerIn: parent; text: "Apply"; color: "#0a0a0a"; font.pixelSize: 11; font.weight: Font.Medium }
                MouseArea { id: constMouse; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor
                    onClicked: { var v = parseInt(constBpmInput.text); if (v > 0) appController.constBpm(v) }
                }
            }
        }

        Rectangle { Layout.fillWidth: true; height: 1; color: "#222" }

        // ── Scale BPM ────────────────────────────────────────────────────
        Label { text: "SCALE BPM"; font.pixelSize: 9; font.letterSpacing: 1.5; color: "#666" }

        RowLayout {
            spacing: 8
            Label { text: "Ratio:"; font.pixelSize: 12; color: "#aaa" }
            TextField {
                id: scaleInput
                Layout.preferredWidth: 80
                text: "1.0"
                font.pixelSize: 13; color: "#fff"
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                selectByMouse: true
                background: Rectangle { color: "#2a2a2a"; border.color: scaleInput.activeFocus ? "#ff764d" : "#333"; border.width: 1; radius: 1 }
            }
            Rectangle {
                Layout.preferredWidth: 70; Layout.preferredHeight: 28
                color: scaleMouse.containsMouse ? "#ff8f6d" : "#ff764d"; radius: 1
                Text { anchors.centerIn: parent; text: "Apply"; color: "#0a0a0a"; font.pixelSize: 11; font.weight: Font.Medium }
                MouseArea { id: scaleMouse; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor
                    onClicked: { var v = parseFloat(scaleInput.text); if (v > 0) appController.scaleBpm(v) }
                }
            }
        }

        RowLayout {
            spacing: 8
            Repeater {
                model: [{"label": "×2", "ratio": 2.0}, {"label": "÷2", "ratio": 0.5}]
                Rectangle {
                    Layout.preferredWidth: 50; Layout.preferredHeight: 28
                    color: qMouse.containsMouse ? "#3d3d3d" : "#2a2a2a"
                    border.color: "#444"; radius: 1
                    Text { anchors.centerIn: parent; text: modelData.label; color: "#ddd"; font.pixelSize: 12 }
                    MouseArea { id: qMouse; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor
                        onClicked: appController.scaleBpm(modelData.ratio)
                    }
                }
            }
        }
    }
}
