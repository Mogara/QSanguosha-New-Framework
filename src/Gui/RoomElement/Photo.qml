import QtQuick 2.4
import QtQuick.Layouts 1.1
import Cardirector.Device 1.0
import Cardirector.Resource 1.0

Item {
    property string headGeneral: "anjiang"
    property string deputyGeneral: "anjiang"
    property alias screenName: screenNameItem.text
    property int maxHp: 5
    property alias faceTurned: faceTurnedCover.visible

    width: Device.gu(157)
    height: Device.gu(181)
    clip: true

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

    RowLayout {
        anchors.fill: parent

        Item {
            width: Device.gu(75)
            height: Device.gu(181)

            Image {
                anchors.fill: parent
                fillMode: Image.PreserveAspectCrop
                source: "image://general/fullphoto/" + headGeneral
            }
        }

        Item {
            width: Device.gu(75)
            height: Device.gu(181)

            Image {
                anchors.fill: parent
                fillMode: Image.PreserveAspectCrop
                source: "image://general/fullphoto/" + deputyGeneral
            }
        }
    }

    Image {
        source: "image://photo/circle-photo"
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

    Text {
        id: screenNameItem
        color: "white"
        font.pixelSize: Device.gu(12)
        x: Device.gu(5)
        y: Device.gu(3)
    }
}
