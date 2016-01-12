import QtQuick 2.4
import Cardirector.Device 1.0

Item {
    property string value: "unknown"

    width: Device.gu(22)
    height: Device.gu(22)
    visible: value != "hidden"

    Item {
        anchors.fill: parent
        visible: value == "unknown"

        SimpleKingdomIcon {
            id: wei
            originalColor: "#2779AA"
        }

        SimpleKingdomIcon {
            id: qun
            originalColor: "#C3B154"
            x: parent.width / 2
        }

        SimpleKingdomIcon {
            id: shu
            originalColor: "#AC3120"
            y: parent.height / 2
        }

        SimpleKingdomIcon {
            id: wu
            originalColor: "#55A434"
            x: parent.width / 2
            y: parent.height / 2
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            expandingBox.forceActiveFocus();
            expandingBox.visible = true;
        }
    }

    Image {
        id: expandingBox
        source: "image://root/kingdom/expanding"
        focus: true
        visible: false

        onActiveFocusChanged: {
            if (!activeFocus)
                visible = false;
        }

        Image {
            source: "image://root/kingdom/expandingmask/wei"
            x: Device.gu(3)
            y: Device.gu(3)
            visible: !wei.included
        }

        Image {
            source: "image://root/kingdom/expandingmask/qun"
            x: Device.gu(23)
            y: Device.gu(3)
            visible: !qun.included
        }

        Image {
            source: "image://root/kingdom/expandingmask/shu"
            x: Device.gu(3)
            y: Device.gu(23)
            visible: !shu.included
        }

        Image {
            source: "image://root/kingdom/expandingmask/wu"
            x: Device.gu(23)
            y: Device.gu(23)
            visible: !wu.included
        }

        MouseArea {
            width: parent.width / 2
            height: parent.height / 2
            onClicked: wei.included = !wei.included;
        }

        MouseArea {
            width: parent.width / 2
            height: parent.height / 2
            x: parent.width / 2
            onClicked: qun.included = !qun.included;
        }

        MouseArea {
            width: parent.width / 2
            height: parent.height / 2
            y: parent.height / 2
            onClicked: shu.included = !shu.included;
        }

        MouseArea {
            width: parent.width / 2
            height: parent.height / 2
            x: parent.width / 2
            y: parent.height / 2
            onClicked: wu.included = !wu.included;
        }
    }

    Image {
        source: value ? "image://root/kingdom/" + value : ""
        visible: value != "unknown" && value != "hidden"
        anchors.centerIn: parent
    }
}
