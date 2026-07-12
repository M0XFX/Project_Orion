import QtQuick

Rectangle {

    color: "#181818"
    border.color: "#404040"
    border.width: 1
    radius: 6

    Text {
        anchors.centerIn: parent

        //text: "14.074.000 MHz"

        //text: orion.radio.frequency
        text: orion && orion.radio ? orion.radio.frequency : "No Radio"

        color: "#00FF66"

        font.family: "Consolas"
        font.pixelSize: 48
        font.bold: true
    }

    Text {
        anchors.right: parent.right
        anchors.rightMargin: 30
        anchors.verticalCenter: parent.verticalCenter

        //text: orion.radio.mode
        text: orion && orion.radio ? orion.radio.mode : "---"
        color: "#E0E0E0"
        font.pixelSize: 28
        font.bold: true
    }

    Text {
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.top: parent.top
        anchors.topMargin: 12

        text: orion && orion.receiver ? orion.receiver.status : "Receiver stopping..."

        color: "#A0A0A0"

        font.pixelSize: 18
    }



}
