import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

/// Find & Replace dialog for header fields and WAV/BMP resource filenames.
Dialog {
    id: root
    title: "Find / Replace"
    modal: true
    width: 460
    x: parent ? (parent.width - width) / 2 : 0
    y: parent ? (parent.height - height) / 2 : 0
    height: 340

    background: Rectangle { color: "#1a1a1a"; border.color: "#333333"; border.width: 1 }

    footer: Rectangle {
        color: "#1a1a1a"
        height: 40
        RowLayout {
            anchors.fill: parent
            anchors.rightMargin: 12
            anchors.leftMargin: 12
            spacing: 8
            Item { Layout.fillWidth: true }
            
            Rectangle {
                Layout.preferredWidth: 60; Layout.preferredHeight: 24
                color: closeMouse.containsMouse ? "#3d3d3d" : "#2a2a2a"
                border.color: "#333333"; border.width: 1; radius: 1
                Text { anchors.centerIn: parent; text: "Close"; color: "#ffffff"; font.pixelSize: 11 }
                MouseArea { id: closeMouse; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: root.close() }
            }
        }
    }

    property int matchCount: 0
    property string statusText: ""

    onOpened: findField.forceActiveFocus()

    readonly property var headerFields: [
        "TITLE","SUBTITLE","ARTIST","SUBARTIST","GENRE",
        "STAGEFILE","BANNER","BACKBMP","VOLWAV"
    ]

    function doFind() {
        var term = findField.text.trim()
        if (term === "") { statusLabel.text = ""; return }
        var count = 0
        var ic = caseCheck.checked

        for (var i = 0; i < headerFields.length; ++i) {
            var val = editorDoc.headerValue(headerFields[i])
            var haystack = ic ? val.toLowerCase() : val
            var needle   = ic ? term.toLowerCase() : term
            if (haystack.indexOf(needle) >= 0) count++
        }
        for (var r = 0; r < wavResourceModel.rowCount(); ++r) {
            var fname = wavResourceModel.data(wavResourceModel.index(r, 0), 258)
            if (fname) {
                var h2 = ic ? fname.toLowerCase() : fname
                var n2 = ic ? term.toLowerCase() : term
                if (h2.indexOf(n2) >= 0) count++
            }
        }
        statusLabel.text = count === 0
            ? "No matches."
            : count + " match" + (count > 1 ? "es" : "") + " found."
        statusLabel.color = count === 0 ? "#d94452" : "#7ec856"
    }

    function doReplace() {
        var term    = findField.text.trim()
        var repl    = replaceField.text
        if (term === "") return
        var ic      = caseCheck.checked
        var count   = 0

        for (var i = 0; i < headerFields.length; ++i) {
            var key = headerFields[i]
            var val = editorDoc.headerValue(key)
            var haystack = ic ? val.toLowerCase() : val
            var needle   = ic ? term.toLowerCase() : term
            if (haystack.indexOf(needle) >= 0) {
                var flags = ic ? "gi" : "g"
                var newVal = val.replace(new RegExp(term.replace(/[.*+?^${}()|[\]\\]/g, '\\$&'), flags), repl)
                editorDoc.setHeaderValue(key, newVal)
                count++
            }
        }
        statusLabel.text = count === 0
            ? "No replacements made."
            : "Replaced " + count + " header field" + (count > 1 ? "s" : "") + "."
        statusLabel.color = count === 0 ? "#d94452" : "#7ec856"
    }

    ColumnLayout {
        width: parent.width
        height: parent.height
        spacing: 14

        TabBar {
            id: scopeBar
            Layout.fillWidth: true
            background: Rectangle { color: "#0a0a0a" }

            TabButton {
                text: "Header Fields"
                background: Rectangle { color: parent.checked ? "#2a2a2a" : "#0a0a0a" }
                contentItem: Text { text: parent.text; color: parent.checked ? "#ffffff" : "#555555"; font.pixelSize: 11; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
            }
            TabButton {
                text: "WAV / BMP Files"
                background: Rectangle { color: parent.checked ? "#2a2a2a" : "#0a0a0a" }
                contentItem: Text { text: parent.text; color: parent.checked ? "#ffffff" : "#555555"; font.pixelSize: 11; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
            }
        }

        GridLayout {
            columns: 3
            Layout.fillWidth: true
            columnSpacing: 8
            rowSpacing: 8

            Label { text: "Find:"; font.pixelSize: 12; color: "#999999" }
            TextField {
                id: findField
                Layout.fillWidth: true
                Layout.columnSpan: 2
                placeholderText: "Search text..."
                color: "#ffffff"
                font.pixelSize: 11
                selectByMouse: true
                background: Rectangle { color: "#2a2a2a"; border.color: findField.activeFocus ? "#ff764d" : "#333333"; border.width: 1; radius: 1 }
                onTextChanged: root.doFind()
                Keys.onReturnPressed: root.doFind()
            }

            Label { text: "Replace:"; font.pixelSize: 12; color: "#999999" }
            TextField {
                id: replaceField
                Layout.fillWidth: true
                Layout.columnSpan: 2
                placeholderText: "Replacement text..."
                color: "#ffffff"
                font.pixelSize: 11
                selectByMouse: true
                background: Rectangle { color: "#2a2a2a"; border.color: replaceField.activeFocus ? "#ff764d" : "#333333"; border.width: 1; radius: 1 }
            }

            Item {}
            CheckBox {
                id: caseCheck
                text: "Case-insensitive"
                font.pixelSize: 11
                onCheckedChanged: root.doFind()
            }
            Item {}
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Rectangle {
                Layout.preferredWidth: findBtnText.implicitWidth + 24; Layout.preferredHeight: 28
                color: findBtnMouse.containsMouse ? "#3d3d3d" : "#2a2a2a"
                border.color: "#333333"; border.width: 1; radius: 1
                Text { id: findBtnText; anchors.centerIn: parent; text: "Find"; color: "#ffffff"; font.pixelSize: 11 }
                MouseArea { id: findBtnMouse; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: root.doFind() }
            }

            Rectangle {
                Layout.preferredWidth: replaceBtnText.implicitWidth + 24; Layout.preferredHeight: 28
                color: replaceBtnMouse.containsMouse ? "#ff8f6d" : "#ff764d"
                radius: 1
                opacity: scopeBar.currentIndex === 0 ? 1.0 : 0.3
                Text { id: replaceBtnText; anchors.centerIn: parent; text: "Replace All"; color: "#0a0a0a"; font.pixelSize: 11; font.weight: Font.Medium }
                MouseArea { id: replaceBtnMouse; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; enabled: scopeBar.currentIndex === 0; onClicked: root.doReplace() }
            }

            Item { Layout.fillWidth: true }
        }

        Label {
            id: statusLabel
            Layout.fillWidth: true
            font.pixelSize: 11
            wrapMode: Text.Wrap
        }

        Item { Layout.fillHeight: true }
    }
}
