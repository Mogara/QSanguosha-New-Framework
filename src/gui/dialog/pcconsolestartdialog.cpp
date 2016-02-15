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
#include "pcconsolestartdialog.h"
#include "server.h"

#include <CRoom>
#include <CServerUser>

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
    Server *server = new Server(qApp);
    if (!server->listen(QHostAddress::Any, port)) {
        server->deleteLater();
        return;
    }

    m_client->connectToHost(QHostAddress::LocalHost, port);
}

void PcConsoleStartDialog::onServerConnected()
{
    m_client->signup("", "", m_screenName, m_avatar);
    enterLobby();
    m_client->createRoom();
}

C_REGISTER_QMLTYPE("Sanguosha.Dialogs", 1, 0, PcConsoleStartDialog)
