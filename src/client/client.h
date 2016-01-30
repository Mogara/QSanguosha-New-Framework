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

#ifndef CLIENT_H
#define CLIENT_H

#include <cclient.h>

#include <QMap>

#include "structs.h"

class Card;
class ClientPlayer;

class Client : public CClient
{
    Q_OBJECT

public:
    static Client *instance();
    ~Client();

    const ClientPlayer *findPlayer(CClientUser *user) const;
    const ClientPlayer *selfPlayer() const { return m_user2player.value(self()); }
    QList<const ClientPlayer *> players() const;
    int playerNum() const;

    const Card *findCard(uint id) { return m_cards.value(id); }
    void useCard(const QList<const Card *> &cards, const QList<const Player *> &targets, const Skill *skill);
    void respondCard(const QList<const Card *> &cards, const Skill *skill);

    const CardArea *wugu() const { return m_wugu; }

    typedef void (*Callback)(Client *, const QVariant &);
    static void AddInteraction(int command, Callback callback);
    static void AddCallback(int command, Callback callback);

signals:
    void promptReceived(const QString &prompt);
    void seatArranged();
    void chooseGeneralRequested(const QList<const General *> &candidates /* @to-do: add banned pair */);
    void cardsMoved(const QList<CardsMoveStruct> &moves);
    void damageDone(const ClientPlayer *victim, DamageStruct::Nature nature, int damage);
    void recoverDone(const ClientPlayer *from, const ClientPlayer *to, int num);
    void usingCard(const QString &pattern, const QList<const Player *> &assignedTarget);
    void cardUsed(const ClientPlayer *from, const QList<const ClientPlayer *> &to);
    void cardAsked(const QString &pattern);
    void cardsAsked(const QString &pattern, int minNum, int maxNum, bool optional);
    void amazingGraceStarted();
    void amazingGraceFinished();
    void amazingGraceRequested();
    void choosePlayerCardRequested(const QList<Card *> &handcards, const QList<Card *> &equips, const QList<Card *> &delayedTricks);
    void cardShown(const ClientPlayer *from, const QList<const Card *> &cards);
    void optionRequested(const QStringList &options);
    void arrangeCardRequested(const QList<Card *> &cards, const QList<int> &capacities, const QStringList &areaNames);
    void skillInvoked(const ClientPlayer *invoker, const Skill *skill, const QList<Card *> &cards, const QList<const ClientPlayer *> &targets);

private:
    Client(QObject *parent = 0);

    void restart();
    ClientPlayer *findPlayer(uint id) { return m_players.value(id); }
    CardArea *findArea(const CardsMoveStruct::Area &area);
    QList<Card *> findCards(const QVariant &data);
    QList<const ClientPlayer *> findPlayers(const QVariant &data);

    static inline QString tr(const QString &text) { return tr(text.toLatin1().constData()); }

    C_DECLARE_INITIALIZER(Client)

    static void ShowPromptCommand(Client *client, const QVariant &data);
    static void ArrangeSeatCommand(Client *client, const QVariant &data);
    static void PrepareCardsCommand(Client *client, const QVariant &data);
    static void UpdatePlayerPropertyCommand(Client *client, const QVariant &data);
    static void ChooseGeneralRequestCommand(Client *client, const QVariant &data);
    static void MoveCardsCommand(Client *client, const QVariant &data);
    static void UseCardRequestCommand(Client *client, const QVariant &data);
    static void UseCardCommand(Client *client, const QVariant &data);
    static void AddCardHistoryCommand(Client *client, const QVariant &data);
    static void DamageCommand(Client *client, const QVariant &data);
    static void RecoverCommand(Client *client, const QVariant &data);
    static void AskForCardRequestCommand(Client *client, const QVariant &data);
    static void ShowAmazingGraceCommand(Client *client, const QVariant &);
    static void ClearAmazingGraceCommand(Client *client, const QVariant &);
    static void TakeAmazingGraceRequestCommand(Client *client, const QVariant &data);
    static void ChoosePlayerCardRequestCommand(Client *client, const QVariant &data);
    static void ShowCardCommand(Client *client, const QVariant &data);
    static void AddSkillCommand(Client *client, const QVariant &data);
    static void RemoveSkillCommand(Client *client, const QVariant &data);
    static void TriggerOrderCommand(Client *client, const QVariant &data);
    static void ArrangeCardCommand(Client *client, const QVariant &data);
    static void ArrangeCardStartCommand(Client *client, const QVariant &data);
    static void ArrangeCardMoveCommand(Client *client, const QVariant &data);
    static void ArrangeCardEndCommand(Client *client, const QVariant &data);
    static void InvokeSkillCommand(Client *client, const QVariant &data);
    static void ClearSkillHistoryCommand(Client *client, const QVariant &data);

    QMap<uint, ClientPlayer *> m_players;
    QMap<CClientUser *, ClientPlayer *> m_user2player;
    QMap<uint, Card *> m_cards;//Record card state

    CardArea *m_wugu;
};

#endif // CLIENT_H
