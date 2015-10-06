import QtQuick 2.4
import QtQuick.Layouts 1.1
import Cardirector.Device 1.0

GraphicsBox {
    signal cardSelected(int cid)

    id: root
    title.text: qsTr("Please choose one card")
    //@to-do: Adjust the UI design in case there are more than 7 cards
    width: Device.gu(70 + Math.min(7, Math.max(1, handcards.count, equips.count, delayedTricks.count)) * 100)
    height: Device.gu(50 + (handcards.count > 0 ? 150 : 0) + (equips.count > 0 ? 150 : 0) + (delayedTricks.count > 0 ? 150 : 0))

    ListModel {
        id: handcards
    }

    ListModel {
        id: equips
    }

    ListModel {
        id: delayedTricks
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.topMargin: Device.gu(40)
        anchors.leftMargin: Device.gu(20)
        anchors.rightMargin: Device.gu(20)
        anchors.bottomMargin: Device.gu(20)

        Row {
            height: Device.gu(130)
            spacing: Device.gu(15)
            visible: handcards.count > 0

            Rectangle {
                border.color: "#A6967A"
                radius: Device.gu(5)
                color: "transparent"
                width: Device.gu(18)
                height: parent.height

                Text {
                    color: "#E4D5A0"
                    text: qsTr("Handcard Area")
                    anchors.fill: parent
                    wrapMode: Text.WrapAnywhere
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: Device.gu(15)
                }
            }

            Row {
                spacing: Device.gu(7)
                Repeater {
                    model: handcards

                    CardItem {
                        name: "hegback"
                        autoBack: false
                        selectable: true
                        onClicked: root.cardSelected(cid);
                    }
                }
            }
        }

        Row {
            height: Device.gu(130)
            spacing: Device.gu(15)
            visible: equips.count > 0

            Rectangle {
                border.color: "#A6967A"
                radius: Device.gu(5)
                color: "transparent"
                width: Device.gu(18)
                height: parent.height

                Text {
                    color: "#E4D5A0"
                    text: qsTr("Equip Area")
                    anchors.fill: parent
                    wrapMode: Text.WrapAnywhere
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: Device.gu(15)
                }
            }

            Row {
                spacing: Device.gu(7)
                Repeater {
                    model: equips

                    CardItem {
                        cid: model.cid
                        name: model.name
                        suit: model.suit
                        number: model.number
                        autoBack: false
                        selectable: true
                        onClicked: root.cardSelected(cid);
                    }
                }
            }
        }

        Row {
            height: Device.gu(130)
            spacing: Device.gu(15)
            visible: delayedTricks.count > 0

            Rectangle {
                border.color: "#A6967A"
                radius: Device.gu(5)
                color: "transparent"
                width: Device.gu(18)
                height: parent.height

                Text {
                    color: "#E4D5A0"
                    text: qsTr("Delayed Trick Area")
                    anchors.fill: parent
                    wrapMode: Text.WrapAnywhere
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: Device.gu(15)
                }
            }

            Row {
                spacing: Device.gu(7)
                Repeater {
                    model: delayedTricks

                    CardItem {
                        cid: model.cid
                        name: model.name
                        suit: model.suit
                        number: model.number
                        autoBack: false
                        selectable: true
                        onClicked: root.cardSelected(cid);
                    }
                }
            }
        }
    }

    onCardSelected: finished();

    function addHandcards(cards)
    {
        if (cards instanceof Array) {
            for (var i = 0; i < cards.length; i++)
                handcards.append(cards[i]);
        } else {
            handcards.append(cards);
        }
    }

    function addEquips(cards)
    {
        if (cards instanceof Array) {
            for (var i = 0; i < cards.length; i++)
                equips.append(cards[i]);
        } else {
            equips.append(cards);
        }
    }

    function addDelayedTricks(cards)
    {
        if (cards instanceof Array) {
            for (var i = 0; i < cards.length; i++)
                delayedTricks.append(cards[i]);
        } else {
            delayedTricks.append(cards);
        }
    }
}
