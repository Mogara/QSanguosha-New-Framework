import QtQuick 2.4
import QtGraphicalEffects 1.0
import Cardirector.Device 1.0

Item {
    property alias title: titleItem
    signal finished()

    id: root

    Rectangle {
        id: background
        anchors.fill: parent
        color: "#B0000000"
        radius: Device.gu(5)
        border.color: "#A6967A"
        border.width: Device.gu(1)
    }

    DropShadow {
        source: background
        anchors.fill: background
        color: "#B0000000"
        radius: 5
        samples: 12
        spread: 0.2
        horizontalOffset: Device.gu(5)
        verticalOffset: Device.gu(4)
        transparentBorder: true
    }

    Text {
        id: titleItem
        color: "#E4D5A0"
        width: parent.width
        font.pixelSize: Device.gu(18)
        horizontalAlignment: Text.AlignHCenter
        x: Device.gu(6)
        y: Device.gu(6)

        transform: Scale {
            xScale: Math.min(1.0, (root.width - Device.gu(12)) / titleItem.width)
        }
    }

    MouseArea {
        anchors.fill: parent
        drag.target: parent
        drag.axis: Drag.XAndYAxis
    }
}
