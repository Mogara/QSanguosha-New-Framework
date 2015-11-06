import QtQuick 2.4
import Cardirector.Device 1.0

Item {
    property var skills: []

    width: childrenRect.width
    height: childrenRect.height

    Repeater {
        model: skills

        SkillButton {
            columns: (skills.length % 2 == 0 || index < skills.length - 1) ? 2 : 1
            x: (index % 2 == 1) ? width + Device.gu(1) : 0
            y: Math.floor(index / 2) * (height + Device.gu(1))

            name: modelData.name
            type: modelData.type
            enabled: modelData.enabled

            onPressedChanged: {
                if (enabled)
                    roomScene.onSkillActivated(name, pressed);
            }
        }
    }
}
