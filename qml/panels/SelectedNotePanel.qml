import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    color: "transparent"

    property var noteInfo: editorDoc.selectedNoteInfo
    property int slotIndex: noteInfo && noteInfo.hasSelection ? noteInfo.slotIndex : 0
    property var peaks: slotIndex > 0 ? audioEngine.waveformPeaksForSlot(slotIndex) : []

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 8

        Label {
            Layout.fillWidth: true
            text: noteInfo && noteInfo.hasSelection
                ? (noteInfo.selectedCount > 1
                    ? noteInfo.selectedCount + " notes selected, showing first"
                    : "1 selected note")
                : "No note selected"
            color: "#d7d7d7"
            font.pixelSize: 12
            font.bold: true
        }

        GridLayout {
            Layout.fillWidth: true
            columns: 2
            columnSpacing: 10
            rowSpacing: 6
            visible: noteInfo && noteInfo.hasSelection

            Label { text: "Measure"; color: "#777777"; font.pixelSize: 10 }
            Label { text: noteInfo.measureIndex; color: "#f0f0f0"; font.pixelSize: 10 }

            Label { text: "Beat"; color: "#777777"; font.pixelSize: 10 }
            Label { text: noteInfo.measureBeatString; color: "#f0f0f0"; font.pixelSize: 10 }

            Label { text: "Time"; color: "#777777"; font.pixelSize: 10 }
            Label { text: Number(noteInfo.seconds || 0).toFixed(4) + "s"; color: "#f0f0f0"; font.pixelSize: 10 }

            Label { text: "Slot"; color: "#777777"; font.pixelSize: 10 }
            Label { text: noteInfo.slotLabel !== "" ? noteInfo.slotLabel : "—"; color: "#f0f0f0"; font.pixelSize: 10 }

            Label { text: "Filename"; color: "#777777"; font.pixelSize: 10 }
            Label {
                text: noteInfo.displayFilename
                color: "#f0f0f0"
                font.pixelSize: 10
                Layout.fillWidth: true
                elide: Text.ElideRight
            }

            Label { text: "Length"; color: "#777777"; font.pixelSize: 10 }
            Label { text: Number(noteInfo.durationInBeats || 0).toFixed(3); color: "#f0f0f0"; font.pixelSize: 10 }

            Label { text: "Flags"; color: "#777777"; font.pixelSize: 10 }
            Label {
                text: {
                    var flags = []
                    if (noteInfo.isLong) flags.push("Long")
                    if (noteInfo.isHidden) flags.push("Hidden")
                    if (noteInfo.isLandmine) flags.push("Mine")
                    if (noteInfo.hasError) flags.push("Error")
                    return flags.length > 0 ? flags.join(", ") : "None"
                }
                color: noteInfo.hasError ? "#ff9b8f" : "#f0f0f0"
                font.pixelSize: 10
                Layout.fillWidth: true
                wrapMode: Text.Wrap
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 110
            radius: 3
            color: "#101010"
            border.color: "#2f2f2f"
            border.width: 1
            visible: noteInfo && noteInfo.hasSelection && peaks.length > 0

            Canvas {
                id: waveformCanvas
                anchors.fill: parent
                anchors.margins: 6

                onPaint: {
                    var ctx = getContext("2d")
                    ctx.reset()
                    ctx.fillStyle = "#101010"
                    ctx.fillRect(0, 0, width, height)

                    var mid = height * 0.5
                    ctx.strokeStyle = "#1f8f45"
                    ctx.lineWidth = 1
                    ctx.beginPath()
                    ctx.moveTo(0, mid)
                    ctx.lineTo(width, mid)
                    ctx.stroke()

                    if (!root.peaks || root.peaks.length === 0)
                        return

                    ctx.strokeStyle = "#61d66c"
                    ctx.lineWidth = 1
                    ctx.beginPath()
                    for (var i = 0; i < root.peaks.length; ++i) {
                        var x = (i / Math.max(1, root.peaks.length - 1)) * width
                        var amp = Math.max(0, Math.min(1, root.peaks[i]))
                        var y = mid - amp * (height * 0.42)
                        if (i === 0)
                            ctx.moveTo(x, y)
                        else
                            ctx.lineTo(x, y)
                    }
                    for (var j = root.peaks.length - 1; j >= 0; --j) {
                        var x2 = (j / Math.max(1, root.peaks.length - 1)) * width
                        var amp2 = Math.max(0, Math.min(1, root.peaks[j]))
                        var y2 = mid + amp2 * (height * 0.42)
                        ctx.lineTo(x2, y2)
                    }
                    ctx.closePath()
                    ctx.fillStyle = "#2ec94a88"
                    ctx.fill()
                }

                Connections {
                    target: root
                    function onPeaksChanged() { waveformCanvas.requestPaint() }
                }
            }
        }
    }
}
