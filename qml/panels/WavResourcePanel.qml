import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs

Rectangle {
    id: root
    color: "transparent"

    ListView {
        anchors.fill: parent
        model: wavResourceModel
        clip: true

        delegate: Rectangle {
            width: ListView.view.width
            height: 28
            color: model.isCurrent ? "#2a3548"
                 : model.isUsed ? "#1a2a1a"
                 : model.filename !== "" ? "#1a1a1a"
                 : "transparent"
            border.color: model.isCurrent ? "#8bc6ff" : "transparent"
            border.width: model.isCurrent ? 1 : 0

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 6
                anchors.rightMargin: 6
                spacing: 4

                Label {
                    text: model.slotLabel
                    font.pixelSize: 10
                    font.family: Qt.platform.os === "osx" ? "Menlo" : "Consolas"
                    color: model.isCurrent ? "#d8ecff" : "#555555"
                    Layout.preferredWidth: 26
                }

                TextField {
                    id: filenameField
                    Layout.fillWidth: true
                    font.pixelSize: 11
                    color: model.isCurrent ? "#f3f8ff" : "#ffffff"
                    leftPadding: 4
                    topPadding: 2
                    bottomPadding: 2
                    text: model.filename
                    placeholderText: "\u2014"

                    background: Rectangle {
                        color: filenameField.activeFocus ? "#2a2a2a" : "transparent"
                        border.color: filenameField.activeFocus ? "#ff764d" : "transparent"
                        border.width: 1
                        radius: 1
                    }

                    onEditingFinished: {
                        wavResourceModel.setFilename(model.index, text)
                    }
                }

                Rectangle {
                    Layout.preferredWidth: 22; Layout.preferredHeight: 22
                    color: browseMouseWav.containsMouse ? "#3d3d3d" : "transparent"
                    radius: 1

                    Text {
                        anchors.centerIn: parent
                        text: "\u2026"
                        color: "#999999"
                        font.pixelSize: 12
                    }

                    MouseArea {
                        id: browseMouseWav
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: filePicker.open()
                    }

                    FileDialog {
                        id: filePicker
                        nameFilters: ["Audio Files (*.wav *.ogg *.mp3 *.flac)", "All Files (*)"]
                        onAccepted: wavResourceModel.setFilename(model.index, selectedFile)
                    }
                }

                Rectangle {
                    Layout.preferredWidth: 22; Layout.preferredHeight: 22
                    color: playMouseWav.containsMouse ? "#3d3d3d" : "transparent"
                    radius: 1
                    opacity: model.filename !== "" ? 1.0 : 0.3

                    Text {
                        anchors.centerIn: parent
                        text: "\u25B6"
                        color: "#ff764d"
                        font.pixelSize: 10
                    }

                    MouseArea {
                        id: playMouseWav
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: model.filename !== "" ? Qt.PointingHandCursor : Qt.ArrowCursor
                        enabled: model.filename !== ""
                        onClicked: audioEngine.play(model.slotIndex)
                    }
                }

                Rectangle {
                    Layout.preferredWidth: 28; Layout.preferredHeight: 22
                    color: convMouseWav.containsMouse ? "#3d3d3d" : "transparent"
                    radius: 1
                    opacity: model.filename !== "" ? 1.0 : 0.3

                    Text {
                        anchors.centerIn: parent
                        text: "OGG"
                        color: "#77d09b"
                        font.pixelSize: 9
                        font.bold: true
                    }

                    MouseArea {
                        id: convMouseWav
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: model.filename !== "" ? Qt.PointingHandCursor : Qt.ArrowCursor
                        enabled: model.filename !== ""
                        onClicked: appController.convertWavSlotToOgg(model.slotIndex)
                    }
                }
            }

            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: 1
                color: "#222222"
            }
        }
    }
}
