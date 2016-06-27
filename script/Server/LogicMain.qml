import QtQuick 2.5
import Sanguosha.CardArea 1.0
import Sanguosha.Player 1.0
import Sanguosha.GameLogic 1.0


QtObject {
    id: gameLogicMain

    property GameLogic gameLogicCpp;

    property var eventHandlers;
    property list players;
    property ServerPlayer currentPlayer;
    property list extraTurns;
    property GameRule gameRule;
    property list packages;
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

    function init()
    {
        currentPlayer = null;
        gameRule = null;
        skipGameRule = false;
        round = 0;
        reshufflingCount = 0;

        drawPile = new CardArea(CardArea.DrawPile);
        discardPile = new CardArea(CardArea.DiscardPile);
        tablePile = new CardArea(CardArea.Table);
        tablePile.setIsVirtualCardArea(true);
        wuguPile = new CardArea(CardArea.Wugu);

        // load game mode

        var players = gameLogicCpp.players(); // list?



    }

    function run()
    {

    }
}
