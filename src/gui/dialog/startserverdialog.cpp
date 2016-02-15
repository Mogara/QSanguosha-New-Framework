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

#include "startserverdialog.h"
#include "server.h"

#include <CRoom>
#include <CServerUser>

StartServerDialog::StartServerDialog(QQuickItem *parent)
    : QQuickItem(parent)
    , m_server(nullptr)
{
}

void StartServerDialog::createServer()
{
    ushort port = 5927;
    m_server = new Server(this);
    if (!m_server->listen(QHostAddress::Any, port)) {
        emit messageLogged(tr("The server failed to start, probably due to port %1 occupied by another application.").arg(port));
        m_server->deleteLater();
        m_server = nullptr;
        return;
    }

    emit messageLogged(tr("The server is listening on port %1").arg(port));

    connect(m_server, &CServer::userAdded, this, &StartServerDialog::onUserAdded);
    connect(m_server, &CServer::roomCreated, this, &StartServerDialog::onRoomCreated);
}

void StartServerDialog::onUserAdded(CServerUser *user)
{
    emit messageLogged(tr("User %1(%2) logged in.").arg(user->screenName()).arg(user->id()));
    connect(user, &CServerUser::networkDelayChanged, this, &StartServerDialog::onUserNetworkDelayChanged);
    connect(user, &CServerUser::disconnected, this, &StartServerDialog::onUserRemoved);
    user->updateNetworkDelay();
}

void StartServerDialog::onUserNetworkDelayChanged()
{
    CServerUser *user = qobject_cast<CServerUser *>(sender());
    if(user == nullptr)
        return;
    emit messageLogged(tr("User %1(%2) Network Delay: %3").arg(user->screenName()).arg(user->id()).arg(user->networkDelay()));
}

void StartServerDialog::onUserRemoved()
{
    CServerUser *user = qobject_cast<CServerUser *>(sender());
    if(user == nullptr)
        return;
    emit messageLogged(tr("User %1(%2) logged out.").arg(user->screenName()).arg(user->id()));
}

void StartServerDialog::onRoomCreated(CRoom *room)
{
    connect(room, &CRoom::abandoned, this, &StartServerDialog::onRoomAbandoned);
    CServerUser *owner = room->owner();
    emit messageLogged(tr("%1(%2) created a new room(%3)").arg(owner->screenName()).arg(owner->id()).arg(room->id()));
}

void StartServerDialog::onRoomAbandoned()
{
    CRoom *room = qobject_cast<CRoom *>(sender());
    if(room == nullptr)
        return;
    emit messageLogged(tr("Room(%1) became empty and thus closed.").arg(room->id()));
}

C_REGISTER_QMLTYPE("Sanguosha.Dialogs", 1, 0, StartServerDialog)
