import QtQuick 2.0

Column {
    property int maxValue: 5
    property int value: 5

    Repeater {
        id: repeater
        model: maxValue
        Magatama {
            state: (maxValue - 1 - index) >= value ? 0 : (value >= 3 || value >= maxValue ? 3 : (value <= 0 ? 0 : value))
        }
    }
}
