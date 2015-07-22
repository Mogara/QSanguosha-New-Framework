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

class Lobby : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(uint currentRoomId READ currentRoomId NOTIFY currentRoomIdChanged)
    Q_PROPERTY(QString currentRoomName READ currentRoomName NOTIFY currentRoomNameChanged)
    Q_PROPERTY(QUrl currentRoomLogo READ currentRoomLogo NOTIFY currentRoomLogoChanged)

public:
    Lobby(QQuickItem *parent = 0);

    uint currentRoomId() const { return m_currentRoomId; }
    QString currentRoomName() const { return m_currentRoomName; }
    QUrl currentRoomLogo() const { return m_currentRoomLogo; }

    Q_INVOKABLE void createRoom();
    Q_INVOKABLE void speakToServer(const QString &text);

signals:
    void currentRoomIdChanged();
    void currentRoomNameChanged();
    void currentRoomLogoChanged();

protected:
    C_DECLARE_INITIALIZER(Lobby)

    void setCurrentRoomId(uint id);
    void setCurrentRoomName(const QString &name);
    void setCurrentRoomLogo(const QUrl &url);

    uint m_currentRoomId;
    QString m_currentRoomName;
    QUrl m_currentRoomLogo;
};

#endif // LOBBY_H
