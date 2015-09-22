import QtQuick 2.4
import Cardirector.Device 1.0

Item {
    property var cards: []
    property int length: 0
    property var pendingInput: []
    property bool checkExisting: false

    id: root

    function add(inputs)
    {
        var card;
        if (inputs instanceof Array) {
            for (var i = 0; i < inputs.length; i++) {
                card = inputs[i];
                pendingInput.push(card);
                cards.push(card.toData());
                card.destroy();
            }

            if (checkExisting)
                length = cards.length;
            else
                length += inputs.length;
        } else {
            pendingInput.push(inputs);
            cards.push(inputs.toData());

            if (checkExisting)
                length = cards.length;
            else
                length++;
        }
    }

    function contains(cid)
    {
        if (!checkExisting)
            return true;

        for (var i = 0; i < cards.length; i++)
        {
            if (cards[i].cid === cid)
                return true;
        }
        return false;
    }

    function remove(outputs)
    {
        var component = Qt.createComponent("CardItem.qml");
        if (component.status !== Component.Ready)
            return [];

        var parentPos = roomScene.mapFromItem(root, 0, 0);
        var card;
        var items = [];
        for (var i = 0; i < outputs.length; i++) {
            if (contains(outputs[i].cid)) {
                var state = outputs[i];
                state.homeX = parentPos.x;
                state.homeY = parentPos.y;
                state.opacity = 0;
                card = component.createObject(roomScene, state);
                card.x -= card.width / 2;
                card.y -= card.height / 2;
                items.push(card);
                if (checkExisting) {
                    cards.splice(i, 1);
                    i--;
                }
            }
        }
        if (checkExisting)
            length = cards.length;
        else
            length -= outputs.length;
        return items;
    }

    function updateCardPosition(animated)
    {
        var i, card;

        if (animated) {
            var parentPos = roomScene.mapFromItem(root, 0, 0);
            for (i = 0; i < pendingInput.length; i++) {
                card = pendingInput[i];
                card.homeX = parentPos.x - card.width / 2;
                card.homeY = parentPos.y - card.height / 2;
                card.homeOpacity = 1;
                card.destroyOnStop();
            }

            for (i = 0; i < pendingInput.length; i++)
                pendingInput[i].goBack(true);
        } else {
            for (i = 0; i < pendingInput.length; i++) {
                card = pendingInput[i];
                card.x = parentPos.x - card.width / 2;
                card.y = parentPos.y - card.height / 2;
                card.opacity = 1;
                card.destroy();
            }
        }

        pendingInput = [];
    }
}
