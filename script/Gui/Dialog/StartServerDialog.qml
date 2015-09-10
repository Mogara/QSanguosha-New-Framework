import QtQuick 2.4
import Cardirector.Device 1.0
import Sanguosha.Dialogs 1.0

StartServerDialog {
    Component.onCompleted: {
        createServer();
    }

    Image {
        source: config.backgroundImage
        anchors.fill: parent
    }

    TextEdit {
        id: logs
        anchors.fill: parent
        anchors.margins: Device.gu(20)
        readOnly: true
        color: "white"
        font.pixelSize: Device.gu(18)
    }

    onMessageLogged: logs.append(message);
}
