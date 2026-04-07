import QtQuick 2.15
import QtQuick.Controls 2.15

ComboBox {
    id: control
    implicitWidth: 100
    implicitHeight: 24
    font.pixelSize: 11

    background: Rectangle {
        color: "#2a2a2a"
        border.color: control.activeFocus ? "#ff764d" : "#333333"
        border.width: 1
        radius: 1
    }

    contentItem: Text {
        text: control.displayText
        color: "#ffffff"
        font.pixelSize: 11
        verticalAlignment: Text.AlignVCenter
        leftPadding: 6
        elide: Text.ElideRight
    }

    delegate: ItemDelegate {
        width: control.popup.width
        height: 24
        
        contentItem: Text {
            text: modelData && modelData.text !== undefined ? modelData.text : (model.text !== undefined ? model.text : modelData)
            color: parent.highlighted ? "#ff764d" : "#ffffff"
            font.pixelSize: 11
            verticalAlignment: Text.AlignVCenter
            leftPadding: 6
            elide: Text.ElideNone
        }
        
        background: Rectangle {
            color: parent.highlighted ? "#3d3d3d" : "#1a1a1a"
        }
    }

    popup: Popup {
        y: control.height
        width: control.width
        implicitHeight: contentItem.implicitHeight
        padding: 1

        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model: control.popup.visible ? control.delegateModel : null
            currentIndex: control.highlightedIndex
            ScrollIndicator.vertical: ScrollIndicator { }
        }

        background: Rectangle {
            color: "#1a1a1a"
            border.color: "#333333"
            border.width: 1
            radius: 1
        }
    }
}