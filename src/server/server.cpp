/********************************************************************
    Copyright (c) 2013-2015 - Mogara

    This file is part of QSanguosha.

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

#include "engine.h"
#include "gamelogic.h"
#include "gamemode.h"
#include "roomsettings.h"
#include "server.h"

#include <CRoom>
#include <CServerUser>
#include <CServerRobot>

Server::Server(QObject *parent)
    : CServer(parent)
{
    CRoom *lobby = this->lobby();
    lobby->setName(tr("QSanguosha Lobby"));

    connect(this, &CServer::roomCreated, [](CRoom *room){
        GameLogic *logic = new GameLogic(room);
        room->setGameLogic(logic);

        room->setSettings(new RoomSettings);
        CServerUser *owner = room->owner();
        room->setName(tr("%1's Room").arg(owner->screenName()));
        room->broadcastConfig();
    });

    connect(this, &CServer::robotAdded, [](CServerRobot *robot) {
        robot->initAi("script/Ai/smart-ai.js");
    });
}
