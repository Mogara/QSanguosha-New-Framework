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

#ifndef LOBBY_H
#define LOBBY_H

#include <cglobal.h>
#include <QQuickItem>

#include <cclientplayer.h>

class Client;
class CClientPlayer;

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

    void roomAdded(const QVariant &room);
    void roomRemoved(uint id);
    void roomListCleared();

protected:
    C_DECLARE_INITIALIZER(Lobby)

    Q_INVOKABLE void onCreateButtonClicked();
    Q_INVOKABLE void onRoomListItemClicked(uint id);

    void onRoomListUpdated(const QVariant &list);
    void onRoomEntered(const QVariant &config);
    void onPlayerAdded(const CClientPlayer *player);
    void onPlayerRemoved(const CClientPlayer *player);
    void onPlayerSpeaking(const QString &message);

    Client *m_client;
};

#endif // LOBBY_H
