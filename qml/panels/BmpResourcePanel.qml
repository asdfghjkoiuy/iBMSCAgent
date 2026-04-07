import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs

Rectangle {
    id: root
    color: "transparent"

    ListView {
        anchors.fill: parent
        model: bmpResourceModel
        clip: true

        delegate: Rectangle {
            width: ListView.view.width
            height: 32
            color: model.isUsed ? "#1a2a1a"
                 : model.filename !== "" ? "#1a1a1a"
                 : "transparent"

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 6
                anchors.rightMargin: 6
                spacing: 4

                Label {
                    text: model.slotLabel
                    font.pixelSize: 10
                    font.family: Qt.platform.os === "osx" ? "Menlo" : "Consolas"
                    color: "#555555"
                    Layout.preferredWidth: 26
                }

                Image {
                    source: model.thumbnailUrl
                    visible: model.thumbnailUrl !== ""
                    Layout.preferredWidth: 24
                    Layout.preferredHeight: 24
                    fillMode: Image.PreserveAspectFit
                }

                TextField {
                    id: filenameField
                    Layout.fillWidth: true
                    font.pixelSize: 11
                    color: "#ffffff"
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

                    onEditingFinished: bmpResourceModel.setFilename(model.index, text)
                }

                Rectangle {
                    Layout.preferredWidth: 22; Layout.preferredHeight: 22
                    color: browseMouseBmp.containsMouse ? "#3d3d3d" : "transparent"
                    radius: 1

                    Text {
                        anchors.centerIn: parent
                        text: "\u2026"
                        color: "#999999"
                        font.pixelSize: 12
                    }

                    MouseArea {
                        id: browseMouseBmp
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: filePicker.open()
                    }

                    FileDialog {
                        id: filePicker
                        nameFilters: ["Image Files (*.bmp *.png *.jpg *.gif *.mpg)", "All Files (*)"]
                        onAccepted: bmpResourceModel.setFilename(model.index, selectedFile)
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
