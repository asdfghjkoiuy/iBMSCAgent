import QtQuick 2.15

/// Transparent overlay that draws BPM-change and STOP labels over the chart canvas.
Canvas {
    id: root

    property double scrollBeat: 0
    property double pxPerBeat: 80
    property double canvasHeight: height

    readonly property double rulerWidth:    40.0
    readonly property double totalWidth:    9 * (48.0 + 2.0)

    onScrollBeatChanged:  requestPaint()
    onPxPerBeatChanged:   requestPaint()
    onCanvasHeightChanged:requestPaint()

    Connections {
        target: editorDoc
        function onDocumentChanged() { root.requestPaint() }
    }

    onPaint: {
        var ctx = getContext("2d")
        ctx.clearRect(0, 0, width, height)

        var events = editorDoc.timingEvents()
        if (!events || events.length === 0) return

        var h = height

        for (var i = 0; i < events.length; ++i) {
            var ev = events[i]
            var beat = ev.beat
            var y = h - (beat - root.scrollBeat) * root.pxPerBeat

            if (y < -2 || y > h + 2) continue

            var isStop = ev.isStop
            var label  = ev.label

            ctx.save()
            ctx.setLineDash([4, 3])
            ctx.lineWidth = 1
            ctx.strokeStyle = isStop ? "rgba(217, 68, 82, 0.7)"
                                     : "rgba(212, 168, 67, 0.7)"
            ctx.beginPath()
            ctx.moveTo(root.rulerWidth, y)
            ctx.lineTo(root.rulerWidth + root.totalWidth, y)
            ctx.stroke()
            ctx.restore()

            ctx.save()
            ctx.font = "10px " + (Qt.platform.os === "osx" ? "Menlo" : "Consolas")
            ctx.fillStyle = isStop ? "rgba(217, 68, 82, 0.9)"
                                   : "rgba(212, 168, 67, 0.9)"
            ctx.fillText(label, 2, Math.max(y - 3, 10))
            ctx.restore()
        }
    }
}
