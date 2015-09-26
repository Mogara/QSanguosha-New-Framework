import QtQuick 2.4
import Cardirector.Gui 1.0
import Cardirector.Device 1.0
import "../Client"

Image {
    id: startScene
    source: config.backgroundImage
    anchors.fill: parent

    property bool isFeteDay: isInFeteDays()

    Image {
        id: logo
        source: "image://system/mogara/logo" + (isFeteDay ? "-moxuan" : "")
        anchors.centerIn: parent
        anchors.horizontalCenterOffset: -parent.width / 4
        opacity: 0

        ToolTipArea {
            text: isFeteDay ? qsTr("At 10:40 a.m., August 19, 2014, Moxuanyanyun, a developer of QSanguosha, passed away peacefully in Dalian Medical College. He was 18 and had struggled with leukemia for more than 4 years. May there is no pain in Heaven.") : ""
        }
    }

    NumberAnimation {
        id: logoAni
        target: logo
        property: "opacity"
        duration: 1000
        to: 1
        onStopped: grid.model = buttons;
    }

    GridView {
        id: grid
        interactive: false
        anchors.centerIn: parent
        anchors.horizontalCenterOffset: parent.width / 4
        flow: GridView.FlowTopToBottom
        cellHeight: Device.gu(162); cellWidth: Device.gu(162)
        height: cellHeight * 4; width: cellWidth * 2
        delegate: buttonDelegate
    }

    Component {
        id: buttonDelegate

        TileButton {
            text: name
            iconSource: "image://system/tileicon/" + icon
            onClicked: {
                var dialog = icon.substr(0, 1).toUpperCase() + icon.substr(1);
                dialog = dialog.replace(/\_([a-z])/g, function(str, group1){
                    return group1.toUpperCase();
                });
                dialogLoader.setSource("Dialog/" + dialog + "Dialog.qml");
            }
        }
    }

    ListModel {
        id: buttons
        ListElement { name: qsTr("Start Game"); icon: "start_game" }
        ListElement { name: qsTr("Start Server"); icon: "start_server" }
        ListElement { name: qsTr("PC Console Start"); icon: "pc_console_start" }
        ListElement { name: qsTr("Replay"); icon: "replay" }
        ListElement { name: qsTr("Configure"); icon: "configure" }
        ListElement { name: qsTr("General Overview"); icon: "general_overview" }
        ListElement { name: qsTr("Card Overview"); icon: "card_overview" }
        ListElement { name: qsTr("About"); icon: "about" }
    }

    Timer {
        interval: 1000
        running: true
        onTriggered: logoAni.start()
    }

    function isInFeteDays() {
        var date = new Date();
        if (date.getMonth() == 7 && date.getDate() >= 19 && date.getDate() <= 26) {
            return true;
        }
        return false;
    }
}

