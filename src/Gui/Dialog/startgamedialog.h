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

#ifndef STARTGAMEDIALOG_H
#define STARTGAMEDIALOG_H

#include <cglobal.h>
#include <QQuickItem>

class Client;

class StartGameDialog : public QQuickItem
{
    Q_OBJECT

public:
    StartGameDialog(QQuickItem *parent = 0);

    Q_INVOKABLE void signup(const QString &screenName, const QString &avatar);
    Q_INVOKABLE void connectToServer(const QString &server, ushort port);

signals:
    void lobbyEntered();

protected:
    void onServerConnected();

    Client *m_client;
    QString m_screenName;
    QString m_avatar;
};

#endif // STARTGAMEDIALOG_H
