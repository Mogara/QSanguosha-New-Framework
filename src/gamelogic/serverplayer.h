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

#ifndef SERVERPLAYER_H
#define SERVERPLAYER_H

#include "event.h"
#include "player.h"
#include "structs.h"

class CRoom;
class CServerAgent;
class GameLogic;
class Skill;

class ServerPlayer : public Player
{
    Q_OBJECT

public:
    ServerPlayer(GameLogic *logic, CServerAgent *agent);
    ~ServerPlayer();

    CServerAgent *agent() const;
    void setAgent(CServerAgent *agent);

    CRoom *room() const;

    ServerPlayer *next() const { return qobject_cast<ServerPlayer *>(Player::next()); }
    ServerPlayer *next(bool ignoreRemoved) const{ return qobject_cast<ServerPlayer *>(Player::next(ignoreRemoved)); }
    ServerPlayer *nextAlive(int step = 1, bool ignoreRemoved = true) const { return qobject_cast<ServerPlayer *>(Player::nextAlive(step, ignoreRemoved)); }

    void drawCards(int n);
    void recastCard(Card *card);

    void showCard(Card *card);
    void showCards(const QList<Card *> &cards);

    void play();
    void play(const QList<Phase> &phases);
    bool activate();

    void skipPhase(Phase phase) { m_skippedPhase.insert(phase); }
    bool isPhaseSkipped(Phase phase) { return m_skippedPhase.contains(phase); }
    void clearSkippedPhase() { m_skippedPhase.clear(); }

    void showPrompt(const QString &message, int number);
    void showPrompt(const QString &message, const Card *card = nullptr);
    void showPrompt(const QString &message, const ServerPlayer *p1, const Card *card = nullptr);
    void showPrompt(const QString &message, const ServerPlayer *p1, const ServerPlayer *p2, const Card *card = nullptr);
    void showPrompt(const QString &message, const QVariantList &args = QVariantList());

    Event askForTriggerOrder(const QString &reason, const EventList &options, bool cancelable);
    Card *askForCard(const QString &pattern, bool optional = true);
    QList<Card *> askForCards(const QString &pattern, int num, bool optional = false);
    QList<Card *> askForCards(const QString &pattern, int minNum, int maxNum, bool optional = false);
    Card *askToChooseCard(ServerPlayer *owner, const QString &areaFlag = "hej", bool handcardVisible = false);
    Card *askToUseCard(const QString &pattern, const QList<ServerPlayer *> &assignedTargets);

    void broadcastProperty(const char *name) const;
    void broadcastProperty(const char *name, const QVariant &value, ServerPlayer *except = nullptr) const;
    void unicastPropertyTo(const char *name, ServerPlayer *player);

    void addCardHistory(const QString &name, int times = 1);
    void clearCardHistory();

    void addSkill(const Skill *skill, SkillArea area = HeadSkillArea);
    void removeSkill(const Skill *skill, SkillArea area = UnknownSkillArea);

    //These two functions don't trigger SkillAdded/SkillRemoved events
    void attachSkill(const Skill *skill, SkillArea area = HeadSkillArea);
    void detachSkill(const Skill *skill, SkillArea area = UnknownSkillArea);

    QMap<QString, QVariant> tag;

private:
    void addTriggerSkill(const Skill *skill);
    void removeTriggerSkill(const Skill *skill);

    GameLogic *m_logic;
    CRoom *m_room;
    CServerAgent *m_agent;
    QSet<Phase> m_skippedPhase;
};

#endif // SERVERPLAYER_H
