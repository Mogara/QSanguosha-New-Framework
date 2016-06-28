import QtQuick 2.5
import Sanguosha.GameLogic 1.0
import Sanguosha.Player 1.0

QtObject {

    //property EventType e;
    property var triggerSkill;

    property ServerPlayer skillOwner;
    property ServerPlayer skillInvoker;
    property var m_targets;

    property bool isCompulsory;
    property bool triggered;

    property ServerPlayer preferredTarget;

    property var tag;

    function init(skill,owner,invoker,targets,is_compulsory,preferred_target) {
        triggerSkill = skill;    // TriggerSkill

        triggered = false;
        isCompulsory = is_compulsory;

        skillInvoker = invoker;    //ServerPlayer
        skillOwner = owner;      //ServerPlayer
        preferredTarget = preferred_target; //ServerPlayer

        m_targets = targets;    //QList<ServerPlayer *>

        tag = {};               //QVariantMap
    }

    function isValid() {
        return triggerSkill !== null;
    }

    function sameSkillWith(other) {
        return triggerSkill === other.triggerSkill && skillOwner === other.skillOwner && skillInvoker === other.skillInvoker;
    }

    function sameTimingWith(other) {
        if (!isValid() || !other.isValid())
            return false;
        return triggerSkill.getPriority() === other.triggerSkill.getPriority() && triggerSkill.inherits("EquipSkill") === other.triggerSkill.inherits("EquipSkill")
    }

    function toVariant() {
        var ob = {};
        if (triggerSkill)
            ob["skill"] = triggerSkill.objectName;
        if (owner)
            ob["owner"] = skillOwner.objectName;
        if (invoker)
            ob["invoker"] = skillInvoker.objectName;
        // @to_do(Xusine):insert preferredTarget into ob.

        return ob;
    }

    function compare(other) { // a function to stand for the operator "<"
        if (!isValid() || !other.isValid())
            return false;
        if (triggerSkill.getPriority() !== other.triggerSkill.getPriority())
            return triggerSkill.getPriority() > other.triggerSkill.getPriority()
        if (skillInvoker !== other.skillInvoker)
            return ServerPlayer.sortByActionOrder(skillInvoker,other.skillInvkoer);
        return triggerSkill.inherits("EquipSkill") && !other.triggerSkill.inherits("EquipSkill")
    }

    function toList() {
        var result = new Array;
        if (!isValid()){
            for (var i = 1; i < 4; i++)
                result.push("");
        } else {
            result.push(triggerSkill.objectName);
            result.push(skillOwner.objectName);
            result.push(skillInvoker.objectName);
            result.push(preferredTarget.objectName)
        }
        return result;
    }
}
