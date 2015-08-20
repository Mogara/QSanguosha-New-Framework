#include "gamelogic.h"
#include "player.h"

#include <croom.h>
#include <cserver.h>

GameLogic::GameLogic(CRoom *parent)
    : CAbstractGameLogic(parent)
{

}

CAbstractPlayer *GameLogic::createPlayer()
{
    return new Player(this);
}

void GameLogic::run()
{
    //@to-do: implement Sanguosha logic here
}
