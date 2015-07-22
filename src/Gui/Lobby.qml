import QtQuick 2.4
import QtQuick.Layouts 1.1
import Cardirector.Gui 1.0
import Cardirector.Device 1.0
import Sanguosha 1.0

Lobby {
    anchors.fill: parent

    onMessageLogged: chatLog.append(message);
    onRoomListCleared: roomList.clear();
    onRoomAdded: {
        var info = {
            rid: room.id,
            name: room.name,
            playerNum: room.playerNum,
            capacity: room.capacity
        };
        roomList.append(info);
    }
    Component.onCompleted: roomListUpdater.start();

    Timer {
        id: roomListUpdater
        interval: 10000
        repeat: true
        triggeredOnStart: true
        onTriggered: updateRoomList();
    }

    Rectangle {
        anchors.fill: parent
        color: "#696367"
    }

    ListModel {
        id: roomList
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: Device.gu(15)
        spacing: Device.gu(32)

        ListView {
            id: roomView
            Layout.preferredWidth: Device.gu(160)
            Layout.fillHeight: true
            x: Device.gu(10)
            spacing: Device.gu(10)
            model: roomList
            delegate: Component {
                TileButton {
                    backgroundColor: "#3A5D59"
                    border.color: Qt.rgba(0, 0, 0, 0)
                    text: ""
                    width: Device.gu(160)
                    height: Device.gu(60)

                    Item {
                        anchors.fill: parent
                        anchors.margins: Device.gu(5)

                        Text {
                            text: name
                            color: "#E1DF95"
                            font.pixelSize: Device.gu(20)
                        }

                        Text {
                            text: "<" + rid + ">"
                            color: "#D5D8D1"
                            font.pixelSize: Device.gu(15)
                            y: parent.height - height
                        }

                        Text {
                            text: playerNum + "/" + capacity
                            color: "#82906D"
                            font.pixelSize: Device.gu(18)
                            x: parent.width - width
                            y: parent.height - height
                        }
                    }
                }
            }
        }

        ColumnLayout {
            id: mainArea
            spacing: Device.gu(10)

            Item{
                height: Device.gu(30)
            }

            RowLayout {
                id: roomArea
                spacing: Device.gu(10)

                ColumnLayout {
                    spacing: 0

                    Rectangle {
                        color: "#BFBFBF"
                        Layout.fillWidth: true
                        height: Device.gu(60)

                        Canvas {
                            antialiasing: true
                            width: Device.gu(30)
                            height: Device.gu(21)
                            x: -width
                            y: parent.height - height

                            property color color: "#BFBFBF"
                            property real alpha: 1.0

                            onPaint: {
                                var ctx = getContext("2d");
                                ctx.save();
                                ctx.clearRect(0, 0, width, height);
                                ctx.fillStyle = color;
                                ctx.globalAlpha = alpha;
                                ctx.lineJoin = "round";
                                ctx.beginPath();
                                ctx.moveTo(0, 0);
                                ctx.lineTo(width, 2);
                                ctx.lineTo(width, height);
                                ctx.closePath();
                                ctx.fill();
                                ctx.restore();
                            }
                        }

                        Image{
                            id: roomLogo
                            source: currentRoomLogo
                            width: Device.gu(100)
                            height: Device.gu(100)
                            x: Device.gu(20)
                            y: -height / 2
                        }

                        Text {
                            text: currentRoomName
                            anchors.left: roomLogo.right
                            anchors.bottom: parent.bottom
                            anchors.leftMargin: Device.gu(10)
                            anchors.bottomMargin: Device.gu(10)
                            font.pixelSize: Device.gu(26)
                            color: "#695F5E"
                            verticalAlignment: Text.AlignBottom
                        }
                    }

                    Rectangle {
                        color: "#DDDDDB"
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        TextEdit {
                            id: chatLog
                            anchors.fill: parent
                            anchors.margins: Device.gu(20)
                            font.pixelSize: Device.gu(20)
                            clip: true
                            readOnly: true
                            selectByKeyboard: true
                            selectByMouse: true
                            wrapMode: TextEdit.WordWrap
                        }
                    }

                    Rectangle {
                        color: "#BFBFBF"
                        Layout.fillWidth: true
                        height: Device.gu(60)
                    }
                }

                ColumnLayout {
                    id: roomInfo
                    Layout.fillWidth: false
                    Layout.preferredWidth: Device.gu(200)
                    spacing: 0

                    Rectangle {
                        color: "#BFBFBF"
                        Layout.fillWidth: true
                        height: Device.gu(60)

                        Text {
                            text: qsTr("Information")
                            anchors.fill: parent
                            anchors.leftMargin: Device.gu(10)
                            anchors.bottomMargin: Device.gu(10)
                            font.pixelSize: Device.gu(26)
                            color: "#695F5E"
                            verticalAlignment: Text.AlignBottom
                        }
                    }

                    Rectangle {
                        color: "#DDDDDB"
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }

                    Rectangle {
                        color: "#BFBFBF"
                        Layout.fillWidth: true
                        height: Device.gu(60)

                        MetroButton {
                            width: Device.gu(80)
                            height: Device.gu(36)
                            backgroundColor: "#A46061"
                            anchors.bottom: parent.bottom
                            anchors.right: parent.right
                            anchors.bottomMargin: Device.gu(10)
                            anchors.rightMargin: Device.gu(10)
                            text: qsTr("Join")
                            textColor: "#EDC5C5"
                            textFont.pixelSize: Device.gu(18)
                            border.width: 0
                        }
                    }
                }
            }

            RowLayout {
                id: controlPanel
                Layout.fillHeight: false
                Layout.preferredHeight: Device.gu(150)
                spacing: Device.gu(10)

                ColumnLayout {
                    id: buttonArea
                    Layout.fillWidth: false
                    Layout.preferredWidth: Device.gu(140)
                    spacing: Device.gu(10)

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: Device.gu(30)
                        color: "#F6F6F6"

                        TextInput {
                            anchors.fill: parent
                            color: "#000000"
                            font.pixelSize: Device.gu(16)
                            wrapMode: TextInput.Wrap
                            inputMethodHints: Qt.ImhDigitsOnly
                            verticalAlignment: TextInput.AlignVCenter
                            horizontalAlignment: TextInput.AlignHCenter
                            validator: IntValidator {
                                bottom: 1
                            }
                        }
                    }

                    MetroButton {
                        Layout.fillWidth: true
                        Layout.preferredHeight: Device.gu(45)
                        backgroundColor: "#455473"
                        textColor: "#D2BDFF"
                        text: qsTr("Join")
                        textFont.pixelSize: Device.gu(28)
                        border.width: 0
                    }

                    MetroButton {
                        Layout.fillWidth: true
                        Layout.preferredHeight: Device.gu(55)
                        backgroundColor: "#A46061"
                        textColor: "#EDC5C5"
                        text: qsTr("Create")
                        textFont.pixelSize: Device.gu(28)
                        border.width: 0
                        onClicked: createRoom();
                    }
                }

                Rectangle {
                    id: chatBox
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "#DDDDDB"

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: Device.gu(10)
                        spacing: Device.gu(10)

                        ColumnLayout {
                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: Device.gu(32)
                                color: "#F6F6F6"

                                TextInput {
                                    id: chatInput
                                    anchors.fill: parent
                                    anchors.margins: Device.gu(4)
                                    color: "#000000"
                                    font.pixelSize: Device.gu(16)
                                    wrapMode: TextInput.Wrap
                                    verticalAlignment: TextInput.AlignVCenter
                                    clip: true
                                    onAccepted: send();

                                    function send(){
                                        speakToServer(text);
                                        text = "";
                                    }
                                }

                                Image {
                                    source: "image://mogara/enter_icon"
                                    width: Device.gu(34)
                                    height: Device.gu(22)
                                    x: parent.width - width
                                    y: parent.height + Device.gu(5)

                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: chatInput.send();
                                    }
                                }
                            }

                            Item {
                                Layout.fillHeight: true
                            }
                        }

                        ColumnLayout {
                            Layout.fillWidth: false
                            Layout.preferredWidth: Device.gu(110)

                            Rectangle {
                                Layout.preferredWidth: Device.gu(110)
                                Layout.preferredHeight: Device.gu(110)
                                color: "white"

                                Image {
                                    anchors.fill: parent
                                    source: userAvatar
                                }
                            }

                            Text {
                                Layout.fillWidth: true
                                Layout.preferredHeight: Device.gu(16)
                                text: userName
                                font.pixelSize: Device.gu(16)
                                color: "#535351"
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }
                    }
                }
            }
        }
    }
}
