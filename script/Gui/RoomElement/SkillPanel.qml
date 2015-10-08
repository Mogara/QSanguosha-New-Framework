import QtQuick 2.4
import Cardirector.Device 1.0

Item {
    width: childrenRect.width
    height: childrenRect.height

    ListModel {
        id: skills
    }

    Repeater {
        model: skills

        SkillButton {
            columns: (skills.count % 2 == 0 || index < skills.count - 1) ? 2 : 1
            x: (index % 2 == 1) ? width + Device.gu(1) : 0
            y: Math.floor(index / 2) * (height + Device.gu(1))

            name: model.name
            type: model.type
            status: model.status
        }
    }

    function addSkill(skill)
    {
        skills.append(skill);
    }

    function removeSkill(name)
    {
        for (var i = 0; i < skills.count; i++) {
            if (skills.get(i).name === name) {
                skills.remove(i);
                break;
            }
        }
    }

    function updateSkillStatus(name, status)
    {
        for (var i = 0; i < skills.count; i++) {
            var skill = skills.get(i);
            if (skill.name === name) {
                skill.status = status;
                break;
            }
        }
    }
}
