import QtQuick 2.4

Item {
    property string name
    property string mouseState: "normal"

    signal clicked
    signal doubleClicked

    id: button

    Image {
        source: "image://root/button/" + name + "/" + (enabled ? mouseState : "disabled")

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onClicked: button.clicked();
            onDoubleClicked: button.doubleClicked();
            onEntered: mouseState = "hover";
            onExited: mouseState = "normal";
            onPressed: mouseState = "down";
            onReleased: mouseState = containsMouse ? "hover" : "normal";
        }
    }
}
