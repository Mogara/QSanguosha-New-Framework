import QtQuick 2.4
import Cardirector.Device 1.0

Image {
    property alias text: content.text

    width: Device.gu(480)
    height: Device.gu(200)
    source: "image://root/prompt"

    signal finished()

    Text {
        id: content
        color: "white"
        x: Device.gu(30)
        y: Device.gu(35)
        width: parent.width - x * 2
        font.family: "LiSu"
        font.pixelSize: Device.gu(22)
        wrapMode: Text.WordWrap
    }
}
