import QtQuick 2.4
import Cardirector.Device 1.0

Item {
    property string avatar: ""
    property string kingdom: ""
    property alias name: nameItem

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

        Text {
            id: nameItem
            font.family: "隶书"
            font.pixelSize: Device.gu(18)
            color: "white"
        }
    }
}
