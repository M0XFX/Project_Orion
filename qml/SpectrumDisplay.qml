import QtQuick

Rectangle {
    color: "black"
    border.color: "#404040"
    border.width: 1

    Canvas {
        id: spectrumCanvas
        anchors.fill: parent
        anchors.margins: 8

        property real minDb: -100
        property real maxDb: 40

        property var peakBins: []



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

            if (y < 0)
                y = 0

            if (y > height)
                y = height

            return y
        }




        function drawGrid(ctx) {
            ctx.strokeStyle = "#1f3a2f"
            ctx.lineWidth = 1

            for (var i = 1; i < 10; i++) {
                var x = i * width / 10
                ctx.beginPath()
                ctx.moveTo(x, 0)
                ctx.lineTo(x, height)
                ctx.stroke()
            }

            for (var j = 1; j < 6; j++) {
                var y = j * height / 6
                ctx.beginPath()
                ctx.moveTo(0, y)
                ctx.lineTo(width, y)
                ctx.stroke()
            }

            ctx.fillStyle = "#607060"
            ctx.font = "12px Consolas"

            if (orion.displaySettings.showDbScale) {
                ctx.fillStyle = "#607060"
                ctx.font = "12px Consolas"

                for (var d = minDb; d <= maxDb; d += 20) {
                    var yy = dbToY(d)
                    ctx.fillText(d + " dB", 8, yy - 3)
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

                for (var p = 0; p < bins.length; p++) {
                    if (bins[p] > peakBins[p])
                        peakBins[p] = bins[p]
                }

                ctx.strokeStyle = "#608060"
                ctx.lineWidth = 1
                ctx.beginPath()

                for (var h = 0; h < peakBins.length; h++) {
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

            for (var i = 0; i < bins.length; i++) {
                var x = i * width / (bins.length - 1)
                var y = dbToY(bins[i])

                if (i === 0)
                    ctx.moveTo(x, y)
                else
                    ctx.lineTo(x, y)
            }

            ctx.stroke()
        }


        Connections {
            target: (orion && orion.receiver) ? orion.receiver : null

            function onSpectrumBinsChanged() {
                spectrumCanvas.requestPaint()
            }
        }





    }
}