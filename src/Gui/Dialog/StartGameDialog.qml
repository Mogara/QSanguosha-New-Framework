import QtQuick 2.5
import QtQuick.Layouts 1.2
import Cardirector.Gui 1.0
import Cardirector.Device 1.0
import Sanguosha.Dialogs 1.0

StartGameDialog {

    onServerConnected: dialogLoader.setSource("../Lobby.qml");

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
                text: qsTr("Server IP")
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
                    id: serverIpInput
                    anchors.fill: parent
                    anchors.margins: Device.gu(11)
                    font.pixelSize: Device.gu(20)
                    color: "white"
                    clip: true
                    text: "127.0.0.1"
                }
            }

            Text {
                color: "white"
                text: qsTr("Server Port")
                font.pixelSize: Device.gu(20)
                width: Device.gu(50)
                height: Device.gu(22)
            }

            Rectangle {
                color: Qt.rgba(0xFF, 0xFF, 0xFF, 0.3)
                Layout.fillWidth: true
                height: Device.gu(42)
                TextInput {
                    id: serverPortInput
                    anchors.fill: parent
                    anchors.margins: Device.gu(11)
                    font.pixelSize: Device.gu(20)
                    color: "white"
                    clip: true
                    text: "5927"
                    validator: IntValidator {
                        bottom: 1
                        top: 65535
                    }
                }
            }

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
                }
            }
        }

        ColumnLayout {
            spacing: Device.gu(20)

            MetroButton {
                text: qsTr("Connect")
                onClicked: {
                    signup(screenNameInput.text, "");
                    connectToServer(serverIpInput.text, parseInt(serverPortInput.text, 10));
                }
            }
            MetroButton {
                text: qsTr("Cancel")
                onClicked: dialogLoader.sourceComponent = undefined;
            }
        }
    }
}
