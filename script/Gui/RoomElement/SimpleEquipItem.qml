import QtQuick 2.4
import Cardirector.Device 1.0
import "../../engine.js" as Engine

Rectangle {
    property string icon: ""
    property string suit: ""
    property int number: 0
    property alias text: textItem.text

    border.color: "#88767676"
    color: Qt.rgba(255, 255, 255, 0.5)

    Image {
        id: iconItem
        anchors.verticalCenter: parent.verticalCenter
        x: 1

        source: "image://root/card/equip/icon/background"
        Image {
            source: icon ? "image://root/card/equip/icon/" + icon : ""
            anchors.centerIn: parent
        }
    }

    GlowText {
        id: textItem
        font.family: "LiSu"
        font.pixelSize: Device.gu(14)
        glow.color: "#FFFFBE"
        glow.spread: 0.7
        glow.radius: 8
        glow.samples: 16
        anchors.left: iconItem.right
    }

    GlowText {
        id: numberItem
        visible: number > 0 && number < 14
        text: Engine.convertNumber(number)
        glow.color: "#CFCFCD"
        glow.spread: 1
        glow.radius: 1
        glow.samples: 2
        anchors.right: suitItem.left
    }

    Image {
        id: suitItem
        anchors.right: parent.right
        source: suit ? "image://root/card/suit/" + suit : ""
        width: implicitWidth / implicitHeight * height
        height: parent.height
    }
}
