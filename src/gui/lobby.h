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

#ifndef LOBBY_H
#define LOBBY_H

#include <cglobal.h>
#include <QQuickItem>

#include <cclientuser.h>

class Client;
class CClientUser;

class Lobby : public QQuickItem
{
    Q_OBJECT

public:
    Lobby(QQuickItem *parent = 0);

    Q_INVOKABLE void createRoom();
    Q_INVOKABLE void speakToServer(const QString &text);
    Q_INVOKABLE void updateRoomList();

signals:
    void messageLogged(const QString &message);
    void roomListUpdated(const QVariant &rooms);

protected:
    Q_INVOKABLE void onCreateButtonClicked();
    Q_INVOKABLE void onRoomListItemClicked(uint id);
    Q_INVOKABLE void onReadyButtonClicked();

    void onRoomPropertyUpdated(const QString &name, const QVariant &value);

    void onRoomEntered(const QVariant &config);
    void onUserAdded(const CClientUser *user);
    void onUserRemoved(const CClientUser *user);
    void onUserSpeaking(const QString &message);
    void onSystemMessageReceived(const QString &message);

    Client *m_client;
};

#endif // LOBBY_H
