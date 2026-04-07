import QtQuick 2.15
import QtQuick.Controls 2.15

/// A fixed-height header bar drawn above ChartCanvas.
/// Reads column layout from appSettings.columnConfigs.
Item {
    id: root
    implicitHeight: 26

    readonly property double rulerWidth:    40.0
    readonly property double columnSpacing: 2.0

    Rectangle {
        anchors.fill: parent
        color: "#0a0a0a"

        Rectangle {
            anchors.bottom: parent.bottom
            width: parent.width
            height: 1
            color: "#222222"
        }

        Item {
            x: 0; y: 0
            width: root.rulerWidth
            height: parent.height

            Text {
                anchors.centerIn: parent
                text: "BAR"
                font.pixelSize: 9
                font.weight: Font.Medium
                color: "#555555"
                font.letterSpacing: 0.8
            }
        }

        Repeater {
            model: appSettings ? appSettings.columnConfigs : []

            Item {
                visible: modelData.enabled && modelData.isNoteColumn
                x: {
                    // Compute x by summing widths of preceding enabled columns
                    var configs = appSettings.columnConfigs
                    var pos = root.rulerWidth
                    for (var i = 0; i < index; i++) {
                        if (configs[i].enabled && configs[i].isNoteColumn)
                            pos += configs[i].width + root.columnSpacing
                    }
                    return pos
                }
                y: 0
                width: modelData.width
                height: parent.height

                Rectangle {
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: 2
                    color: modelData.isSound ? "#61afef" : "#e5c07b"
                    opacity: 0.6
                }

                Text {
                    anchors.centerIn: parent
                    anchors.verticalCenterOffset: 1
                    text: modelData.title
                    font.pixelSize: 10
                    font.weight: Font.Medium
                    color: "#cccccc"
                    font.family: Qt.platform.os === "osx" ? "Menlo" : "Consolas"
                    opacity: 0.8
                }

                MouseArea {
                    anchors.fill: parent
                    // No action — column visibility is managed via Column Config dialog
                }
            }
        }
    }
}
