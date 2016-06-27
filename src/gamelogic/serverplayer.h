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

//#include "event.h"
//#include "player.h"
#include "datavalue.h"

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

    Q_INVOKABLE ServerPlayer *next() const { return qobject_cast<ServerPlayer *>(Player::next()); }
    Q_INVOKABLE ServerPlayer *next(bool ignoreRemoved) const{ return qobject_cast<ServerPlayer *>(Player::next(ignoreRemoved)); }
    Q_INVOKABLE ServerPlayer *nextAlive(int step = 1, bool ignoreRemoved = true) const { return qobject_cast<ServerPlayer *>(Player::nextAlive(step, ignoreRemoved)); }

    Q_INVOKABLE void drawCards(int n);
    Q_INVOKABLE void recastCard(Card *card);

    Q_INVOKABLE void showCard(Card *card);
    Q_INVOKABLE void showCards(const QList<Card *> &cards);

    Q_INVOKABLE void play();
    Q_INVOKABLE void play(const QList<Phase> &phases);
    Q_INVOKABLE bool activate();

    Q_INVOKABLE void skipPhase(Phase phase) { m_skippedPhase.insert(phase); }
    Q_INVOKABLE bool isPhaseSkipped(Phase phase) { return m_skippedPhase.contains(phase); }
    Q_INVOKABLE void clearSkippedPhase() { m_skippedPhase.clear(); }

    Q_INVOKABLE void showPrompt(const QString &message, int number);
    Q_INVOKABLE void showPrompt(const QString &message, const Card *card);
    Q_INVOKABLE void showPrompt(const QString &message, const ServerPlayer *p1, const Card *card = nullptr);
    Q_INVOKABLE void showPrompt(const QString &message, const ServerPlayer *p1, const ServerPlayer *p2, const Card *card = nullptr);
    Q_INVOKABLE void showPrompt(const QString &message, const QVariantList &args = QVariantList());

    Q_INVOKABLE /*EventPtr*/ int askForTriggerOrder(const /*EventPtrList*/ int &options, bool cancelable);
    Q_INVOKABLE Card *askForCard(const QString &pattern, bool optional = true);
    Q_INVOKABLE QList<Card *> askForCards(const QString &pattern, int num, bool optional = false);
    Q_INVOKABLE QList<Card *> askForCards(const QString &pattern, int minNum, int maxNum, bool optional = false);
    Q_INVOKABLE Card *askToChooseCard(ServerPlayer *owner, const QString &areaFlag = "hej", bool handcardVisible = false);
    Q_INVOKABLE DataValue::CardUseValue askToUseCard(const QString &pattern, const QList<ServerPlayer *> &assignedTargets = QList<ServerPlayer *>());
    Q_INVOKABLE DataValue::SkillInvokeValue askToInvokeSkill(const Skill *skill);
    Q_INVOKABLE QList<QList<Card *> > askToArrangeCard(const QList<Card *> &cards, const QList<int> &capacities, const QStringList &areaNames = QStringList());
    Q_INVOKABLE QString askForOption(const QStringList &options);

    Q_INVOKABLE void broadcastProperty(const char *name) const;
    Q_INVOKABLE void broadcastProperty(const char *name, const QVariant &value, ServerPlayer *except = nullptr) const;
    Q_INVOKABLE void unicastPropertyTo(const char *name, ServerPlayer *player);

    Q_INVOKABLE void addSkillHistory(const Skill *skill);
    Q_INVOKABLE void addSkillHistory(const Skill *skill, const QList<Card *> &cards);
    Q_INVOKABLE void addSkillHistory(const Skill *skill, const QList<ServerPlayer *> &targets);
    Q_INVOKABLE void addSkillHistory(const Skill *skill, const QList<Card *> &cards, const QList<ServerPlayer *> &targets);
    Q_INVOKABLE void clearSkillHistory();

    Q_INVOKABLE void addCardHistory(const QString &name, int times = 1);
    Q_INVOKABLE void clearCardHistory();

    Q_INVOKABLE void addSkill(const Skill *skill, SkillArea area = HeadSkillArea);
    Q_INVOKABLE void removeSkill(const Skill *skill, SkillArea area = UnknownSkillArea);

    //These two functions don't trigger SkillAdded/SkillRemoved events
    Q_INVOKABLE void attachSkill(const Skill *skill, SkillArea area = HeadSkillArea);
    Q_INVOKABLE void detachSkill(const Skill *skill, SkillArea area = UnknownSkillArea);

    Q_INVOKABLE void broadcastTag(const QString &key);
    Q_INVOKABLE void unicastTagTo(const QString &key, ServerPlayer *to);

    Q_INVOKABLE QList<const General *> askForGeneral(const QList<const General *> &candidates, int num);

    Q_INVOKABLE static bool sortByActionOrder(ServerPlayer *a, ServerPlayer *b);

    Q_INVOKABLE GameLogic *logic() { return m_logic; }
    Q_INVOKABLE const GameLogic *gameLogic() const { return m_logic; }

private:
    void addTriggerSkill(const Skill *skill);
    void removeTriggerSkill(const Skill *skill);

    GameLogic *m_logic;
    CRoom *m_room;
    CServerAgent *m_agent;
    QSet<Phase> m_skippedPhase;
};

#endif // SERVERPLAYER_H
