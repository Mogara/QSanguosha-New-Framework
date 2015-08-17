import QtQuick 2.4
import QtGraphicalEffects 1.0
import Cardirector.Device 1.0

Item {
    property string avatar: ""
    property string kingdom: ""
    property string generalPosition: "head"
    property alias generalName: nameItem.text

    width: Device.gu(128)
    height: Device.gu(153)

    Image {
        source: "image://general/fullphoto/" + avatar
        anchors.fill: parent
    }

    Image {
        source: "image://dashboard/avatar"
        anchors.fill: parent
    }

    Image {
        x: Device.gu(2)
        y: Device.gu(2)
        source: "image://dashboard/title/" + (kingdom != "" ? kingdom : "god")
        visible: kingdom != ""

        Item {
            x: nameItem.text.length >= 3 ? Device.gu(26) : Device.gu(28)

            Text {
                id: nameItem
                font.family: "隶书"
                font.pixelSize: Device.gu(18)
                color: "white"
                font.letterSpacing: text.length < 3 ? 9 : 0
            }

            Glow {
                source: nameItem
                anchors.fill: nameItem
                spread: 0.9
                radius: Device.gu(0.9)
                samples: 4
                color: "#222222"
            }

            transform: Scale {
                xScale: Math.min(1.0, 57 / nameItem.width)
            }
        }
    }

    Image {
        source: "image://dashboard/" + generalPosition
    }
}
