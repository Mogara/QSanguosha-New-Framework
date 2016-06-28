import QtQuick 2.5
import Sanguosha.GameLogic 1.0


QtObject {

    property var events;
    property string name;
    property int priority;
    property bool isEquipSkill;

    function init(){
        events = {};
        priority = 0;
        name = "";
        isEquipSkill = false;
    }

    function addEvent(e){
        events.push(e);
    }

    function triggerable(logic, eventType, data, player){
        var result = {};
        return result;
    }

    function onCost(logic, eventType, eventValue, data, player){
        return false;
    }

    function onEffect(logic, eventType, eventValue, data, player){
        return false;
        // @to_do(Xusine): We should create an enumeration to make the result of this function more understandable.
    }
}
