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

#include "structs.h"

#include <QQuickItem>

class Client;
class ClientPlayer;

class RoomScene : public QQuickItem
{
    Q_OBJECT

public:
    RoomScene(QQuickItem *parent = 0);

signals:
    //Signals from QML to C++
    void chooseGeneralFinished(const QString &head, const QString &deputy);
    void cardSelected(const QVariantList &cardIds);
    void photoSelected(const QVariantList &seats);
    void accepted();

    //Signals from C++ to QML
    void cardsMoved(const QVariant &moves);
    void cardEnabled(const QVariant &cardIds);
    void chooseGeneralStarted(const QVariant &generals);

private:
    void animateCardsMoving(const QList<CardsMoveStruct> &moves);

    void onSeatArranged();
    void onChooseGeneralRequested(const QStringList &candidates);
    void onChooseGeneralFinished(const QString &head, const QString &deputy);
    void onUsingCard(const QString &pattern);
    void onCardSelected(const QVariantList &cardIds);
    void onPhotoSelected(const QVariantList &seats);
    void onAccepted();

    Client *m_client;
    QList<const Card *> m_selectedCard;
    QList<const ClientPlayer *> m_selectedPlayer;
};

#endif // ROOMSCENE_H
