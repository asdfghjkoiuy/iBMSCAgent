import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

/// Statistics dialog showing note counts by type.
Dialog {
    id: root
    title: "Statistics"
    modal: true
    width: 360
    x: parent ? (parent.width - width) / 2 : 0
    y: parent ? (parent.height - height) / 2 : 0

    background: Rectangle { color: "#1a1a1a"; border.color: "#333333"; border.width: 1 }

    property var stats: ({})

    onOpened: {
        if (editorDoc) stats = editorDoc.noteStatistics()
    }

    footer: Rectangle {
        color: "#1a1a1a"
        height: 40
        RowLayout {
            anchors.fill: parent
            anchors.rightMargin: 12
            spacing: 8
            Item { Layout.fillWidth: true }
            Rectangle {
                Layout.preferredWidth: 60; Layout.preferredHeight: 24
                color: okMouse.containsMouse ? "#3d3d3d" : "#2a2a2a"
                border.color: "#333333"; border.width: 1; radius: 1
                Text { anchors.centerIn: parent; text: "Close"; color: "#fff"; font.pixelSize: 11 }
                MouseArea { id: okMouse; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: root.accept() }
            }
        }
    }

    ColumnLayout {
        width: parent.width
        spacing: 8

        Label { text: "NOTE STATISTICS"; font.pixelSize: 9; font.letterSpacing: 1.5; color: "#666" }
        Rectangle { Layout.fillWidth: true; height: 1; color: "#333" }

        GridLayout {
            columns: 2
            columnSpacing: 16
            rowSpacing: 4
            Layout.fillWidth: true

            Label { text: "Total Notes"; color: "#aaa"; font.pixelSize: 11 }
            Label { text: stats.total || "0"; color: "#fff"; font.pixelSize: 11; font.family: "Menlo" }

            Label { text: "BPM Changes"; color: "#aaa"; font.pixelSize: 11 }
            Label { text: stats.bpm || "0"; color: "#e5c07b"; font.pixelSize: 11; font.family: "Menlo" }

            Label { text: "STOP Events"; color: "#aaa"; font.pixelSize: 11 }
            Label { text: stats.stop || "0"; color: "#e5c07b"; font.pixelSize: 11; font.family: "Menlo" }

            Label { text: "BGM Notes"; color: "#aaa"; font.pixelSize: 11 }
            Label { text: stats.bgm || "0"; color: "#e06c75"; font.pixelSize: 11; font.family: "Menlo" }
        }

        Rectangle { Layout.fillWidth: true; height: 1; color: "#333" }
        Label { text: "P1 (A-GROUP)"; font.pixelSize: 9; font.letterSpacing: 1.5; color: "#666" }

        GridLayout {
            columns: 2
            columnSpacing: 16
            rowSpacing: 4
            Layout.fillWidth: true

            Label { text: "Short"; color: "#aaa"; font.pixelSize: 11 }
            Label { text: stats.aShort || "0"; color: "#61afef"; font.pixelSize: 11; font.family: "Menlo" }

            Label { text: "Long"; color: "#aaa"; font.pixelSize: 11 }
            Label { text: stats.aLong || "0"; color: "#98c379"; font.pixelSize: 11; font.family: "Menlo" }

            Label { text: "Hidden"; color: "#aaa"; font.pixelSize: 11 }
            Label { text: stats.aHidden || "0"; color: "#888"; font.pixelSize: 11; font.family: "Menlo" }

            Label { text: "Landmine"; color: "#aaa"; font.pixelSize: 11 }
            Label { text: stats.aMine || "0"; color: "#e06c75"; font.pixelSize: 11; font.family: "Menlo" }
        }

        Rectangle { Layout.fillWidth: true; height: 1; color: "#333" }
        Label { text: "P2 (D-GROUP)"; font.pixelSize: 9; font.letterSpacing: 1.5; color: "#666" }

        GridLayout {
            columns: 2
            columnSpacing: 16
            rowSpacing: 4
            Layout.fillWidth: true

            Label { text: "Short"; color: "#aaa"; font.pixelSize: 11 }
            Label { text: stats.dShort || "0"; color: "#61afef"; font.pixelSize: 11; font.family: "Menlo" }

            Label { text: "Long"; color: "#aaa"; font.pixelSize: 11 }
            Label { text: stats.dLong || "0"; color: "#98c379"; font.pixelSize: 11; font.family: "Menlo" }

            Label { text: "Hidden"; color: "#aaa"; font.pixelSize: 11 }
            Label { text: stats.dHidden || "0"; color: "#888"; font.pixelSize: 11; font.family: "Menlo" }

            Label { text: "Landmine"; color: "#aaa"; font.pixelSize: 11 }
            Label { text: stats.dMine || "0"; color: "#e06c75"; font.pixelSize: 11; font.family: "Menlo" }
        }
    }
}
