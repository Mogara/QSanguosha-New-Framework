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

#ifndef ROOMSCENE_H
#define ROOMSCENE_H

#include <QQuickItem>

class Client;

class RoomScene : public QQuickItem
{
    Q_OBJECT

public:
    RoomScene(QQuickItem *parent = 0);

signals:
    void chooseGeneralStarted(const QVariant &generals);
    void chooseGeneralFinished(const QString &head, const QString &deputy);

private:
    void onChooseGeneralRequested(const QStringList &candidates);
    void onChooseGeneralFinished(const QString &head, const QString &deputy);

    Client *m_client;
};

#endif // ROOMSCENE_H
