import QtQuick 2.4
import Cardirector.Device 1.0
import "../../engine.js" as Engine

Rectangle {
    property int cid: 0
    property string name: ""
    property string suit: ""
    property int number: 0

    property string icon: ""
    property alias text: textItem.text

    id: root
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
        glow.spread: 0.9
        glow.radius: 3
        glow.samples: 6
        anchors.left: iconItem.right
        anchors.leftMargin: Device.gu(3)
        anchors.right: numberItem.left
        anchors.rightMargin: Device.gu(4)
        horizontalAlignment: Text.AlignHCenter
    }

    GlowText {
        id: numberItem
        visible: number > 0 && number < 14
        text: Engine.convertNumber(number)
        font.weight: Font.Bold
        font.pixelSize: Device.gu(10)
        glow.color: "#FFFFBE"
        glow.spread: 0.75
        glow.radius: 2
        glow.samples: 4
        x: parent.width - Device.gu(24)
        y: Device.gu(1)
    }

    Image {
        id: suitItem
        anchors.right: parent.right
        source: suit ? "image://root/card/suit/" + suit : ""
        width: implicitWidth / implicitHeight * height
        height: parent.height
    }

    ParallelAnimation {
        id: showAnime

        NumberAnimation {
            target: root
            property: "x"
            duration: 200
            easing.type: Easing.InOutQuad
            from: Device.gu(10)
            to: 0
        }


        NumberAnimation {
            target: root
            property: "opacity"
            duration: 200
            easing.type: Easing.InOutQuad
            from: 0
            to: 1
        }
    }

    ParallelAnimation {
        id: hideAnime

        NumberAnimation {
            target: root
            property: "x"
            duration: 200
            easing.type: Easing.InOutQuad
            from: 0
            to: Device.gu(10)
        }


        NumberAnimation {
            target: root
            property: "opacity"
            duration: 200
            easing.type: Easing.InOutQuad
            from: 1
            to: 0
        }
    }

    function reset()
    {
        cid = 0;
        name = "";
        suit = "";
        number = 0;
        text = "";
    }

    function setCard(card)
    {
        cid = card.cid;
        name = card.name;
        suit = card.suit;
        number = card.number;
        text = qsTr(card.name + ":brief-description");
    }

    function show()
    {
        showAnime.start();
    }

    function hide()
    {
        hideAnime.start();
    }
}
