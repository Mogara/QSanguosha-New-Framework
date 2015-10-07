import QtQuick 2.4
import Cardirector.Gui 1.0
import Cardirector.Device 1.0

GraphicsBox {
    property alias model: generalList
    property string headGeneral: ""
    property string deputyGeneral: ""

    ListModel {
        id: generalList
    }

    id: root
    title.text: qsTr("Please choose 2 generals of the same kingdom")
    width: generalArea.width + body.anchors.leftMargin + body.anchors.rightMargin
    height: body.implicitHeight + body.anchors.topMargin + body.anchors.bottomMargin

    Column {
        id: body
        anchors.fill: parent
        anchors.margins: Device.gu(40)
        anchors.bottomMargin: Device.gu(20)

        Item {
            id: generalArea
            width: Device.gu(generalList.count >= 5 ? Math.ceil(generalList.count / 2) : Math.max(3, generalList.count)) * 97
            height: Device.gu(generalList.count >= 5 ? 290 : 150)
            z: 1

            Repeater {
                id: generalMagnetList
                model: generalList.count

                Item {
                    width: Device.gu(93)
                    height: Device.gu(130)
                    x: Device.gu((index % Math.ceil(generalList.count / (generalList.count >= 5 ? 2 : 1))) * 98 + (generalList.count >= 5 && index > generalList.count / 2 && generalList.count % 2 == 1 ? 50 : 0))
                    y: generalList.count < 5 ? 0 : (index < generalList.count / 2 ? 0 : Device.gu(135))
                }
            }
        }

        Item {
            id: splitLine
            width: parent.width - Device.gu(80)
            height: Device.gu(6)
            anchors.horizontalCenter: parent.horizontalCenter
            clip: true

            Image {
                anchors.centerIn: parent
                source: "image://root/choosegeneralbox/split-line"
            }
        }

        Item {
            id: resultArea
            width: parent.width
            height: Device.gu(145)

            Rectangle {
                id: headGeneralItem
                color: "#1D1E19"
                radius: Device.gu(3)
                width: Device.gu(93)
                height: Device.gu(130)
                x: parent.width / 2 - width - Device.gu(5)
                y: Device.gu(10)
            }

            Rectangle {
                id: deputyGeneralItem
                color: "#1D1E19"
                radius: Device.gu(3)
                width: Device.gu(93)
                height: Device.gu(130)
                x: parent.width / 2 + Device.gu(5)
                y: Device.gu(10)
            }
        }

        Item {
            id: buttonArea
            width: parent.width
            height: Device.gu(40)

            MetroButton {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                text: qsTr("Fight")
                width: Device.gu(120)
                height: Device.gu(35)
                enabled: headGeneral != "" && deputyGeneral != ""

                onClicked: close();
            }
        }
    }

    Repeater {
        id: generalCardList
        model: generalList

        GeneralCardItem {
            name: model.name
            kingdom: model.kingdom
            selectable: true
            draggable: true

            onClicked: {
                if (headGeneral == name) {
                    headGeneral = "";
                } else if (deputyGeneral == name) {
                    deputyGeneral = "";
                } else {
                    if (headGeneral == "") {
                        headGeneral = name;
                    } else if (deputyGeneral == "") {
                        deputyGeneral = name;
                    }
                }
            }

            onReleased: {
                if (isClicked)
                    return;

                if (y < splitLine.y) {
                    if (headGeneral == name)
                        headGeneral = "";
                    else if (deputyGeneral == name)
                        deputyGeneral = "";
                } else {
                    if (headGeneral == "") {
                        headGeneral = name;
                    } else if (deputyGeneral == "") {
                        deputyGeneral = name;
                    } else {
                        var horizontalCenter = x + width / 2;
                        if (horizontalCenter > deputyGeneralItem.x && headGeneral === name) {
                            headGeneral = deputyGeneral;
                            deputyGeneral = name;
                        } else if (horizontalCenter < headGeneralItem.x + headGeneralItem.width && deputyGeneral === name) {
                            deputyGeneral = headGeneral;
                            headGeneral = name;
                        }
                    }
                }
            }
        }
    }

    onHeadGeneralChanged: arrangeCards();
    onDeputyGeneralChanged: arrangeCards();
    function arrangeCards()
    {
        var item, pos;
        for (var i = 0; i < generalList.count; i++)
        {
            item = generalCardList.itemAt(i);
            if (headGeneral === item.name) {
                pos = root.mapFromItem(resultArea, headGeneralItem.x, headGeneralItem.y);
            } else if (deputyGeneral === item.name) {
                pos = root.mapFromItem(resultArea, deputyGeneralItem.x, deputyGeneralItem.y);
            } else {
                var magnet = generalMagnetList.itemAt(i);
                pos = root.mapFromItem(generalMagnetList.parent, magnet.x, magnet.y);
            }

            if (item.homeX !== pos.x || item.homeY !== item.y) {
                item.homeX = pos.x;
                item.homeY = pos.y;
                item.goBack(true);
            }
        }
    }
}
