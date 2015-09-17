import QtQuick 2.4
import Cardirector.Device 1.0

Rectangle {
    property int minValue: 0
    property int maxValue: 100
    property int value: 50

    color: "#171512"
    border.color: "#A9A797"
    radius: Device.gu(5)
    clip: true

    Rectangle {
        width: parent.width / (maxValue - minValue) * (value - minValue)
        height: parent.height
        radius: parent.radius
        gradient: Gradient {
            GradientStop {position: 0; color: "#F2B8B7"}
            GradientStop {position: 0.2; color: "#DD0300"}
            GradientStop {position: 0.8; color: "#DD0300"}
            GradientStop {position: 1; color: "#9E0706"}
        }
    }
}
