import QtQuick

Rectangle {
    color: "black"
    border.color: "#404040"
    border.width: 1

    Canvas {
        id: spectrumCanvas
        anchors.fill: parent
        anchors.margins: 8

        property real maxDb: orion.displaySettings.spectrumOffsetDb
        property real minDb: maxDb - orion.displaySettings.spectrumRangeDb
        property var peakBins: []

        onMaxDbChanged: requestPaint()
        onMinDbChanged: requestPaint()

        onPaint: {
            var ctx = getContext("2d")
            ctx.reset()

            ctx.fillStyle = "black"
            ctx.fillRect(0, 0, width, height)

            if (orion.displaySettings.gridEnabled)
                drawGrid(ctx)

            drawTrace(ctx)

            if (orion.displaySettings.centerLineEnabled)
                drawCenterLine(ctx)
        }

        function dbToY(db) {
            var norm = (db - minDb) / (maxDb - minDb)
            var y = height - norm * height
            return Math.max(0, Math.min(height, y))
        }

        function drawGrid(ctx) {
            ctx.strokeStyle = "#1f3a2f"
            ctx.lineWidth = 1

            for (var i = 1; i < 10; ++i) {
                var x = i * width / 10
                ctx.beginPath()
                ctx.moveTo(x, 0)
                ctx.lineTo(x, height)
                ctx.stroke()
            }

            // The vertical scale always uses 10 dB divisions. Offset and
            // range therefore change both the labels and grid automatically.
            var divisionDb = 10
            var divisionCount = Math.round((maxDb - minDb) / divisionDb)

            for (var j = 1; j < divisionCount; ++j) {
                var gridDb = maxDb - j * divisionDb
                var y = dbToY(gridDb)

                ctx.beginPath()
                ctx.moveTo(0, y)
                ctx.lineTo(width, y)
                ctx.stroke()
            }

            if (orion.displaySettings.showDbScale) {
                ctx.fillStyle = "#607060"
                ctx.font = "12px Consolas"

                for (var d = maxDb; d >= minDb - 0.1; d -= divisionDb) {
                    var yy = dbToY(d)
                    var labelY = Math.max(12, Math.min(height - 3, yy - 3))
                    ctx.fillText(Math.round(d) + " dBFS", 8, labelY)
                }
            }
        }

        function drawCenterLine(ctx) {
            var x = width / 2

            ctx.strokeStyle = "#305040"
            ctx.lineWidth = 1
            ctx.beginPath()
            ctx.moveTo(x, 0)
            ctx.lineTo(x, height)
            ctx.stroke()

            ctx.fillStyle = "#809080"
            ctx.font = "12px Consolas"
            ctx.fillText("CENTER", x + 6, 14)
        }

        function drawTrace(ctx) {
            if (!orion || !orion.receiver)
                return

            var bins = orion.receiver.spectrumBins
            if (!bins || bins.length < 2)
                return

            if (orion.displaySettings.peakHold) {
                if (peakBins.length !== bins.length)
                    peakBins = bins.slice()

                for (var p = 0; p < bins.length; ++p) {
                    if (bins[p] > peakBins[p])
                        peakBins[p] = bins[p]
                }

                ctx.strokeStyle = "#608060"
                ctx.lineWidth = 1
                ctx.beginPath()

                for (var h = 0; h < peakBins.length; ++h) {
                    var hx = h * width / (peakBins.length - 1)
                    var hy = dbToY(peakBins[h])

                    if (h === 0)
                        ctx.moveTo(hx, hy)
                    else
                        ctx.lineTo(hx, hy)
                }

                ctx.stroke()
            }

            ctx.strokeStyle = "#00FF66"
            ctx.lineWidth = 2
            ctx.beginPath()

            for (var i = 0; i < bins.length; ++i) {
                var x = i * width / (bins.length - 1)
                var y = dbToY(bins[i])

                if (i === 0)
                    ctx.moveTo(x, y)
                else
                    ctx.lineTo(x, y)
            }

            ctx.stroke()
        }

        function updateAutomaticScale() {
            if (!orion.displaySettings.autoScale || !orion || !orion.receiver)
                return

            var bins = orion.receiver.spectrumBins
            if (!bins || bins.length < 16)
                return

            // Sample the bins to keep this inexpensive. Sorting approximately
            // 512 values twice per second is negligible compared with the DSP.
            var sampled = []
            var step = Math.max(1, Math.floor(bins.length / 512))

            for (var i = 0; i < bins.length; i += step) {
                var value = Number(bins[i])
                if (isFinite(value))
                    sampled.push(value)
            }

            if (sampled.length < 16)
                return

            sampled.sort(function(a, b) { return a - b })

            // Robust percentiles prevent one transient spur from making the
            // entire display jump. The lower percentile estimates the floor;
            // the upper percentile estimates useful signal peaks.
            var noiseIndex = Math.floor((sampled.length - 1) * 0.20)
            var peakIndex = Math.floor((sampled.length - 1) * 0.995)
            var noiseFloorDb = sampled[noiseIndex]
            var peakDb = sampled[peakIndex]

            var targetTopDb = Math.ceil((peakDb + 8.0) / 10.0) * 10.0
            var targetBottomDb = Math.floor((noiseFloorDb - 12.0) / 10.0) * 10.0
            var targetRangeDb = Math.ceil((targetTopDb - targetBottomDb) / 10.0) * 10.0

            targetTopDb = Math.max(-100.0, Math.min(20.0, targetTopDb))
            targetRangeDb = Math.max(40.0, Math.min(140.0, targetRangeDb))

            // Ten-dB hysteresis avoids constant scale movement around a
            // threshold while still reacting promptly to genuine changes.
            if (Math.abs(targetTopDb - orion.displaySettings.spectrumOffsetDb) >= 10.0)
                orion.displaySettings.spectrumOffsetDb = targetTopDb

            if (Math.abs(targetRangeDb - orion.displaySettings.spectrumRangeDb) >= 10.0)
                orion.displaySettings.spectrumRangeDb = targetRangeDb
        }

        Timer {
            interval: 500
            repeat: true
            running: orion.displaySettings.autoScale
            onTriggered: spectrumCanvas.updateAutomaticScale()
        }

        Connections {
            target: (orion && orion.receiver) ? orion.receiver : null

            function onSpectrumBinsChanged() {
                spectrumCanvas.requestPaint()
            }
        }

        Connections {
            target: orion.displaySettings

            function onSpectrumOffsetDbChanged() {
                spectrumCanvas.requestPaint()
            }

            function onSpectrumRangeDbChanged() {
                spectrumCanvas.requestPaint()
            }

            function onAutoScaleChanged() {
                spectrumCanvas.requestPaint()
            }
        }
    }
}
