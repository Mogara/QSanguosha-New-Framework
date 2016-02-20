import QtQuick 2.4
import Cardirector.Device 1.0
import "../../engine.js" as Engine

Item {
    property string name: null
    property string suit: null
    property int number: null
    property bool selectable: false
    property bool selected: false

    id: root
    width: Device.gu(145)
    height: Device.gu(22)
    opacity: 0

    Image {
        source: name ? "image://root/card/equip/" + name : ""
    }

    Image {
        source: suit ? "image://root/card/suit/" + suit : ""
        x: parent.width - width
        y: Math.round((parent.height - height) / 2)
    }

    GlowText {
        text: number ? (Engine.convertNumber(number)) : ""
        font.pixelSize: Device.gu(14)
        x: parent.width - Device.gu(28)
        y: Device.gu(2)

        glow.color: "#CFCFCD"
        glow.radius: 1
        glow.spread: 1
        glow.samples: 2
    }

    PixmapAnimation {
        id: border
        visible: false
        source: "equip_border"
        loop: true
        width: Device.gu(156)
        height: Device.gu(32)
        x: -Math.round((width - root.width) / 2)
        y: -Math.round((height - root.height) / 2)

        Connections {
            target: root
            onSelectedChanged: {
                if (root.selected)
                    border.start();
                else
                    border.stop();
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (!selectable || selectAnime.running || unselectAnime.running)
                return;
            selected = !selected;
        }
    }

    ParallelAnimation {
        id: showAnime

        NumberAnimation {
            target: root
            property: "opacity"
            duration: 500
            easing.type: Easing.InOutQuad
            from: 0
            to: 1
        }


        NumberAnimation {
            target: root
            property: "x"
            duration: 500
            easing.type: Easing.InOutQuad
            from: Device.gu(15)
            to: 0
        }
    }

    ParallelAnimation {
        id: hideAnime

        NumberAnimation {
            target: root
            property: "opacity"
            duration: 500
            easing.type: Easing.InOutQuad
            from: 1
            to: 0
        }


        NumberAnimation {
            target: root
            property: "x"
            duration: 500
            easing.type: Easing.InOutQuad
            from: 0
            to: Device.gu(10)
        }
    }

    NumberAnimation {
        id: selectAnime
        target: root
        property: "x"
        duration: 200
        easing.type: Easing.Linear
        from: 0
        to: Device.gu(10)
        onStarted: border.visible = true;
    }

    NumberAnimation {
        id: unselectAnime
        target: root
        property: "x"
        duration: 200
        easing.type: Easing.Linear
        from: Device.gu(10)
        to: 0
        onStopped: border.visible = false;
    }

    onSelectedChanged: {
        if (selected)
            selectAnime.start();
        else
            unselectAnime.start();
    }

    function show()
    {
        showAnime.start();
    }

    function hide()
    {
        hideAnime.start();
    }
}
