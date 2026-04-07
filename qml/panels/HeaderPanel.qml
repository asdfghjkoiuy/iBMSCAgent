import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs

ScrollView {
    id: root
    contentWidth: availableWidth

    property var headerFields: [
        "TITLE", "SUBTITLE", "ARTIST", "SUBARTIST", "GENRE",
        "BPM", "PLAYER", "PLAYLEVEL", "RANK", "DIFFICULTY",
        "TOTAL", "VOLWAV", "EXRANK", "COMMENT",
        "STAGEFILE", "BANNER", "BACKBMP", "LNOBJ", "LNTYPE"
    ]
    readonly property var browseFields: ["STAGEFILE", "BANNER", "BACKBMP"]

    FileDialog {
        id: browseDialog
        property string targetField: ""
        title: "Select " + targetField
        onAccepted: {
            var path = selectedFile.toString().replace("file://", "")
            editorDoc.setHeaderValue(targetField, path)
        }
    }

    ColumnLayout {
        width: root.availableWidth
        spacing: 2

        Repeater {
            model: root.headerFields

            RowLayout {
                Layout.fillWidth: true
                spacing: 6

                Label {
                    text: modelData
                    font.pixelSize: 10
                    font.family: Qt.platform.os === "osx" ? "Menlo" : "Consolas"
                    color: "#555555"
                    Layout.preferredWidth: 76
                    horizontalAlignment: Text.AlignRight
                }

                TextField {
                    id: field
                    Layout.fillWidth: true
                    font.pixelSize: 11
                    color: "#ffffff"
                    leftPadding: 6
                    topPadding: 4
                    bottomPadding: 4

                    background: Rectangle {
                        color: field.activeFocus ? "#2a2a2a" : "#1a1a1a"
                        border.color: field.activeFocus ? "#ff764d" : "#222222"
                        border.width: 1
                        radius: 1
                    }

                    Connections {
                        target: editorDoc
                        function onDocumentChanged() {
                            field.text = editorDoc.headerValue(modelData)
                        }
                    }

                    onEditingFinished: {
                        editorDoc.setHeaderValue(modelData, text)
                    }
                }

                // Browse button for file fields
                Rectangle {
                    visible: root.browseFields.indexOf(modelData) >= 0
                    Layout.preferredWidth: visible ? 24 : 0
                    Layout.preferredHeight: 24
                    color: browseMouse.containsMouse ? "#3d3d3d" : "#2a2a2a"
                    border.color: "#333"; radius: 1
                    Text { anchors.centerIn: parent; text: "…"; color: "#aaa"; font.pixelSize: 12 }
                    MouseArea {
                        id: browseMouse; anchors.fill: parent; hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: { browseDialog.targetField = modelData; browseDialog.open() }
                    }
                }
            }
        }
    }
}
