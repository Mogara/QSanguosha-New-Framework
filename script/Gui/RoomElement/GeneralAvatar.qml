import QtQuick 2.4
import Cardirector.Device 1.0

Item {
    property string avatar: ""
    property string kingdom: ""
    property string generalPosition: "head"
    property alias generalName: nameItem.text

    width: Device.gu(128)
    height: Device.gu(153)

    Image {
        source: "image://root/general/fullphoto/" + avatar
        anchors.fill: parent
    }

    Image {
        source: "image://root/dashboard/avatar"
        anchors.fill: parent
    }

    Image {
        x: Device.gu(2)
        y: Device.gu(2)
        source: "image://root/dashboard/title/" + (kingdom != "" && kingdom != "hidden" ? kingdom : "god")
        visible: kingdom != ""

        Item {
            x: nameItem.text.length >= 3 ? Device.gu(26) : Device.gu(28)

            GlowText {
                id: nameItem
                font.family: "LiSu"
                font.pixelSize: Device.gu(18)
                color: "white"
                font.letterSpacing: text.length < 3 ? 9 : 0

                glow.spread: 0.9
                glow.radius: Device.gu(0.9)
                glow.samples: 4
                glow.color: "#222222"
            }

            transform: Scale {
                xScale: Math.min(1.0, 57 / nameItem.width)
            }
        }
    }

    Image {
        source: "image://root/dashboard/" + generalPosition
    }
}
