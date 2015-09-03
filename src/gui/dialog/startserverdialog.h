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

#ifndef STARTSERVERDIALOG_H
#define STARTSERVERDIALOG_H

#include <cglobal.h>
#include <QQuickItem>

class CServer;
class CServerUser;
class CRoom;

class StartServerDialog : public QQuickItem
{
    Q_OBJECT

public:
    StartServerDialog(QQuickItem *parent = 0);

    Q_INVOKABLE void createServer();

signals:
    void messageLogged(const QString &message);

protected:
    void onUserAdded(CServerUser *user);
    void onUserRemoved();
    void onUserNetworkDelayChanged();
    void onRoomCreated(CRoom *room);
    void onRoomAbandoned();

    CServer *m_server;
};

#endif // STARTSERVERDIALOG_H
