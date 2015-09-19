import QtQuick 2.4
import QtQuick.Layouts 1.1
import Cardirector.Device 1.0
import Cardirector.Resource 1.0

RowLayout {
    property alias headGeneralName: headGeneralItem.generalName
    property alias headGeneralAvatar: headGeneralItem.avatar
    property alias headGeneralKingdom: headGeneralItem.kingdom
    property alias deputyGeneralName: deputyGeneralItem.generalName
    property alias deputyGeneralAvatar: deputyGeneralItem.avatar
    property alias deputyGeneralKingdom: deputyGeneralItem.kingdom
    property int seatNumber: 0
    property string userRole: "unknown"
    property alias hp: hpBar.value
    property alias maxHp: hpBar.maxValue
    property string phase: "not_active"

    property alias acceptButton: acceptButtonItem
    property alias rejectButton: rejectButtonItem
    property alias finishButton: finishButtonItem
    property alias handcardArea: handcardAreaItem
    property alias equipArea: equipAreaItem
    property alias progressBar: progressBarItem

    signal accepted()
    signal rejected()
    signal finished()

    id: root
    spacing: 0
    Layout.fillHeight: false
    Layout.preferredHeight: Device.gu(150)

    ImageProvider {
        id: roomSceneImage
        providerId: "dashboard"

        function imagePath(imageId, requestedSize)
        {
            return "image/dashboard/" + imageId + ".png";
        }
    }

    Image {
        id: equipAreaItem
        source: "image://dashboard/equip"
        Layout.preferredWidth: Device.gu(164)
        Layout.fillHeight: true
    }

    Image {
        source: "image://dashboard/hand"
        Layout.fillWidth: true
        Layout.fillHeight: true

        RowLayout {
            anchors.fill: parent

            RowLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true

                HandcardArea {
                    id: handcardAreaItem
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.topMargin: Device.gu(5)

                    ProgressBar {
                        id: progressBarItem
                        width: parent.width * 0.4
                        height: 15
                        y: -height - Device.gu(10)
                        x: (parent.width - width) / 2
                        visible: false
                    }

                    Image {
                        source: root.phase != "not_active" ? "image://root/phase/" + root.phase + ".png" : ""
                        y: -height - Device.gu(5)
                        x: parent.width - width
                        visible: root.phase != "not_active"
                    }

                    Connections {
                        target: root
                        onPhaseChanged: handcardArea.enableCards([]);
                    }
                }

                Item {
                    Layout.preferredWidth: Device.gu(20)
                    Layout.fillHeight: true

                    HandcardNumber {
                        id: handcardNumItem
                        kingdom: headGeneralKingdom
                        value: handcardArea.length
                        y: parent.height - height - Device.gu(5)
                    }
                }
            }

            Image {
                id: platter
                source: "image://dashboard/platter"

                IrregularButton {
                    id: acceptButtonItem
                    name: "platter/confirm"
                    enabled: false
                    x: Device.gu(6)
                    y: Device.gu(3)

                    onClicked: root.accepted();
                }

                IrregularButton {
                    id: rejectButtonItem
                    name: "platter/cancel"
                    enabled: false
                    x: Device.gu(6)
                    y: Device.gu(79)

                    onClicked: root.rejected();
                }

                IrregularButton {
                    id: finishButtonItem
                    name: "platter/discard"
                    enabled: false
                    x: Device.gu(67)
                    y: Device.gu(37)

                    onClicked: root.finished();
                }

                Image {
                    source: "image://root/role/" + userRole
                    x: Device.gu(70)
                    y: Device.gu(3)
                }

                Image {
                    x: Device.gu(71)
                    y: Device.gu(117)
                    source: seatNumber > 0 ? "image://dashboard/seatnum/" + seatNumber : ""
                    visible: seatNumber > 0
                }
            }
        }
    }

    Item {
        width: Device.gu(283)
        height: Device.gu(149)

        Image {
            source: "image://dashboard/base"
        }

        Image {
            source: "image://dashboard/hpbase"
            anchors.bottom: parent.bottom
            anchors.left: deputyGeneralItem.right

            HpBar {
                id: hpBar
                visible: false
                anchors.bottom: parent.bottom
                transform: Scale {
                    xScale: hpBar.parent.width / hpBar.width
                    yScale: xScale
                }
            }
        }

        Image {
            source: "image://dashboard/avatarbg"
        }

        GeneralAvatar {
            id: headGeneralItem
            y: -Device.gu(4)
            avatar: "huangyueying"
            generalPosition: "head"
        }

        GeneralAvatar {
            id: deputyGeneralItem
            x: Device.gu(128)
            y: -Device.gu(4)
            avatar: "zhugeliang"
            generalPosition: "deputy"
        }
    }
}
