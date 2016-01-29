import QtQuick 2.4
import Cardirector.Device 1.0

GraphicsBox {
    property var cards: []
    property var result: []
    property var areaCapacities: []
    property var areaNames: []
    property int padding: Device.gu(25)

    title.text: qsTr("Please arrange cards")
    width: body.width + padding * 2
    height: title.height + body.height + padding * 2

    Column {
        id: body
        x: padding
        y: parent.height - padding - height
        spacing: Device.gu(20)

        Repeater {
            id: areaRepeater
            model: areaCapacities

            Row {
                spacing: Device.gu(5)
                anchors.horizontalCenter: parent.horizontalCenter

                property int areaCapacity: modelData
                property string areaName: index < areaNames.length ? qsTr(areaNames[index]) : ""

                Repeater {
                    id: cardRepeater
                    model: areaCapacity

                    Rectangle {
                        color: "#1D1E19"
                        width: Device.gu(93)
                        height: Device.gu(130)

                        Text {
                            anchors.centerIn: parent
                            text: areaName
                            color: "#59574D"
                            width: parent.width * 0.8
                            wrapMode: Text.WordWrap
                        }
                    }
                }
                property alias cardRepeater: cardRepeater
            }
        }
    }

    Repeater {
        id: cardItem
        model: cards

        CardItem {
            cid: modelData.cid
            name: modelData.name
            suit: modelData.suit
            number: modelData.number
            draggable: true
            onReleased: arrangeCards();
        }
    }

    onWidthChanged: arrangeCards();
    onHeightChanged: arrangeCards();

    function arrangeCards()
    {
        result = new Array(areaCapacities.length);
        var i;
        for(i = 0; i < result.length; i++)
            result[i] = [];

        var card, j, area, cards, stay;
        for (i = 0; i < cardItem.count; i++) {
            card = cardItem.itemAt(i);

            stay = true;
            for (j = areaRepeater.count - 1; j >= 0; j--) {
                area = areaRepeater.itemAt(j);
                cards = result[j];
                if (cards.length < areaCapacities[j] && card.y >= area.y) {
                    cards.push(card);
                    stay = false;
                    break;
                }
            }

            if (stay)
                result[0].push(card);
        }

        for(i = 0; i < result.length; i++)
            result[i].sort(function(a, b){return a.x - b.x;});

        var box, pos, pile;
        for (j = 0; j < areaRepeater.count; j++) {
            pile = areaRepeater.itemAt(j);
            for (i = 0; i < result[j].length; i++) {
                box = pile.cardRepeater.itemAt(i);
                pos = mapFromItem(pile, box.x, box.y);
                card = result[j][i];
                card.homeX = pos.x;
                card.homeY = pos.y;
                card.goBack(true);
            }
        }
    }
}
