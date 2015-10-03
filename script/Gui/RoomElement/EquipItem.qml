import QtQuick 2.4
import Cardirector.Device 1.0
import "../../engine.js" as Engine

Item {
    property string name: null
    property string suit: null
    property int number: null

    id: root
    width: Device.gu(145)
    height: Device.gu(22)
    opacity: 0

    Image {
        source: name ? "image://root/card/equip/" + name : ""
    }

    Image {
        source: suit ? "image://root/card/suit/" + suit : ""
        x: parent.width - width
        y: Math.round((parent.height - height) / 2)
    }

    GlowText {
        text: number ? (Engine.convertNumber(number)) : ""
        font.pixelSize: Device.gu(14)
        x: parent.width - Device.gu(28)
        y: Device.gu(2)

        glow.color: "#CFCFCD"
        glow.radius: 1
        glow.spread: 1
        glow.samples: 2
    }

    ParallelAnimation {
        id: showAnime

        NumberAnimation {
            target: root
            property: "opacity"
            duration: 500
            easing.type: Easing.InOutQuad
            from: 0
            to: 1
        }


        NumberAnimation {
            target: root
            property: "x"
            duration: 500
            easing.type: Easing.InOutQuad
            from: Device.gu(15)
            to: 0
        }
    }

    ParallelAnimation {
        id: hideAnime

        NumberAnimation {
            target: root
            property: "opacity"
            duration: 500
            easing.type: Easing.InOutQuad
            from: 1
            to: 0
        }


        NumberAnimation {
            target: root
            property: "x"
            duration: 500
            easing.type: Easing.InOutQuad
            from: 0
            to: Device.gu(10)
        }
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
