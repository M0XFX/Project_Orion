import QtQuick
import HF_SDR_Display

Rectangle {
    color: "black"
    border.color: "#404040"
    border.width: 1

    SpectrumView {
        anchors.fill: parent
        anchors.margins: 8

        receiver: orion ? orion.receiver : null
        displaySettings: orion ? orion.displaySettings : null
    }
}
