import QtQuick 2.4

Item {
    property alias rows: grid.rows
    property alias columns: grid.columns

    InvisibleCardArea {
        id: area
    }

    ListModel {
        id: cards
    }

    Grid {
        id: grid
        anchors.fill: parent
        rows: 100
        columns: 100

        Repeater {
            model: cards

            Image {
                source: "image://root/card/icon/" + name
            }
        }
    }

    function add(inputs)
    {
        area.add(inputs);
        if (inputs instanceof Array) {
            for (var i = 0; i < inputs.length; i++)
                cards.append(inputs[i]);
        } else {
            cards.append(inputs);
        }
    }

    function remove(outputs)
    {
        var result = area.remove(outputs);
        for (var i = 0; i < result.length; i++) {
            var item = result[i];
            for (var j = 0; j < cards.count; j++) {
                var icon = cards.get(j);
                if (icon.cid === item.cid) {
                    cards.remove(j, 1);
                    break;
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
