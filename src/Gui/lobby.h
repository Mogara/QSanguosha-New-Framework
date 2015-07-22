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

    Q_PROPERTY(uint currentRoomId READ currentRoomId NOTIFY currentRoomIdChanged)
    Q_PROPERTY(QString currentRoomName READ currentRoomName NOTIFY currentRoomNameChanged)
    Q_PROPERTY(QUrl currentRoomLogo READ currentRoomLogo NOTIFY currentRoomLogoChanged)
    Q_PROPERTY(QUrl userAvatar READ userAvatar NOTIFY userAvatarChanged)
    Q_PROPERTY(QString userName READ userName NOTIFY userNameChanged)

public:
    Lobby(QQuickItem *parent = 0);

    uint currentRoomId() const { return m_currentRoomId; }
    QString currentRoomName() const { return m_currentRoomName; }
    QUrl currentRoomLogo() const { return m_currentRoomLogo; }
    QUrl userAvatar() const { return m_userAvatar;}
    QString userName() const { return m_userName; }

    Q_INVOKABLE void createRoom();
    Q_INVOKABLE void speakToServer(const QString &text);
    Q_INVOKABLE void updateRoomList();

signals:
    void messageLogged(const QString &message);

    void currentRoomIdChanged();
    void currentRoomNameChanged();
    void currentRoomLogoChanged();
    void userAvatarChanged();
    void userNameChanged();

    void roomAdded(const QVariant &room);
    void roomRemoved(uint id);
    void roomListCleared();

protected:
    C_DECLARE_INITIALIZER(Lobby)

    void onRoomListUpdated(const QVariant &list);
    void onPlayerAdded(const CClientPlayer *player);
    void onPlayerRemoved(const CClientPlayer *player);
    void onPlayerSpeaking(const QString &message);

    void setCurrentRoomId(uint id);
    void setCurrentRoomName(const QString &name);
    void setCurrentRoomLogo(const QUrl &url);
    void setUserAvatar(const QUrl &avatar);
    void setUserName(const QString &name);

    Client *m_client;

    uint m_currentRoomId;
    QString m_currentRoomName;
    QUrl m_currentRoomLogo;
    QUrl m_userAvatar;
    QString m_userName;
};

#endif // LOBBY_H
