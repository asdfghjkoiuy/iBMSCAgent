import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    color: "transparent"

    property var noteInfo: editorDoc.selectedNoteInfo
    property int slotIndex: noteInfo && noteInfo.hasSelection ? noteInfo.slotIndex : 0
    property var peaks: slotIndex > 0 ? audioEngine.waveformPeaksForSlot(slotIndex) : []
    property bool locked: false
    property int lockedSlot: 0
    property var lockedPeaks: []

    property int activeSlot: locked ? lockedSlot : slotIndex
    property var activePeaks: locked ? lockedPeaks : peaks
    property string activeFilename: {
        if (locked) return lockedFilename
        if (noteInfo && noteInfo.hasSelection) return noteInfo.displayFilename || ""
        return ""
    }
    property string lockedFilename: ""

    // Zoom/pan state
    property real zoomStart: 0.0   // 0..1 fraction of peaks
    property real zoomEnd:   1.0   // 0..1 fraction of peaks

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 6

        RowLayout {
            Layout.fillWidth: true
            spacing: 6

            Label {
                text: activeSlot > 0
                    ? ("Slot " + activeSlot + " — " + activeFilename)
                    : "No waveform"
                color: "#d7d7d7"
                font.pixelSize: 11
                font.bold: true
                Layout.fillWidth: true
                elide: Text.ElideRight
            }

            Button {
                text: root.locked ? "Unlock" : "Lock"
                font.pixelSize: 10
                implicitWidth: 52
                implicitHeight: 22
                onClicked: {
                    if (!root.locked) {
                        root.lockedSlot = root.slotIndex
                        root.lockedPeaks = root.peaks
                        root.lockedFilename = root.activeFilename
                        root.locked = true
                    } else {
                        root.locked = false
                    }
                }
            }

            Button {
                text: "Play"
                font.pixelSize: 10
                implicitWidth: 42
                implicitHeight: 22
                enabled: root.activeSlot > 0
                onClicked: audioEngine.play(root.activeSlot)
            }

            Button {
                text: "1:1"
                font.pixelSize: 10
                implicitWidth: 36
                implicitHeight: 22
                enabled: root.zoomStart > 0.001 || root.zoomEnd < 0.999
                onClicked: { root.zoomStart = 0.0; root.zoomEnd = 1.0 }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: 120
            radius: 3
            color: "#101010"
            border.color: "#2f2f2f"
            border.width: 1

            Canvas {
                id: waveCanvas
                anchors.fill: parent
                anchors.margins: 4

                onPaint: {
                    var ctx = getContext("2d")
                    ctx.reset()
                    ctx.fillStyle = "#101010"
                    ctx.fillRect(0, 0, width, height)

                    var mid = height * 0.5

                    // Center line
                    ctx.strokeStyle = "#1f8f45"
                    ctx.lineWidth = 1
                    ctx.beginPath()
                    ctx.moveTo(0, mid)
                    ctx.lineTo(width, mid)
                    ctx.stroke()

                    var p = root.activePeaks
                    if (!p || p.length === 0) return

                    var iStart = Math.floor(root.zoomStart * (p.length - 1))
                    var iEnd   = Math.ceil(root.zoomEnd   * (p.length - 1))
                    iStart = Math.max(0, iStart)
                    iEnd   = Math.min(p.length - 1, iEnd)
                    var count = iEnd - iStart + 1

                    // Filled waveform
                    ctx.beginPath()
                    for (var i = 0; i < count; ++i) {
                        var x = (i / Math.max(1, count - 1)) * width
                        var amp = Math.max(0, Math.min(1, p[iStart + i]))
                        var y = mid - amp * (height * 0.45)
                        if (i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y)
                    }
                    for (var j = count - 1; j >= 0; --j) {
                        var x2 = (j / Math.max(1, count - 1)) * width
                        var amp2 = Math.max(0, Math.min(1, p[iStart + j]))
                        var y2 = mid + amp2 * (height * 0.45)
                        ctx.lineTo(x2, y2)
                    }
                    ctx.closePath()
                    ctx.fillStyle = "#2ec94a88"
                    ctx.fill()
                    ctx.strokeStyle = "#61d66c"
                    ctx.lineWidth = 1
                    ctx.stroke()

                    // Zoom range indicator (thin bar at bottom)
                    if (root.zoomStart > 0.001 || root.zoomEnd < 0.999) {
                        ctx.fillStyle = "#ffffff22"
                        ctx.fillRect(0, height - 4, width, 4)
                        ctx.fillStyle = "#ff764d"
                        ctx.fillRect(root.zoomStart * width, height - 4,
                                     (root.zoomEnd - root.zoomStart) * width, 4)
                    }
                }

                Connections {
                    target: root
                    function onActivePeaksChanged() { waveCanvas.requestPaint() }
                    function onZoomStartChanged()   { waveCanvas.requestPaint() }
                    function onZoomEndChanged()     { waveCanvas.requestPaint() }
                }

                // Wheel to zoom in/out around cursor
                WheelHandler {
                    onWheel: function(event) {
                        var factor = event.angleDelta.y > 0 ? 0.8 : 1.25
                        var cx = event.x / waveCanvas.width  // 0..1 cursor position
                        var span = root.zoomEnd - root.zoomStart
                        var newSpan = Math.min(1.0, Math.max(0.01, span * factor))
                        var center = root.zoomStart + cx * span
                        var newStart = center - cx * newSpan
                        var newEnd   = newStart + newSpan
                        if (newStart < 0) { newEnd -= newStart; newStart = 0 }
                        if (newEnd > 1)   { newStart -= (newEnd - 1); newEnd = 1 }
                        root.zoomStart = Math.max(0, newStart)
                        root.zoomEnd   = Math.min(1, newEnd)
                    }
                }

                // Drag to pan
                property real _dragX: 0
                property real _dragZoomStart: 0

                DragHandler {
                    onActiveChanged: {
                        if (active) {
                            waveCanvas._dragX = centroid.position.x
                            waveCanvas._dragZoomStart = root.zoomStart
                        }
                    }
                    onCentroidChanged: {
                        if (active) {
                            var dx = (centroid.position.x - waveCanvas._dragX) / waveCanvas.width
                            var span = root.zoomEnd - root.zoomStart
                            var newStart = waveCanvas._dragZoomStart - dx
                            newStart = Math.max(0, Math.min(1 - span, newStart))
                            root.zoomStart = newStart
                            root.zoomEnd   = newStart + span
                        }
                    }
                }
            }

            Label {
                anchors.centerIn: parent
                text: "No waveform loaded"
                color: "#555555"
                font.pixelSize: 11
                visible: !root.activePeaks || root.activePeaks.length === 0
            }
        }
    }
}
