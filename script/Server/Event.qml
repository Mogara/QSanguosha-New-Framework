import QtQuick 2.5
import Sanguosha.GameLogic 1.0
import Sanguosha.Player 1.0

QtObject {

    //property EventType e;
    property var triggerSkill;

    property var skillOwner;
    property var skillInvoker;
    property list targets;

    property bool isCompulsory;
    property bool triggered;

    property var preferredTarget;

    property list tag;

    function init(skill,owner,invoker,targets,is_compulsory,preferred_target) {
        triggerSkill = skill;    // TriggerSkill

        triggered = false;
        isCompulsory = is_compulsory;

        skillInvoker = invoker;    //ServerPlayer
        skillOwner = owner;      //ServerPlayer
        preferredTarget = preferred_target; //ServerPlayer

        targets = targets;    //QList<ServerPlayer *>

        tag = new Array;        //QVariantMap
    }

    function isValid() {
        return triggerSkill !== null;
    }

    function sameSkillWith(other) {
        return triggerSkill === other.triggerSkill && owner === other.owner && invoker === other.invoker;
    }

    function sameTimingWith(other) {
        if (!isValid() || !other.isValid())
            return false;
        return triggerSkill.getPriority() === other.triggerSkill.getPriority() && triggerSkill.inherits("EquipSkill") === other.inherits("EquipSkill")
    }

    function toVariant() {
        var ob = new JSON;
        if (triggerSkill)
            ob["skill"] = triggerSkill.objectName;
        if (owner)
            ob["owner"] = owner.objectName;
        if (invoker)
            ob["invoker"] = invoker.objectName;
        // @to_do(Xusine):insert preferredTarget into ob.

        return ob;
    }

    function toList() {
        var result = new Array;
        if (!isValid()){
            for (var i = 1; i < 4; i++)
                result.push("");
        } else {
            result.push(triggerSkill.objectName);
            result.push(owner.objectName);
            result.push(invoker.objectName);
            result.push(preferredTarget.objectName)
        }
        return result;
    }
}
