import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    color: "#0a0a0a"

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 20
        width: 360

        // ── Logo ──────────────────────────────────────────────────────────────
        Column {
            Layout.alignment: Qt.AlignHCenter
            spacing: 4

            Text {
                text: "iBMSC"
                font.pixelSize: 42
                font.weight: Font.Bold
                color: "#ff764d"
                font.letterSpacing: 2
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Text {
                text: "BMS CHART EDITOR"
                font.pixelSize: 10
                color: "#555555"
                font.letterSpacing: 3
                font.weight: Font.Medium
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }

        // ── Divider ───────────────────────────────────────────────────────────
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            color: "#222222"
            Layout.topMargin: 4
            Layout.bottomMargin: 4
        }

        // ── Action buttons ────────────────────────────────────────────────────
        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: 8

            Rectangle {
                Layout.preferredWidth: 140; Layout.preferredHeight: 34
                color: newBtnMouse.containsMouse ? "#3d3d3d" : "#2a2a2a"
                radius: 1

                Behavior on color { ColorAnimation { duration: 60 } }

                Text {
                    anchors.centerIn: parent
                    text: "New File"
                    color: "#ffffff"
                    font.pixelSize: 12
                }

                MouseArea {
                    id: newBtnMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: appController.newFile()
                }
            }

            Rectangle {
                Layout.preferredWidth: 140; Layout.preferredHeight: 34
                color: openBtnMouse.containsMouse ? "#ff8f6d" : "#ff764d"
                radius: 1

                Behavior on color { ColorAnimation { duration: 60 } }

                Text {
                    anchors.centerIn: parent
                    text: "Open File..."
                    color: "#0a0a0a"
                    font.pixelSize: 12
                    font.weight: Font.Medium
                }

                MouseArea {
                    id: openBtnMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: appController.requestOpenDialog()
                }
            }
        }

        // ── Recent files ──────────────────────────────────────────────────────
        Column {
            Layout.fillWidth: true
            spacing: 4
            visible: recentList.count > 0

            Text {
                text: "RECENT FILES"
                font.pixelSize: 9
                font.weight: Font.Medium
                font.letterSpacing: 1.5
                color: "#555555"
                leftPadding: 2
            }

            Rectangle {
                width: parent.width
                height: recentList.implicitHeight
                color: "#1a1a1a"
                border.color: "#222222"
                border.width: 1
                clip: true

                ListView {
                    id: recentList
                    anchors.fill: parent
                    anchors.margins: 1
                    implicitHeight: Math.min(contentHeight, 220)
                    clip: true
                    model: appSettings.recentFiles
                    spacing: 0

                    delegate: ItemDelegate {
                        id: recentItem
                        width: ListView.view.width
                        height: 32
                        padding: 0
                        leftPadding: 10
                        rightPadding: 10

                        background: Rectangle {
                            color: recentItem.hovered ? "#2a2a2a" : "transparent"
                            Behavior on color { ColorAnimation { duration: 60 } }
                        }

                        contentItem: RowLayout {
                            spacing: 8

                            Text {
                                text: "\u266A"
                                color: "#555555"
                                font.pixelSize: 10
                            }

                            Column {
                                Layout.fillWidth: true
                                spacing: 1

                                Text {
                                    width: parent.width
                                    text: modelData.split("/").pop()
                                    font.pixelSize: 12
                                    color: "#ffffff"
                                    elide: Text.ElideRight
                                }
                                Text {
                                    width: parent.width
                                    text: modelData
                                    font.pixelSize: 9
                                    color: "#555555"
                                    elide: Text.ElideLeft
                                }
                            }
                        }

                        onClicked: appController.openFile(modelData)
                    }

                    ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
                }
            }
        }

        // ── Keyboard hint ─────────────────────────────────────────────────────
        Text {
            Layout.alignment: Qt.AlignHCenter
            text: "Ctrl+N  New  \u00B7  Ctrl+O  Open  \u00B7  Drag & drop supported"
            font.pixelSize: 9
            color: "#555555"
        }
    }
}
