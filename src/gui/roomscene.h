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
    void moveCards(const QVariant &moves);
    void enableCards(const QVariant &cardIds);
    void chooseGeneral(const QVariant &generals);
    void startEmotion(const QString &emotion, int seat);
    void playAudio(const QString &path);
    void showIndicatorLine(int from, const QVariantList &tos);
    void showPrompt(const QString &prompt);

private:
    enum RespondingState
    {
        InactiveState,
        UsingCardState,
        RespondingCardState
    };

    void animateCardsMoving(const QList<CardsMoveStruct> &moves);

    void onSeatArranged();
    void onChooseGeneralRequested(const QStringList &candidates);
    void onChooseGeneralFinished(const QString &head, const QString &deputy);
    void onUsingCard(const QString &pattern);
    void onCardSelected(const QVariantList &cardIds);
    void onPhotoSelected(const QVariantList &seats);
    void onAccepted();
    void onDamageDone(const ClientPlayer *victim, DamageStruct::Nature nature, int damage);
    void onCardUsed(const ClientPlayer *from, const QList<const ClientPlayer *> &tos);
    void onCardAsked(const QString &pattern, const QString &prompt);

    Client *m_client;
    QList<const Card *> m_selectedCard;
    QList<const ClientPlayer *> m_selectedPlayer;
    RespondingState m_respondingState;
};

#endif // ROOMSCENE_H
