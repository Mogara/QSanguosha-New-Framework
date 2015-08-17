#include "gamelogic.h"
#include "gameplayer.h"

#include <croom.h>
#include <cserver.h>

GameLogic::GameLogic(CRoom *parent)
    : CAbstractGameLogic(parent)
{

}

CAbstractGamePlayer *GameLogic::createPlayer()
{
    return new GamePlayer(this);
}

void GameLogic::run()
{
    //@to-do: implement Sanguosha logic here
}
