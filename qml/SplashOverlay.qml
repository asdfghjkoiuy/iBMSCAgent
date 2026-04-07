import QtQuick 2.15

/// Full-window splash overlay shown on startup, fades out after 1.5s.
Rectangle {
    id: splash
    anchors.fill: parent
    color: "#0a0a0a"
    z: 9999
    opacity: 1.0

    Column {
        anchors.centerIn: parent
        spacing: 12

        Text {
            text: "iBMSC"
            font.pixelSize: 48
            font.weight: Font.Bold
            font.letterSpacing: 4
            color: "#ff764d"
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Text {
            text: "BMS Chart Editor"
            font.pixelSize: 14
            font.letterSpacing: 2
            color: "#666666"
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Item { width: 1; height: 20 }

        Text {
            text: "v4.0  Qt/C++ Rewrite"
            font.pixelSize: 11
            color: "#444444"
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }

    Text {
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 24
        anchors.horizontalCenter: parent.horizontalCenter
        text: "exch-bms2@hotmail.com"
        font.pixelSize: 10
        color: "#333333"
    }

    // Fade out after 1.5s
    Timer {
        id: splashTimer
        interval: 1500
        running: true
        onTriggered: fadeOut.start()
    }

    NumberAnimation {
        id: fadeOut
        target: splash
        property: "opacity"
        from: 1.0; to: 0.0
        duration: 400
        easing.type: Easing.InQuad
        onFinished: splash.visible = false
    }
}
