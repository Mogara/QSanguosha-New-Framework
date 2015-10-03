import QtQuick 2.4
import QtGraphicalEffects 1.0
import Cardirector.Device 1.0

Rectangle {
    color: Qt.rgba(0, 0, 0, 0.65)

    id: root

    ListModel {
        id: cards

        ListElement {cid: 0; name: ""; suit: ""; number: 0; subtype: 0}
        ListElement {cid: 0; name: ""; suit: ""; number: 0; subtype: 1}
        ListElement {cid: 0; name: ""; suit: ""; number: 0; subtype: 2}
        ListElement {cid: 0; name: ""; suit: ""; number: 0; subtype: 3}
        ListElement {cid: 0; name: ""; suit: ""; number: 0; subtype: 4}
    }

    InvisibleCardArea {
        id: area
    }

    Column {
        x: Device.gu(7)
        y: Device.gu(13)
        spacing: Device.gu(6)

        Repeater {
            model: cards.count

            Rectangle {
                width: Device.gu(145)
                height: Device.gu(22)

                LinearGradient {
                    anchors.fill: parent
                    start: Qt.point(0, 0)
                    end: Qt.point(parent.width, 0)
                    gradient: Gradient {
                        GradientStop {position: 0; color: "#444343"}
                        GradientStop {position: 1; color: "#2A2925"}
                    }
                }

                Rectangle {
                    width: parent.width - Device.gu(1)
                    height: parent.height - Device.gu(2)
                    y: Device.gu(1)

                    LinearGradient {
                        anchors.fill: parent
                        start: Qt.point(0, 0)
                        end: Qt.point(parent.width, 0)
                        gradient: Gradient {
                            GradientStop {position: 0; color: "#252525"}
                            GradientStop {position: 1; color: "#1B1B1A"}
                        }
                    }

                    Rectangle {
                        width: parent.width
                        height: parent.height - Device.gu(2)
                        y: Device.gu(1)

                        LinearGradient {
                            anchors.fill: parent
                            start: Qt.point(0, 0)
                            end: Qt.point(parent.width, 0)
                            gradient: Gradient {
                                GradientStop {position: 0; color: "#2C2C2C"}
                                GradientStop {position: 1; color: "#181818"}
                            }
                        }
                    }
                }
            }
        }
    }

    Column {
        x: Device.gu(7)
        y: Device.gu(13)
        spacing: Device.gu(6)

        Repeater {
            id: equipItems
            model: cards

            EquipItem {
                name: model.name
                suit: model.suit
                number: model.number
            }
        }
    }

    function add(inputs)
    {
        var data;
        if (inputs instanceof Array) {
            for (var i = 0; i < inputs.length; i++) {
                data = inputs[i].toData();
                cards.set(data.subtype, data);
                equipItems.itemAt(data.subtype).show();
            }
        } else {
            data = inputs.toData();
            cards.set(data.subtype, data);
            equipItems.itemAt(data.subtype).show();
        }
        area.add(inputs);
    }

    function remove(outputs)
    {
        var result = area.remove(outputs);
        for (var i = 0; i < result.length; i++) {
            for (var j = 0; j < cards.count; j++) {
                if (result[i].cid === cards.get(j).cid) {
                    cards.set(j, {cid: 0, name: "", suit: "", number: 0});
                    equipItems.itemAt(j).hide();
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
