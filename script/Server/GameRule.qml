import QtQuick 2.5
import Sanguosha.GameLogic 1.0
import Sanguosha.Player 1.0
import Sanguosha.DataValues 1.0


EventHandler {

    id : gameRule;

    function init(){
        EventHandler.init();
        EventHandler.events = [GameLogic.TurnStart, GameLogic.PhaseProceeding, GameLogic.PhaseEnd,
        GameLogic.AfterHpReduced, GameLogic.AskForPeach, GameLogic.AskForPeachDone];//......
    }

    function prepareToStart(logic)
    {

    }

    function triggerable(logic, eventType, data, player){
        var result = {};
        var e = new Event();
        e.init(logic,this);
        result[0] = e;
        return result;
    }

    function onEffect(logic, eventType, eventValue, data, player){
        if (logic.skipGameRule()) return false;
        switch (eventType){
            case GameLogic.TurnStart :
                player.setTurnCount(player.trunCount + 1);
                if (!player.faceUp())
                    player.setFaceUp(true)
                else
                    player.play();
                break;

            case GameLogic.PhaseProceeding :
                logic.msleep(500);
                switch (player.phase()) {
                case Player.Judge :
                    var tricks = player.delayedTrickArea().cards();
                    while (tricks.length > 0 && player.isAlive()){
                        var trick = tricks.pop();
                        if (trick.type() === Card.TrickType && trick.subtype() === TrickCard.DelayedType) {
                            var use = new CardUseValue;
                            use.card = trick;
                            use.to.push(player);
                            var effect = new CardEffectValue(use);
                            effect.to = player;
                            logic.takeCardEffect(effect);
                            trick.complete(logic);
                        }
                    }
                    break;
                case Player.Draw :
                    var value = new IntValue(2);
                    logic.trigger(GameLogic.DrawNCards, player, value);
                    var num = value.value;
                    if (num > 0)
                        player.drawCards(num);
                    var valueNew = new IntValue(num);
                    logic.trigger(GameLogic.AfterDrawNCards, player, valueNew);
                    break;
                 case Player.Play :
                     while (player.isAlive()) {
                         if (player.activate())
                             break;
                     }
                     break;
                 case Player.Discard :
                     var max_num = player.hp();
                     var int_value = new IntValue(max_num);
                     logic.trigger(GameLogic.CountMaxCardNum, player, int_value);
                     var maxCardNum = int_value.value;
                     var discardNum = player.handcardNum() - maxCardNum;
                     if (discardNum > 0) {
                         player.showPrompt("ask_to_discard", discardNum);
                         var cards = player.askForCards(".|.|.|hand", discardNum);

                         var moves = new CardsMoveValue;
                         for (var card in cards) {
                             var move = new CardMove;
                             move.card = card;
                             move.toArea = logic.discardPile();
                             move.isOpen = true;
                             moves.moves.push(move);
                         }
                         logic.moveCards(moves);
                     }
                     break;
                 default:

                }

                break

            case GameLogic.PhaseEnd :
                switch (player.phase()) {
                    case Player.Play :
                        player.clearCardHistory();
                        player.clearSkillHistory();
                        break;
                    default:;
                    }
                if (player.isDrunk()) {
                    player.setDrunk(false);
                    player.broadcastProperty("drunk");
                }
                break;

            case GameLogic.AfterHpReduced :
                if (player.hp() > 0)
                    return;

                player.setDying(true);
                player.broadcastProperty("dying");

                var death = new DeathValue;
                death.who = player;
                //death.damage = qobject_cast<DamageValue *>(data);
                death.damage = data; // ......

                for (var p in logic.allPlayers()) {
                    if (logic.trigger(GameLogic.EnterDying, p, death) || player.hp() > 0 || player.isDead())
                        break;
                }

                if (player.isAlive() && player.hp() <= 0) {
                    for(var saver in logic.allPlayers()) {
                        if (player.hp() > 0 || player.isDead())
                            break;

                        logic.trigger(GameLogic.AskForPeach, saver, death);
                    }
                    logic.trigger(GameLogic.AskForPeachDone, player, death);
                }

                player.setDying(false);
                player.broadcastProperty("dying");
                logic.trigger(GameLogic.QuitDying, player, death);
                break;

            case GameLogic.AskForPeach :
                //DeathValue *dying = qobject_cast<DeathValue *>(data);
                var dying = data; //.......
                while (dying.who.hp() <= 0) {
                   var peach = null;
                   if (dying.who.isAlive()) {
                       var peachNum = 1 - dying.who.hp();
                       if (player !== dying.who) {
                           var args = [];
                           args.push("player"); args.push(dying.who.id());
                           args.push(peachNum);
                           player.showPrompt("ask_for_peach", args);
                           peach = player.askForCard("Peach");
                       } else {
                           player.showPrompt("ask_self_for_peach_or_analeptic", peachNum);
                           peach = player.askForCard("Peach,Analeptic");
                       }
                   }
                   if (peach == null)
                       break;

                    var _use = new CardUseValue;
                   _use.from = player;
                   _use.card = peach;
                   _use.to.push(dying.who);
                   logic.useCard(_use);
               }
                break;

            case GameLogic.AskForPeachDone :
                if (player.hp() <= 0 && player.isAlive()) {
                        //DeathValue *death = qobject_cast<DeathValue *>(data);
                        death = data;//.......
                        logic.killPlayer(player, death.damage);
                    }
                break;
            default:
                throw GameFinish;
        }
        return false;
    }
}
