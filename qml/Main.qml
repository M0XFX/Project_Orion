import QtQuick
import QtQuick.Controls

ApplicationWindow {

    visible: true

    width: 1200
    height: 760

    title: qsTr("Project Orion")

    color: "#101010"

    FrequencyDisplay {

        id: frequency

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        anchors.margins: 10

        height: 90
    }

    SpectrumDisplay {

        id: spectrum

        anchors.top: frequency.bottom
        anchors.topMargin: 10

        anchors.left: parent.left
        anchors.right: parent.right

        anchors.leftMargin: 10
        anchors.rightMargin: 10

        height: 260
    }

    WaterfallDisplay {

        id: waterfall

        anchors.top: spectrum.bottom
        anchors.topMargin: 10

        anchors.left: parent.left
        anchors.right: parent.right

        anchors.leftMargin: 10
        anchors.rightMargin: 10

        height: 250
    }

    ControlBar {

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        anchors.margins: 10

        height: 70
    }

}