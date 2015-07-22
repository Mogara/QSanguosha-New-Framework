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

#include "startserverdialog.h"

#include <cserver.h>
#include <cserverplayer.h>

StartServerDialog::StartServerDialog(QQuickItem *parent)
    : QQuickItem(parent)
    , m_server(NULL)
{
}

void StartServerDialog::createServer()
{
    ushort port = 5927;
    m_server = new CServer(this);
    if (!m_server->listen(QHostAddress::Any, port)) {
        emit messageLogged(tr("The server failed to start, probably due to port %1 occupied by another application.").arg(port));
        m_server->deleteLater();
        m_server = NULL;
        return;
    }

    emit messageLogged(tr("The server is listening on port %1").arg(port));

    connect(m_server, &CServer::playerAdded, this, &StartServerDialog::onPlayerAdded);
}

void StartServerDialog::onPlayerAdded(CServerPlayer *player)
{
    emit messageLogged(tr("New player: %1(%2)").arg(player->screenName()).arg(player->id()));
    connect(player, &CServerPlayer::networkDelayChanged, this, &StartServerDialog::onPlayerNetworkDelayChanged);
    player->updateNetworkDelay();
}

void StartServerDialog::onPlayerNetworkDelayChanged()
{
    CServerPlayer *player = qobject_cast<CServerPlayer *>(sender());
    if(player == NULL)
        return;
    emit messageLogged(tr("Player %1(%2) Network Delay: %3").arg(player->screenName()).arg(player->id()).arg(player->networkDelay()));
}

void StartServerDialog::Init()
{
    qmlRegisterType<StartServerDialog>("Sanguosha.Dialogs", 1, 0, "StartServerDialog");
}
C_INITIALIZE_CLASS(StartServerDialog)
