import QtQuick 2.4
import QtQuick.Layouts 1.2
import Cardirector.Gui 1.0
import Cardirector.Device 1.0
import Sanguosha.Dialogs 1.0

PcConsoleStartDialog {

    id: root

    Image {
        source: config.backgroundImage
        anchors.fill: parent
    }

    RowLayout {
        width: Device.gu(600)
        height: Device.gu(270)
        anchors.centerIn: parent
        spacing: Device.gu(22)

        GridLayout {
            columns: 2

            Text {
                text: qsTr("Screen Name")
                color: "white"
                font.pixelSize: Device.gu(20)
                width: Device.gu(50)
                height: Device.gu(22)
            }

            Rectangle {
                color: Qt.rgba(0xFF, 0xFF, 0xFF, 0.3)
                Layout.fillWidth: true
                height: Device.gu(42)

                TextInput {
                    id: screenNameInput
                    anchors.fill: parent
                    anchors.margins: Device.gu(11)
                    font.pixelSize: Device.gu(20)
                    color: "white"
                    clip: true

                    Component.onCompleted: forceActiveFocus();
                }
            }
        }

        ColumnLayout {
            spacing: Device.gu(20)

            MetroButton {
                width: Device.gu(160)
                height: Device.gu(50)
                text: qsTr("Create")
                onClicked: start(screenNameInput.text, "");
            }
            MetroButton {
                width: Device.gu(160)
                height: Device.gu(50)
                text: qsTr("Cancel")
                onClicked: dialogLoader.source = "";
            }
        }
    }

    onEnterLobby: dialogLoader.setSource("../Lobby.qml");
}
