import QtQuick 2.4

CardArea {
    property var discardedCards: []

    id: root

    Timer {
        id: vanishTimer
        interval: 5000
        repeat: true
        running: true
        triggeredOnStart: true
        onTriggered: {
            var i, card;
            for (i = 0; i < discardedCards.length; i++) {
                card = discardedCards[i];
                card.homeOpacity = 0;
                card.goBack(true);
            }

            cards.splice(0, discardedCards.length);
            updateCardPosition(true);

            discardedCards = new Array(cards.length);
            for (i = 0; i < cards.length; i++)
                discardedCards[i] = cards[i];
        }
    }

    function updateCardPosition(animated)
    {
        if (cards.length <= 0)
            return;

        var i, card;

        var overflow = false;
        for (i = 0; i < cards.length; i++) {
            card = cards[i];
            card.homeX = i * card.width;
            if (card.homeX + card.width >= root.width) {
                overflow = true;
                break;
            }
            card.homeY = 0;
        }

        if (overflow) {
            //@to-do: Adjust cards in multiple lines if there are too many cards
            var xLimit = root.width - card.width;
            var spacing = xLimit / (cards.length - 1);
            for (i = 0; i < cards.length; i++) {
                card = cards[i];
                card.homeX = i * spacing;
                card.homeY = 0;
            }
        }

        var offsetX = Math.max(0, (root.width - cards.length * card.width) / 2);
        var parentPos = roomScene.mapFromItem(root, 0, 0);
        for (i = 0; i < cards.length; i++) {
            card = cards[i];
            card.homeX += parentPos.x + offsetX;
            card.homeY += parentPos.y;
        }

        if (animated) {
            for (i = 0; i < cards.length; i++)
                cards[i].goBack(true);
        }
    }
}
