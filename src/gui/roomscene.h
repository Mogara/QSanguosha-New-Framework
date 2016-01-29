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
class ViewAsSkill;

class RoomScene : public QQuickItem
{
    Q_OBJECT

public:
    RoomScene(QQuickItem *parent = 0);

    //From QML to C++
    Q_INVOKABLE void onChooseGeneralFinished(uint head, uint deputy);
    Q_INVOKABLE void onCardSelected(const QVariantList &cardIds);
    Q_INVOKABLE void onPhotoSelected(const QVariantList &seats);
    Q_INVOKABLE void onAccepted();
    Q_INVOKABLE void onRejected();
    Q_INVOKABLE void onFinished();
    Q_INVOKABLE void onAmazingGraceTaken(uint cid);
    Q_INVOKABLE void onPlayerCardSelected(uint cid);
    Q_INVOKABLE void onSkillActivated(uint skillId, bool activated);
    Q_INVOKABLE void onOptionSelected(int selected);
    Q_INVOKABLE void onArrangeCardDone(const QVariantList &results);

signals:
    //Signals from C++ to QML
    void moveCards(const QVariant &moves);
    void enableCards(const QVariant &cardIds);
    void enablePhotos(const QVariant &seats);
    void chooseGeneral(const QVariant &generals);
    void startEmotion(const QString &emotion, int seat);
    void playAudio(const QString &path);
    void showIndicatorLine(int from, const QVariantList &tos);
    void showPrompt(const QString &prompt);
    void hidePrompt();
    void setAcceptEnabled(bool enabled);
    void setRejectEnabled(bool enabled);
    void setFinishEnabled(bool enabled);
    void askToChooseCards(const QVariant &cards);
    void clearPopupBox();
    void askToChoosePlayerCard(const QVariant &handcards, const QVariant &equips, const QVariant &delayedTricks);
    void showCard(int fromSeat, const QVariant &cards);
    void showOptions(const QStringList &options);
    void showArrangeCardBox(const QVariant &cards, const QVariant &capacities, const QVariant &names);

private:
    enum RespondingState
    {
        InactiveState,
        UsingCardState,
        RespondingCardState
    };

    void checkTargetFeasibility();
    void resetDashboard();
    void enableCards(const QString &pattern);
    void enableCards(const QList<const Card *> &cards);

    void onSeatArranged();
    void onChooseGeneralRequested(const QList<const General *> &candidates);
    void onCardsMoved(const QList<CardsMoveStruct> &moves);
    void onUsingCard(const QString &pattern = QString(), const QList<const Player *> &assignedTargets = QList<const Player *>());
    void onDamageDone(const ClientPlayer *victim, DamageStruct::Nature nature, int damage);
    void onRecoverDone(const ClientPlayer *from, const ClientPlayer *to, int num);
    void onCardUsed(const ClientPlayer *from, const QList<const ClientPlayer *> &tos);
    void onCardAsked(const QString &pattern);
    void onCardsAsked(const QString &pattern, int minNum, int maxNum, bool optional);
    void onAmazingGraceStarted();
    void onChoosePlayerCardRequested(const QList<Card *> &handcards, const QList<Card *> &equips, const QList<Card *> &delayedTricks);
    void onCardShown(const ClientPlayer *from, const QList<const Card *> &cards);
    void onArrangeCardRequested(const QList<Card *> &cards, const QList<int> &capacities, const QStringList &areaNames);

    QVariantMap convertToMap(const Card *card) const;

    Client *m_client;
    QList<const Card *> m_selectedCard;
    QList<const Player *> m_selectedPlayer;
    RespondingState m_respondingState;
    int m_minRespondingCardNum;
    int m_maxRespondingCardNum;
    bool m_respondingOptional;
    QString m_respondingPattern;
    QList<const Player *> m_assignedTargets;
    const ViewAsSkill *m_viewAsSkill;
};

#endif // ROOMSCENE_H
