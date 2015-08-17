import QtQuick 2.0
import Cardirector.Device 1.0

Item {
    Rectangle {
        color: "#2779AA"
        width: parent.width / 2
        height: parent.height / 2
        border.color: "black"
        border.width: Device.gu(1)
    }

    Rectangle {
        color: "#C3B154"
        width: parent.width / 2
        height: parent.height / 2
        border.color: "black"
        border.width: Device.gu(1)
        x: parent.width / 2
    }

    Rectangle {
        color: "#AC3120"
        width: parent.width / 2
        height: parent.height / 2
        border.color: "black"
        border.width: Device.gu(1)
        y: parent.height / 2
    }

    Rectangle {
        color: "#55A434"
        width: parent.width / 2
        height: parent.height / 2
        border.color: "black"
        border.width: Device.gu(1)
        x: parent.width / 2
        y: parent.height / 2
    }
}
