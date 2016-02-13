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

#ifndef PCCONSOLESTARTDIALOG_H
#define PCCONSOLESTARTDIALOG_H

#include <CardirectorGlobal>
#include <QQuickItem>

class Client;

class CRoom;

class PcConsoleStartDialog : public QQuickItem
{
    Q_OBJECT

public:
    PcConsoleStartDialog(QQuickItem *parent = 0);

    Q_INVOKABLE void start(const QString &screenName, const QString &avatar);

signals:
    void enterLobby();

private:
    void onServerConnected();

    Client *m_client;
    QString m_screenName;
    QString m_avatar;
};

#endif // PCCONSOLESTARTDIALOG_H
