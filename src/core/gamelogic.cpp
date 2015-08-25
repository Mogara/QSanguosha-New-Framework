/********************************************************************
    Copyright (c) 2013-2015 - Mogara

    This file is part of Cardirector.

    This game engine is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 3.0
    of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    See the LICENSE file for more details.

    Mogara
*********************************************************************/

#include "gamelogic.h"
#include "player.h"

#include <croom.h>
#include <cserver.h>

GameLogic::GameLogic(CRoom *parent)
    : CAbstractGameLogic(parent)
{

}

CAbstractPlayer *GameLogic::createPlayer(CServerUser *user)
{
    C_UNUSED(user);
    return new Player(this);
}

CAbstractPlayer *GameLogic::createPlayer(CRobot *robot)
{
    C_UNUSED(robot);
    return new Player(this);
}

void GameLogic::run()
{
    //@to-do: implement Sanguosha logic here
}
