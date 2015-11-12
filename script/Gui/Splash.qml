import QtQuick 2.4
import QtQuick.Layouts 1.1
import Cardirector.Gui 1.0
import Cardirector.Device 1.0

Rectangle {
    id: splash
    anchors.fill: parent
    color: "#bc3b3b"
    focus: true
    z: 100

    property bool loading: true
    property alias animationRunning: animation.running

    signal disappearing
    signal disappeared

    Grid {
        id: main
        anchors.centerIn: parent
        rows: splash.width >= splash.height ? 1 : 2
        columns: splash.width >= splash.height ? 2 : 1
        horizontalItemAlignment: Grid.AlignHCenter
        verticalItemAlignment: Grid.AlignVCenter
        spacing: Device.gu(25)

        Image {
            id: logo
            source: "image://system/mogara/logo"
            width: Math.min(implicitWidth, Math.round(Math.max(splash.width, splash.height) * 0.2))
            height: width
        }

        Column {
            spacing: Device.gu(6)

            Text {
                id: qsan
                text: qsTr("QSanguosha")
                color: "#ffffff"
                font.family: "微软雅黑"
                font.pixelSize: Device.gu(40)
                opacity: 0
            }

            RowLayout {
                width: parent.width
                spacing: Device.gu(8)

                Text {
                    id: free
                    text: qsTr("Free")
                    color: "#ffffff"
                    font.family: "微软雅黑"
                    font.pixelSize: Device.gu(20)
                    opacity: 0
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                }

                Text {
                    id: open
                    text: qsTr("Open")
                    color: "#ffffff"
                    font.family: "微软雅黑"
                    font.pixelSize: Device.gu(20)
                    opacity: 0
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                }

                Text {
                    id: flexible
                    text: qsTr("Flexible")
                    color: "#ffffff"
                    font.family: "微软雅黑"
                    font.pixelSize: Device.gu(20)
                    opacity: 0
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                }
            }
        }
    }

    Text {
        id: text
        text: qsTr("Press Any Key...")
        color: "#ffffff"
        opacity: 0
        font.family: "微软雅黑"
        font.pointSize: 15
        horizontalAlignment: Text.AlignHCenter
        anchors.horizontalCenter: parent.horizontalCenter
        y: (main.y + main.height + parent.height - height) / 2
        SequentialAnimation on opacity {
            id: textAni
            running: false
            loops: Animation.Infinite
            NumberAnimation { from: 0; to: 1; duration: 1600; easing.type: Easing.InOutQuad; }
            NumberAnimation { from: 1; to: 0; duration: 1600; easing.type: Easing.InOutQuad; }
        }
    }

    SequentialAnimation {
        id: animation
        running: true

        NumberAnimation {
            target: logo
            property: "anchors.horizontalCenterOffset"
            to: -splash.width / 8
            duration: 1000
            easing.type: Easing.InOutQuad
        }

        NumberAnimation {
            target: qsan
            property: "opacity"
            duration: 500
            easing.type: Easing.InOutQuad
            to: 1
        }

        NumberAnimation {
            target: free
            property: "opacity"
            duration: 400
            easing.type: Easing.InOutQuad
            to: 1
        }

        NumberAnimation {
            target: open
            property: "opacity"
            duration: 400
            easing.type: Easing.InOutQuad
            to: 1
        }

        NumberAnimation {
            target: flexible
            property: "opacity"
            duration: 400
            easing.type: Easing.InOutQuad
            to: 1
        }


        ScriptAction { script: textAni.start(); }

        PropertyAction { target: splash; property: "loading"; value: false }
    }

    Text {
        text: qsTr("Powered by Mogara")
        color: "#f39292"
        font.family: "Segoe Script"
        font.pixelSize: Device.gu(20)
        anchors.bottom: parent.bottom
        anchors.right: parent.right
    }

    //--------------------Disappear--------------
    Behavior on opacity {
        SequentialAnimation {
            NumberAnimation { duration: 2000; easing.type: Easing.InOutQuad }
            ScriptAction { script: disappeared() }
        }
    }
    MouseArea {
        acceptedButtons: Qt.AllButtons
        anchors.fill: parent
        onClicked: {
            if (!loading) {
                disappear();
            }
        }
    }

    Keys.onPressed: {
        if (!loading) {
            disappear();
            event.accepted = true
        }
    }

    NumberAnimation {
        id: logoMover
        target: logo
        property: "x"
        to: -splash.width
        duration: 1000
        easing.type: Easing.InOutQuad
    }

    function disappear() {
        disappearing();
        logoMover.start();
        opacity = 0;
    }
}
