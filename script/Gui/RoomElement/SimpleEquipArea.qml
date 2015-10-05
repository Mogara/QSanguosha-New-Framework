import QtQuick 2.4

Column {
    property int itemHeight: Math.floor(height / 4)
    property var items: [weaponItem, armorItem, defensiveHorseItem, offensiveHorseItem, treasureItem]

    InvisibleCardArea {
        id: area
    }

    SimpleEquipItem {
        id: weaponItem
        width: parent.width
        height: itemHeight
        icon: "sword" //@to-do: assign icon for different weapons
        opacity: 0
    }

    SimpleEquipItem {
        id: armorItem
        width: parent.width
        height: itemHeight
        icon: "shield"
        opacity: 0
    }

    Row {
        width: parent.width
        height: itemHeight

        Item {
            width: Math.ceil(parent.width / 2)
            height: itemHeight

            SimpleEquipItem {
                id: defensiveHorseItem
                width: parent.width
                height: itemHeight
                icon: "horse"
                opacity: 0
            }
        }

        Item {
            width: Math.floor(parent.width / 2)
            height: itemHeight

            SimpleEquipItem {
                id: offensiveHorseItem
                width: parent.width
                height: itemHeight
                icon: "horse"
                opacity: 0
            }
        }
    }

    SimpleEquipItem {
        id: treasureItem
        width: parent.width
        height: itemHeight
        //@to-do: add icon for treasures
        opacity: 0
    }

    function add(inputs)
    {
        area.add(inputs);

        var card, item;
        if (inputs instanceof Array) {
            for (var i = 0; i < inputs.length; i++) {
                card = inputs[i];
                item = items[card.subtype];
                item.setCard(card);
                item.show();
            }
        } else {
            card = inputs;
            item = items[card.subtype];
            item.setCard(card);
            item.show();
        }
    }

    function remove(outputs)
    {
        var result = area.remove(outputs);
        for (var i = 0; i < result.length; i++) {
            var card = result[i];
            for (var j = 0; j < items.length; j++) {
                var item = items[j];
                if (item.cid === card.cid) {
                    item.reset();
                    item.hide();
                }
            }
        }

        return result;
    }

    function updateCardPosition(animated)
    {
        area.updateCardPosition(animated);
    }
}
