import QtQuick 2.4
import Cardirector.Device 1.0
import QtGraphicalEffects 1.0

Image {
    property string kingdom
    property int value

    source: "image://root/roomscene/handcard/" + (kingdom != "" ? kingdom : "qun")

    Text {
        id: handcardNumText
        anchors.fill: parent
        color: "white"
        text: value
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    Glow {
        anchors.fill: handcardNumText
        spread: 0.7
        radius: Device.gu(4.0)
        samples: 4
        color: "#000"
        source: handcardNumText
    }
}
