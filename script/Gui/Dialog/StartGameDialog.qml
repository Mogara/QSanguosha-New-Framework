import QtQuick 2.5
import QtQuick.Layouts 1.2
import Cardirector.Gui 1.0
import Cardirector.Device 1.0
import Sanguosha.Dialogs 1.0

StartGameDialog {
    id: startGameDialog

    signal accepted
    signal rejected

    property alias serverAddress: serverAddressInput.text
    property alias screenName: screenNameInput.text

    onLobbyEntered: dialogLoader.setSource("../Lobby.qml");

    onAccepted: {
        signup(screenName, "");
        connectToServer(serverAddress);
    }

    onRejected: dialogLoader.source = "";

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
                text: qsTr("Server Address")
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
                    id: serverAddressInput
                    anchors.fill: parent
                    anchors.margins: Device.gu(11)
                    font.pixelSize: Device.gu(20)
                    color: "white"
                    clip: true
                    text: "127.0.0.1:5927"
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
                width: Device.gu(160)
                height: Device.gu(50)
                text: qsTr("Connect")
                onClicked: startGameDialog.accepted();
            }
            MetroButton {
                width: Device.gu(160)
                height: Device.gu(50)
                text: qsTr("Cancel")
                onClicked: startGameDialog.rejected();
            }
        }
    }
}
