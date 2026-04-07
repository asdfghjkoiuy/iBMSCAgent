import QtQuick 2.15
import QtQuick.Controls 2.15

/// A panel that can be shown/hidden with a smooth animation.
Item {
    id: root

    property bool expanded: true
    property int expandedWidth: 240
    property int expandedHeight: -1  // -1 = use height binding
    property bool horizontal: false  // if true, animates width; otherwise height

    width:  horizontal ? (expanded ? expandedWidth  : 0) : implicitWidth
    height: !horizontal ? (expanded ? (expandedHeight > 0 ? expandedHeight : contentLoader.implicitHeight) : 0) : implicitHeight

    clip: true

    default property alias content: contentLoader.sourceComponent

    Loader {
        id: contentLoader
        anchors.fill: parent
    }

    Behavior on width  { NumberAnimation { duration: 150; easing.type: Easing.InOutQuad } }
    Behavior on height { NumberAnimation { duration: 150; easing.type: Easing.InOutQuad } }
}
