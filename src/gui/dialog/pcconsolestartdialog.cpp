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

#include "client.h"
#include "engine.h"
#include "gamelogic.h"
#include "gamemode.h"
#include "pcconsolestartdialog.h"

#include <cserver.h>
#include <croom.h>
#include <cserveruser.h>

#include <QCoreApplication>

PcConsoleStartDialog::PcConsoleStartDialog(QQuickItem *parent)
    : QQuickItem(parent)
    , m_client(Client::instance())
{
    connect(m_client, &CClient::connected, this, &PcConsoleStartDialog::onServerConnected);
}

void PcConsoleStartDialog::start(const QString &screenName, const QString &avatar)
{
    m_screenName = screenName;
    m_avatar = avatar;

    ushort port = 5927;
    CServer *server = new CServer(qApp);
    if (!server->listen(QHostAddress::Any, port)) {
        server->deleteLater();
        return;
    }

    connect(server, &CServer::roomCreated, [](CRoom *room){
        //@to-do: load game logic on owner updating configurations
        GameLogic *logic = new GameLogic(room);
        Engine *engine = Engine::instance();
        const GameMode *mode = engine->modes().at(0);
        logic->setGameRule(mode->rule());
        logic->setPackages(engine->getPackages(mode));
        room->setGameLogic(logic);

        CServerUser *owner = room->owner();
        room->setName(tr("%1's Room").arg(owner->screenName()));
    });

    m_client->connectToHost(QHostAddress::LocalHost, port);
}

void PcConsoleStartDialog::onServerConnected()
{
    m_client->signup("", "", m_screenName, m_avatar);
    enterLobby();
    m_client->createRoom();
}

C_REGISTER_QMLTYPE("Sanguosha.Dialogs", 1, 0, PcConsoleStartDialog)
