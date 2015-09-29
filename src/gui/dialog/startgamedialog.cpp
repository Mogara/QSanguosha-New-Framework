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

#include "startgamedialog.h"
#include "client.h"

StartGameDialog::StartGameDialog(QQuickItem *parent)
    : QQuickItem(parent)
    , m_client(Client::instance())
{
    connect(m_client, &CClient::connected, this, &StartGameDialog::onServerConnected);
    connect(m_client, &CClient::connected, this, &StartGameDialog::lobbyEntered);
}

void StartGameDialog::signup(const QString &screenName, const QString &avatar)
{
    m_screenName = screenName;
    m_avatar = avatar;
}

void StartGameDialog::connectToServer(const QString &serverAddress)
{
    int colon = serverAddress.lastIndexOf(':');
    QString ip = serverAddress.mid(0, colon);
    ushort port = serverAddress.mid(colon + 1).toUShort();
    QHostAddress host(ip);
    m_client->connectToHost(host, port);
}

void StartGameDialog::onServerConnected()
{
    m_client->signup("", "", m_screenName, m_avatar);
}

C_REGISTER_QMLTYPE("Sanguosha.Dialogs", 1, 0, StartGameDialog)
