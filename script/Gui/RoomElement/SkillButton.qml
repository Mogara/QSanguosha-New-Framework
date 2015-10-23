import QtQuick 2.4
import Cardirector.Device 1.0

Image {
    property string name
    property string type: "proactive"
    property int columns: 1

    readonly property string status: mouseArea.containsMouse ? (mouseArea.pressed ? "down" : "hover") : "normal"

    width: Device.gu(columns === 1 ? 120 : 59)
    height: Device.gu(26)
    source: "image://root/skill/button/" + type + "/" + columns + "-" + (enabled ? status : "disabled")
    clip: true

    GlowText {
        text: qsTr(name)
        color: "white"
        font.family: "楷体"
        font.pixelSize: Device.gu(14)
        font.weight: Font.Black
        font.letterSpacing: columns === 1 ? Device.gu(20) : 0
        horizontalAlignment: Text.AlignHCenter
        x: columns === 1 ? Device.gu(26) : Device.gu(20)
        y: Device.gu(6)
        width: parent.width - Device.gu(25)
        style: Text.Outline
        styleColor: Qt.rgba(255, 255, 255, 0.1)
        glow.color: Qt.rgba(0, 0, 0, 0.65)
        glow.radius: 1
        glow.spread: 1
        glow.samples: 2
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
    }
}
