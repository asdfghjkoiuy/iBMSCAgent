import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

/// Ableton-style collapsible sidebar section with a thin header bar.
Item {
    id: root

    property string title: "Section"
    property bool expanded: true
    default property alias content: contentContainer.data

    implicitWidth: parent ? parent.width : 280
    implicitHeight: headerBar.height + (expanded ? contentContainer.implicitHeight : 0)
    clip: true

    Behavior on implicitHeight {
        NumberAnimation { duration: 150; easing.type: Easing.InOutQuad }
    }

    // ── Section header bar ────────────────────────────────────────────────────
    Rectangle {
        id: headerBar
        width: parent.width
        height: 24
        color: headerMouse.containsMouse ? "#333333" : "#2a2a2a"

        Behavior on color { ColorAnimation { duration: 80 } }

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 8
            anchors.rightMargin: 8
            spacing: 6

            Text {
                text: root.expanded ? "\u25BE" : "\u25B8"
                color: "#999999"
                font.pixelSize: 9
            }

            Text {
                text: root.title.toUpperCase()
                color: "#999999"
                font.pixelSize: 10
                font.letterSpacing: 1.5
                font.weight: Font.Medium
                Layout.fillWidth: true
            }
        }

        MouseArea {
            id: headerMouse
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: root.expanded = !root.expanded
        }

        // Bottom border
        Rectangle {
            anchors.bottom: parent.bottom
            width: parent.width
            height: 1
            color: "#222222"
        }
    }

    // ── Content area ──────────────────────────────────────────────────────────
    Item {
        id: contentContainer
        anchors.top: headerBar.bottom
        width: parent.width
        implicitHeight: childrenRect.height
        visible: root.expanded
        opacity: root.expanded ? 1.0 : 0.0

        Behavior on opacity {
            NumberAnimation { duration: 120 }
        }
    }
}
