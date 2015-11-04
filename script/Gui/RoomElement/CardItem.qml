import QtQuick 2.4
import QtGraphicalEffects 1.0
import Cardirector.Device 1.0

Item {
    property int cid: 0
    property string suit: ""
    property int number: 0
    property string name: "slash"
    readonly property string color: (suit == "heart" || suit == "diamond") ? "red" : "black"
    property int subtype: 0
    property int homeX: 0
    property int homeY: 0
    property real homeOpacity: 1.0
    property int goBackDuration: 500
    property bool selectable: false
    property bool selected: false
    property bool draggable: false
    property bool autoBack: true
    property alias glow: glowItem
    property alias footnote: footnoteItem.text
    property alias card: cardItem
    property bool isClicked: false
    property bool moveAborted: false

    signal toggleDiscards()
    signal clicked()
    signal doubleClicked()
    signal thrown()
    signal released()
    signal entered()
    signal exited()
    signal moveFinished()
    signal generalChanged()
    signal hoverChanged(bool enter)

    id: root
    width: Device.gu(93)
    height: Device.gu(130)

    RectangularGlow {
        id: glowItem
        anchors.fill: parent
        glowRadius: 8
        spread: 0
        color: "#88FFFFFF"
        cornerRadius: 8
        visible: false
    }

    Image {
        id: cardItem
        source: name != "" ? "image://root/card/" + name : ""
        anchors.fill: parent
    }

    Image {
        id: suitItem
        source: suit != "" ? "image://root/card/suit/" + suit : ""
        x: Device.gu(3)
        y: Device.gu(19)
        width: Device.gu(21)
        height: Device.gu(17)
    }

    Image {
        id: numberItem
        source: (color != "" && number > 0) ? "image://root/card/number/" + color + "/" + number : ""
        x: Device.gu(0)
        y: Device.gu(2)
        width: Device.gu(27)
        height: Device.gu(28)
    }

    GlowText {
        id: footnoteItem
        x: Device.gu(4)
        y: parent.height - height - Device.gu(6)
        width: root.width - x * 2
        color: "white"
        wrapMode: Text.WrapAnywhere
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: Device.gu(12)
        glow.color: "black"
        glow.spread: 1
        glow.radius: Device.gu(1)
        glow.samples: 12
    }

    Rectangle {
        visible: !root.selectable
        anchors.fill: parent
        color: Qt.rgba(0, 0, 0, 0.5)
        opacity: 0.7
    }

    MouseArea {
        anchors.fill: parent
        drag.target: draggable ? parent : undefined
        drag.axis: Drag.XAndYAxis
        hoverEnabled: true

        onReleased: {
            root.isClicked = mouse.isClick;
            parent.released();
            if (autoBack)
                goBackAnimation.start();
        }

        onEntered: {
            parent.entered();
            if (draggable) {
                glow.visible = true;
                root.z++;
            }
        }

        onExited: {
            parent.exited();
            if (draggable) {
                glow.visible = false;
                root.z--;
            }
        }

        onClicked: {
            selected = selectable ? !selected : false;
            parent.clicked();
        }
    }

    ParallelAnimation {
        id: goBackAnimation

        PropertyAnimation {
            target: root
            property: "x"
            to: homeX
            easing.type: Easing.OutQuad
            duration: goBackDuration
        }

        PropertyAnimation {
            target: root
            property: "y"
            to: homeY
            easing.type: Easing.OutQuad
            duration: goBackDuration
        }

        SequentialAnimation {
            PropertyAnimation {
                target: root
                property: "opacity"
                to: 1
                easing.type: Easing.OutQuad
                duration: goBackDuration * 0.8
            }

            PropertyAnimation {
                target: root
                property: "opacity"
                to: homeOpacity
                easing.type: Easing.OutQuad
                duration: goBackDuration * 0.2
            }
        }

        onStopped: {
            if (!moveAborted)
                root.moveFinished();
        }
    }

    function setData(data)
    {
        cid = data.cid;
        name = data.name;
        suit = data.suit;
        number = data.number;
        subtype = data.subtype;
    }

    function toData()
    {
        var data = {
            cid: cid,
            name: name,
            suit: suit,
            number: number,
            subtype: subtype
        };
        return data;
    }

    function goBack(animated)
    {
        if (animated) {
            moveAborted = true;
            goBackAnimation.stop();
            moveAborted = false;
            goBackAnimation.start();
        } else {
            x = homeX;
            y = homeY;
            opacity = homeOpacity;
        }
    }

    function destroyOnStop()
    {
        root.moveFinished.connect(function(){
            destroy();
        });
    }
}

