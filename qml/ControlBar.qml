import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {

    color: "#222222"
    border.color: "#404040"
    border.width: 1

    RowLayout {

        anchors.fill: parent
        anchors.margins: 6
        spacing: 6

        Button {
            text: "USB"
            onClicked: orion.radio.mode = "USB"
        }
        Button {
            text: "LSB"
            onClicked: orion.radio.mode = "LSB"
        }
        Button {
            text: "CW"
            onClicked: orion.radio.mode = "CW"
        }
        Button {
            text: "AM"
            onClicked: orion.radio.mode = "AM"
        }
        Button {
            text: "FM"
            onClicked: orion.radio.mode = "FM"
        }
        Item {
            Layout.fillWidth: true
        }
        Button { text: "MENU" }
    }

}
