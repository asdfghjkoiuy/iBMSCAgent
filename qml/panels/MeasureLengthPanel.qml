import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    color: "transparent"

    ListView {
        anchors.fill: parent
        model: measureLengthModel
        clip: true

        delegate: Rectangle {
            width: ListView.view.width
            height: 26
            color: model.isNonDefault ? "#1a2a1a" : "transparent"

            RowLayout {
                anchors.fill: parent
                anchors.margins: 4
                spacing: 6

                Label {
                    text: "#" + model.measureIndex
                    font.pixelSize: 10
                    font.family: Qt.platform.os === "osx" ? "Menlo" : "Consolas"
                    color: "#555555"
                    Layout.preferredWidth: 36
                }

                TextField {
                    Layout.fillWidth: true
                    font.pixelSize: 11
                    color: "#ffffff"
                    text: model.length.toFixed(4)
                    topPadding: 2
                    bottomPadding: 2

                    background: Rectangle {
                        color: parent.activeFocus ? "#2a2a2a" : "transparent"
                        border.color: parent.activeFocus ? "#ff764d" : "transparent"
                        border.width: 1
                        radius: 1
                    }

                    onEditingFinished: measureLengthModel.setLength(model.measureIndex, parseFloat(text))
                }
            }
        }
    }
}
