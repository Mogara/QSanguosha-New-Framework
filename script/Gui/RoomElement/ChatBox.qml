import QtQuick 2.4
import QtQuick.Layouts 1.1
import Cardirector.Gui 1.0
import Cardirector.Device 1.0

Rectangle {
    color: Qt.rgba(0, 0, 0, 0.6)

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            LogEdit {
                id: chatLogBox
                anchors.fill: parent
                anchors.margins: Device.gu(10)
                font.pixelSize: Device.gu(14)
                color: "white"
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: Device.gu(28)
            color: "#040403"
            radius: Device.gu(3)
            border.width: Device.gu(1)
            border.color: "#A6967A"

            TextInput {
                anchors.fill: parent
                anchors.margins: Device.gu(6)
                color: "white"
                clip: true
                font.pixelSize: Device.gu(14)

                onAccepted: {
                    if (text != "") {
                        chatLogBox.append(text);
                        text = "";
                    }
                }
            }
        }
    }
}
