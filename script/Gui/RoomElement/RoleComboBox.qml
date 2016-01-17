import QtQuick 2.4
import Cardirector.Device 1.0

Image {
    property string value: "unknown"
    property var options: ["unknown", "loyalist", "rebel", "renegade"]

    id: root
    source: visible ? "image://root/role/" + value : ""
    visible: value != "hidden"

    Image {
        property string value: "unknown"

        id: assumptionBox
        source: "image://root/role/" + value
        visible: root.value == "unknown"

        MouseArea {
            anchors.fill: parent
            onClicked: {
                console.log("clicked");
                optionPopupBox.visible = true;
            }
        }
    }

    Column {
        id: optionPopupBox
        visible: false
        spacing: Device.gu(2)

        Repeater {
            model: options

            Image {
                source: "image://root/role/" + modelData

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        optionPopupBox.visible = false;
                        assumptionBox.value = modelData;
                    }
                }
            }
        }
    }
}
