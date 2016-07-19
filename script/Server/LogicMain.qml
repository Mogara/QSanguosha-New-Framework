import QtQuick 2.5
import Sanguosha.CardArea 1.0
import Sanguosha.Player 1.0
import Sanguosha.GameLogic 1.0


QtObject {
    id: gameLogicMain

    property GameLogic gameLogicCpp;

    property var eventHandlers;
    property var players;
    property ServerPlayer currentPlayer;
    property var extraTurns;
    property GameRule gameRule;
    property var packages;
    property var cards;
    property bool skipGameRule;
    property int round;
    property int reshufflingCount;

    property CardArea drawPile;
    property CardArea discardPile;
    property CardArea tablePile;
    property CardArea wuguPile;

    property var cardPosition; // use id for its index.

    function loadMode(modeName) // use C++?
    {

    }

    function trigger(triggerEvent, player, data)
    {
        // use C++ ?
    }

    function init()
    {
        currentPlayer = null;
        gameRule = null;
        skipGameRule = false;
        round = 0;
        reshufflingCount = 0;
        players = new Array;
        extraTurns = new Array;
        packages = new Array;

        drawPile = new CardArea(CardArea.DrawPile);
        discardPile = new CardArea(CardArea.DiscardPile);
        tablePile = new CardArea(CardArea.Table);
        tablePile.setIsVirtualCardArea(true);
        wuguPile = new CardArea(CardArea.Wugu);

        // load game mode
        // loadMode(gameLogicCpp.mode);

        var playerList = gameLogicCpp.playersList();
        for (var i = 0; i < playerList.length; ++i)
            players[i] = playerList[i];

        for (i = 0; i < players.length - 1; ++i) {
            var j = Math.floor(Math.random() * (players.length - i + 1)) + i;
            if (i != j) {
                var t = players[i];
                players[i] = players[j];
                players[j] = t;
            }
        }

        for (i = 0; i < players.length - 1; ++i) {
            players[i].setSeat(i + 1);
            players[i].setNext(players[i + 1]);
        }
        players[players.length - 1].setSeat(players.length);
        players[players.length - 1].setNext(players[0]);
        currentPlayer = players[0];

        gameLogicCpp.broadcastSeats();

        // load general list
        // load cards map
        // broadcast card

        cards = {
            count: 0,
            ids: new Array,
            cards: new Array
        };

        cardPosition = {};

        for (i = 0; i < cards.count; ++i) {
            drawPile.add(cards.cards[i]);
            cardPosition[cards.cards[i]] = drawPile;
        }

        gameRule.prepareToStart(this);

        eventHandlers = {}
    }

    function run()
    {
        for (;;) {
            try {
                var current = currentPlayer;
                for (;;) {
                    if (current.seat === 1)
                        round += 1;
                    if (current.dead) {
                        current = current.next();
                        continue;
                    }

                    currentPlayer = current;
                    trigger(GameLogic.TurnStart, current, null);
                    current = current.next;

                    while (extraTurns.length > 0) {
                        var extra = extraTurns.shift();
                        currentPlayer = extra;
                        trigger(GameLogic.TurnStart, current, null);
                    }
                }
            } catch (triggerEvent) {
                if (triggerEvent === GameLogic.GameFinish)
                    return;
                if (triggerEvent === GameLogic.TurnBroken) {
                    var turnBrokenCurrent = currentPlayer;
                    trigger(GameLogic.TurnBroken, turnBrokenCurrent, null);
                    var next = current.nextAlive(1, false);
                    if (turnBrokenCurrent.phase !== Player.Inactive) {
                        // gameRule.effect(this, GameLogic.PhaseEnd, new Event, null, turnBrokenCurrent);
                        // @todo:
                        turnBrokenCurrent.phase = Player.Inactive;
                        turnBrokenCurrent.broadcastProperty("phase");
                    }
                    currentPlayer = next;
                }
            }
        }
    }
    function setGameRule(rule){
        if (gameRule) {
                for (var e in gameRule.events) {
                    if (eventHandlers[e].contains(gameRule))
                        eventHandlers[e].remove(gameRule);
                }
            }

            gameRule = rule;
            if (rule) {
                for (var _e in rule.events)
                    eventHandlers[_e].push(rule);
            }
    }
    function setPackages(pkgs){
        if (pkgs instanceof Array)
            if (pkgs[1] instanceof String){
                var packagPtrs = [];
                for (var name in packages){
                    var pac = Sanguosha.package(name);
                    if (pac)
                        packagPtrs.push(pac);
                }
                packages = packagePtr;
            } else {
                packages = pkgs;
            }
    }
    function addEventHandler(evthdr){
        for (var e in evthdr.events){
            if (!eventHandlers[e].contains(evthdr)){
                eventHandlers[e].push(evthdr);
            }
        }
    }
    function removeEventHandler(evthdr){
        for(var e in evthdr.events){
            eventHandlers[e].remove(evthdr);
        }
    }

    function getEventHandlersAndSort(event,detailsList,triggered,data,player){
        // use C++ ? (I think this function won't be updated.)
    }

    function findPlayer(id){
        //return qobject_cast<ServerPlayer *>(findAbstractPlayer(id));
        // a fucking qobject_cast!
    }

    function allPlayers(include_dead){
        var result = players;
        var current = currentPlayer;
        if (!current)
            return result;

        //    std::sort(players.begin(), players.end(), [](const ServerPlayer *a, const ServerPlayer *b){
        //        return a->seat() < b->seat();
        //    });
        result.sort(function(a,b){
            return a.seat < b.seat;
        }
        )
        var index = result.indexOf(current);
        if (index === -1)
            return reuslt;

        var allp = gameLogicCpp.playersList();
        for (var i = index;i < result.length; ++i){
            if (include_dead || result[i].isAlive())
                allp.push(result[i])
        }
        for (var j = 0; j < index; j++){
            if (include_dead || result[j].isAlive())
                allp.push(result[j])
        }

        if (current.phase === Player.Inactive && allp.contains(currnt)){
            allp.removeOne(current);
            allp.push(current);
        }
        return allp;
    }

    function otherPlayers(except,include_dead){
        var result = allPlayers(include_dead);
        if (except && (except.isAlive() || include_dead))
                result.removeOne(except);
        return result;
    }

    function sortByActionOrder(ps){
        var allp = allPlayers(true);
        ps.sort(function (a,b){
            return allp.indexOf(a) < allp.indexOf(b);
        })
    }

    function reshuffleDrawPile(){
        if (discardPile.ength() <= 0){
            gameOver([]);
        }
        reshufflingCount++;
        /*if (limit > 0 && times == limit)
               gameOver(".");*/
        var cards = discardPile.cards();
        discardPile.clear();
        cards = cards.shuffle();

        for (var c in cards){
            cardPosition[c.id] = drawPile;
        }

        drawPile.add(cards,CardArea.Bottom)
    }

    function getDrawPileCards(n){
        if (drawPile.length() < n)
            reshuffleDrawPile();
        return drawPile.first(n);
    }

    function getDrawPileCard(){
        return getDrawPileCards(1);
    }

    function filterCardsMove(moves){

    }

    function moveCards(moves){
        filterCardsMove(moves);
        for (var p in allPlayers(false)){
            trigger(GameLogic.BeforeCardsMove,p,moves);
        }
        filterCardsMove(moves);

        for (var i = 0; i < moves.length; ++i){
            var move = moves[i];
            if (move.fromArea.contains(move.card, true)){
                if (move.fromArea.remove(move.card)){
                    move.toArea.add(move.card,move.toDirection);
                    cardPosition[move.card.id] = move.toArea;
                }
            }
        }

        for (var i = 0; i < moves.virtualMoves.length; ++i) {
            var move = moves.virtualMoves[i];
            // there are cases each of which fromArea can be nullptr sometimes, so we shouldn't relay on the move.fromArea
            // but if the move.fromArea != nullptr, the move.fromArea must be virtual card area
            var flag = true;
            if (move.fromArea){
                // we should check the virtual card is actually in that area
                if (move.fromArea.contains(move.card)){
                    flag = move.fromArea.remove(move.card);
                }
                 // if move.fromArea has succeeded in removing the old card, we should judge whether the toPosition is virtual card area
                if (flag && move.toArea.isVirtualCardArea()) {
                    move.toArea.add(move.card);
                    cardPosition[move.card.id] = move.toArea;
                } else {
                    cardPosition[move.card.id].remove(move.card);
                }
            }
        }


        for (var viewer in players){
            var data = moves.toVariant(viewer);
            viewer.agent().notify(MOVE_CARDS, data); // how to deal with this protcol......
        }

        for (var _p in allPlayers(false)){
            trigger(GameLogic.AfterCardsMove,_p,moves);
        }

    }

    function useCard(use){
        if (!(use.card && use.from))
            return false;

        //    //Initialize isHandcard
        use.isHandcard = true;
        for (var c in use.card.realCards()) {
            var area = cardPosition[c.id];
            if (area === null || area.owner() !== use.from || area.type() !== CardArea.Hand){
                use.isHandcard = false;
                break;
            }
        }
        if (!use.card.isVirtual()) // make a virtual card of it since all the card in the card using process is virtual card
            use.card = use.card.makeVirtual();

        if (use.from.phase() === Player.Play && use.addHistory)
            use.from.addCardHistory(use.card.objectName());

        try {
            use.card.onUse(this, use);
            trigger(GameLogic.CardUsed, use.from, use);

            if (use.from) {
                trigger(GameLogic.TargetChoosing, use.from, use);
                var args = {};
                args["from"] = use.from.id();
                var tos = [];
                for (var to in use.to)
                   tos.push(to.id());
                args["to"] = tos;

                /*
                since the use card is all virtual card, we can only use the args["card"]
                if (!use.card->isVirtual())
                    args["cardId"] = use.card->id();
                else
                */

                args["card"] = use.card.toVariant();

                room().broadcastNotification(S_COMMAND_USE_CARD, args); //How to deal with this function.

                if (use.from) {
                    if (!use.to.isEmpty()) {
                        for (var _to in use.to) {
                            if (!use.to.contains(_to))
                                continue;
                            trigger(GameLogic.TargetConfirming, _to, use);
                        }

                        if (use.from && !use.to.isEmpty()) {
                            trigger(GameLogic.TargetChosen, use.from, use);

                            if (use.from && !use.to.isEmpty()) {
                                for (var tar in use.to) {
                                    if (!use.to.contains(tar))
                                        continue;
                                    trigger(GameLogic.TargetConfirmed, tar, use);
                                }

                                use.card.use(this, use);
                            }
                        }
                    } else if (use.target) {
                        use.card.use(this, use);
                    }
                }
            }

            trigger(GameLogic.CardFinished, use.from, use);

        } catch (e) {
            //@to-do: handle TurnBroken and StageChange
            throw e;
        }

        return true;
    }
    function takeCardEffect (effect){
        var canceled = false;
        if (effect.to) {
            if (effect.to.isAlive()) {
                canceled = trigger(GameLogic.CardEffect, effect.to, effect);
                if (!canceled) {
                    canceled = trigger(GameLogic.CardEffected, effect.to, effect);
                    if (!canceled) {
                        effect.use.card.onEffect(this, effect);
                        if (effect.to.isAlive() && !effect.isNullified())
                            effect.use.card.effect(this, effect);
                    }
                }
            }
        } else if (effect.use.target) {
            effect.use.card.onEffect(this, effect);
            if (!effect.isNullified())
                effect.use.card.effect(this, effect);
        }
        trigger(GameLogic.PostCardEffected, effect.to, effect);
        return !canceled;
    }
    function invokeProactiveSkill(invoke) {
        if (invoke) {
            if (proactiveSkill.cost(this, invoke.player, invoke.targets, invoke.cards)) {
                proactiveSkill.effect(this, invoke.player, invoke.targets, invoke.cards);
                return true;
            }
        } else{
            //throw   // we only throw it in debug mode @todo_Fs: find all the places which can cause an abnormal processing
        }
        return false;
    }
    function respondCard(response) {
        // todo_Fs: make the responsed card a virtual card

        var moves = new CardsMoveValue;
        var move = new CardMove;
        move.card = response.card;
        move.toArea = m_table;
        move.isOpen = true;
        moves.moves.push(move);
        moveCards(moves);

        var broken = false;

        broken = trigger(GameRule.CardResponded, response.from, response);

        if (response.card && tablePile.contains(response.card)) {
            var _moves = new CardsMoveValue;
            var _move = new CardMove;
            _move.card = response.card;
            _move.toArea = discardPile;
            _move.isOpen = true;
            _moves.moves.append(_move);
            moveCards(_moves);
        }

        return !broken;
    }

}
