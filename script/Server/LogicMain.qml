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

    property var cardPosition;

    function loadMode(modeName) // use C++?
    {

    }

    function trigger(triggerEvent, player, data)
    {

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
}
