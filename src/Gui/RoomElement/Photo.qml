import QtQuick 2.4
import QtQuick.Layouts 1.1
import Cardirector.Device 1.0
import Cardirector.Resource 1.0
import QtGraphicalEffects 1.0

Item {
    property string headGeneral: "anjiang"
    property string deputyGeneral: "anjiang"
    property alias screenName: screenNameItem.text
    property int maxHp: 5
    property alias faceTurned: faceTurnedCover.visible
    property string kingdom: "qun"
    property alias handcardNum: handcardNumItem.value

    width: Device.gu(157)
    height: Device.gu(181)

    ImageProvider {
        id: roomSceneImage
        providerId: "photo"

        function imagePath(imageId, requestedSize)
        {
            return "image/roomscene/photo/" + imageId + ".png";
        }
    }

    ImageProvider {
        id: generalImage
        providerId: "general"

        function imagePath(imageId, requestedSize)
        {
            return "image/general/" + imageId + ".png";
        }
    }

    Item {
        width: deputyGeneral != "" ? Device.gu(75) : Device.gu(155)
        height: Device.gu(182)

        Image {
            anchors.fill: parent
            fillMode: Image.PreserveAspectCrop
            source: "image://general/fullphoto/" + headGeneral
        }
    }

    Item {
        x: Device.gu(80)
        width: Device.gu(75)
        height: Device.gu(182)

        Image {
            anchors.fill: parent
            fillMode: Image.PreserveAspectCrop
            source: "image://general/fullphoto/" + deputyGeneral
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
        kingdom: parent.kingdom
    }

    Text {
        id: screenNameItem
        color: "white"
        font.pixelSize: Device.gu(12)
        x: Device.gu(5)
        y: Device.gu(3)
    }

    Text {
        id: headGeneralNameItem
        text: "暗将"
        color: "white"
        y: Device.gu(30)
        font.pixelSize: Device.gu(18)
        font.family: "隶书"
        font.weight: Font.Bold
        width: Device.gu(32)
        wrapMode: Text.WrapAnywhere
        lineHeight: 1.5
        horizontalAlignment: Text.AlignHCenter
    }

    Glow {
        source: headGeneralNameItem
        anchors.fill: headGeneralNameItem
        color: "black"
        spread: 0.7
        radius: Device.gu(12)
        samples: 24
    }

    Text {
        id: deputyGeneralNameItem
        text: "暗将"
        color: "white"
        x: Device.gu(80)
        y: Device.gu(30)
        font.pixelSize: Device.gu(18)
        font.family: "隶书"
        font.weight: Font.Bold
        width: Device.gu(32)
        wrapMode: Text.WrapAnywhere
        lineHeight: 1.5
        horizontalAlignment: Text.AlignHCenter
    }

    Glow {
        source: deputyGeneralNameItem
        anchors.fill: deputyGeneralNameItem
        color: "black"
        spread: 0.7
        radius: Device.gu(12)
        samples: 24
    }

    RoleComboBox {
        x: parent.width - width - Device.gu(5)
        y: Device.gu(1)
        width: Device.gu(22)
        height: Device.gu(22)
    }
}
