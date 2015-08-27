import QtQuick 2.4
import Cardirector.Device 1.0

GraphicsBox {
    property var cards: []
    property int topCapacity: 5
    property int bottomCapacity: 5
    property int padding: Device.gu(25)

    title.text: qsTr("Please arrange cards")
    width: body.width + padding * 2
    height: title.height + body.height + padding * 2

    Column {
        id: body
        x: padding
        y: parent.height - padding - height
        spacing: Device.gu(20)

        Row {
            id: topArea
            spacing: Device.gu(5)
            anchors.horizontalCenter: parent.horizontalCenter

            Repeater {
                id: pileTopItem
                model: topCapacity

                Rectangle {
                    color: "#1D1E19"
                    width: Device.gu(93)
                    height: Device.gu(130)

                    Text {
                        anchors.centerIn: parent
                        text: qsTr("Cards on the top of card pile")
                        color: "#59574D"
                        width: parent.width * 0.8
                        wrapMode: Text.WordWrap
                    }
                }
            }
        }

        Row {
            id: bottomArea
            spacing: Device.gu(5)
            anchors.horizontalCenter: parent.horizontalCenter

            Repeater {
                id: pileBottomItem
                model: bottomCapacity

                Rectangle {
                    color: "#1D1E19"
                    width: Device.gu(93)
                    height: Device.gu(130)

                    Text {
                        anchors.centerIn: parent
                        text: qsTr("Cards on the bottom of card pile")
                        color: "#59574D"
                        width: parent.width * 0.8
                        wrapMode: Text.WordWrap
                    }
                }
            }
        }
    }

    Repeater {
        id: cardItem
        model: cards

        CardItem {
            onEntered: z++;
            onExited: z--;
            onReleased: arrangeCards();
        }
    }

    Component.onCompleted: {
        var i, pos, item, card;
        for (i = 0; i < cards.length; i++) {
            item = pileTopItem.itemAt(i);
            pos = mapFromItem(pileTopItem, item.x, item.y);
            card = cardItem.itemAt(i);
            card.homeX = pos.x;
            card.homeY = pos.y;
            card.goBack(true);
        }

        if (cards.length > topCapacity)
            cards = cards.slice(0, topCapacity);
    }

    function arrangeCards()
    {
        var topCards = [];
        var bottomCards = [];
        var i, card;
        for (i = 0; i < cardItem.count; i++) {
            card = cardItem.itemAt(i);
            if (bottomCards.length < bottomCapacity && card.y + card.height / 2 >= bottomArea.y)
                bottomCards.push(card);
            else
                topCards.push(card);
        }

        function compareByX(a, b) { return a.x - b.x; }
        topCards.sort(compareByX);
        bottomCards.sort(compareByX);

        var pile = [pileTopItem, pileBottomItem];
        var cards = [topCards, bottomCards];
        var j, box, pos;
        for (j = 0; j < pile.length; j++) {
            for (i = 0; i < cards[j].length; i++) {
                box = pile[j].itemAt(i);
                pos = mapFromItem(pile[j], box.x, box.y);
                card = cards[j][i];
                card.homeX = pos.x;
                card.homeY = pos.y;
                card.goBack(true);
            }
        }
    }
}
