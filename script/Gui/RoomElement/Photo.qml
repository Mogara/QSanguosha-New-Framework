import QtQuick 2.4
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import Cardirector.Device 1.0
import Cardirector.Resource 1.0

Item {
    property string headGeneral: ""
    property string deputyGeneral: ""
    property alias screenName: screenNameItem.text
    property alias faceTurned: faceTurnedCover.visible
    property string userRole: "unknown"
    property alias handcardNum: handcardNumItem.value
    property alias maxHp: hpBar.maxValue
    property alias hp: hpBar.value
    property alias handcardArea: handcardAreaItem
    property string phase: "inactive"
    property alias progressBar: progressBarItem
    property int seat: 0
    property bool selectable: false
    property bool selected: false

    id: root
    width: Device.gu(157)
    height: Device.gu(181)
    states: [
        State {
            name: "normal"
            PropertyChanges {
                target: outerGlow
                visible: false
            }
        },
        State {
            name: "candidate"
            PropertyChanges {
                target: outerGlow
                color: "#EEB300"
                visible: root.selectable && root.selected
            }
        },
        State {
            name: "playing"
            PropertyChanges {
                target: outerGlow
                color: "#BE85EE"
                visible: true
            }
        },
        State {
            name: "responding"
            PropertyChanges {
                target: outerGlow
                color: "#51D659"
                visible: true
            }
        },
        State {
            name: "sos"
            PropertyChanges {
                target: outerGlow
                color: "#ED8B96"
                visible: true
            }
        }
    ]
    state: "normal"

    ImageProvider {
        id: photoImage
        providerId: "photo"

        function imagePath(imageId, requestedSize)
        {
            return "image/photo/" + imageId + ".png";
        }
    }

    RectangularGlow {
        id: outerGlow
        anchors.fill: parent
        visible: true
        glowRadius: 8
        spread: 0.4
        cornerRadius: 8
    }

    Item {
        width: deputyGeneral != "" ? Device.gu(75) : Device.gu(155)
        height: Device.gu(182)

        Image {
            anchors.fill: parent
            fillMode: Image.PreserveAspectCrop
            source: "image://general/fullphoto/" + (headGeneral != "" ? headGeneral : "blank")
        }
    }

    Item {
        x: Device.gu(80)
        width: Device.gu(75)
        height: Device.gu(182)

        Image {
            anchors.fill: parent
            fillMode: Image.PreserveAspectCrop
            source: deputyGeneral != "" ? "image://general/fullphoto/" + deputyGeneral : ""
        }
    }

    Image {
        source: "image://photo/circle-photo"
        visible: deputyGeneral != ""
    }

    Image {
        id: faceTurnedCover
        anchors.fill: parent
        source: "image://photo/faceturned"
        visible: false
    }

    Image {
        anchors.fill: parent
        source: "image://photo/photo-back"
    }

    HandcardNumber {
        id: handcardNumItem
        x: Device.gu(-10)
        y: Device.gu(110)
        kingdom: parent.userRole
        value: handcardArea.length
    }

    Item {
        width: Device.gu(17)
        height: Device.gu(maxHp > 5 ? 72 : 6 + 15 * maxHp)
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Device.gu(1)
        anchors.rightMargin: Device.gu(2)
        clip: true

        Image {
            source: "image://root/magatama/bg"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            visible: hpBar.visible
        }

        HpBar {
            id: hpBar
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: Device.gu(3)
            visible: maxHp > 0

            transform: Scale {
                origin.x: hpBar.width / 2
                origin.y: hpBar.height
                xScale: Device.gu(15) / hpBar.width
                yScale: xScale
            }
        }
    }

    Text {
        id: screenNameItem
        color: "white"
        font.pixelSize: Device.gu(12)
        x: Device.gu(5)
        y: Device.gu(3)
    }

    GlowText {
        id: headGeneralNameItem
        color: "white"
        y: Device.gu(30)
        font.pixelSize: Device.gu(18)
        font.family: "LiSu"
        font.weight: Font.Bold
        width: Device.gu(32)
        wrapMode: Text.WrapAnywhere
        lineHeight: 1.5
        horizontalAlignment: Text.AlignHCenter

        glow.color: "black"
        glow.spread: 0.7
        glow.radius: Device.gu(12)
        glow.samples: 24
    }

    GlowText {
        id: deputyGeneralNameItem
        color: "white"
        x: Device.gu(80)
        y: Device.gu(30)
        font.pixelSize: Device.gu(18)
        font.family: "LiSu"
        font.weight: Font.Bold
        width: Device.gu(32)
        wrapMode: Text.WrapAnywhere
        lineHeight: 1.5
        horizontalAlignment: Text.AlignHCenter

        glow.color: "black"
        glow.spread: 0.7
        glow.radius: Device.gu(12)
        glow.samples: 24
    }

    RoleComboBox {
        x: parent.width - width - Device.gu(5)
        y: Device.gu(1)
    }

    Rectangle {
        id: disableMask
        anchors.fill: parent
        color: "black"
        opacity: 0.3
        visible: root.state == "candidate" && !root.selectable
    }

    Image {
        source: root.phase != "inactive" ? "image://root/phase/" + root.phase + ".png" : ""
        width: parent.width * 0.9
        height: implicitHeight / implicitWidth * width
        x: (parent.width - width) / 2
        y: parent.height - Device.gu(3)
        visible: root.phase != "inactive"
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (parent.state != "candidate" || !parent.selectable)
                return;
            parent.selected = !parent.selected;
            roomScene.photoSelected(roomScene.getSelectedSeats());
        }
    }

    ProgressBar {
        id: progressBarItem
        width: parent.width
        height: 10
        y: parent.height + Device.gu(10)
        visible: false
    }

    InvisibleCardArea {
        id: handcardAreaItem
        anchors.centerIn: parent
    }
}
