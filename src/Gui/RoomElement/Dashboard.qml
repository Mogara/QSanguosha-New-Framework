import QtQuick 2.4
import QtQuick.Layouts 1.1
import Cardirector.Device 1.0
import Cardirector.Resource 1.0

RowLayout {
    spacing: 0
    Layout.fillHeight: false
    Layout.preferredHeight: Device.gu(137)

    ImageProvider {
        id: roomSceneImage
        providerId: "dashboard"

        function imagePath(imageId, requestedSize)
        {
            return "image/roomscene/dashboard/" + imageId + ".png";
        }
    }

    Image {
        source: "image://dashboard/equip"
        Layout.preferredWidth: Device.gu(164)
        Layout.fillHeight: true
    }

    Image {
        source: "image://dashboard/hand"
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
}
