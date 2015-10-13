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

#ifndef PLAYER_H
#define PLAYER_H

class Card;
class CardArea;
class EventHandler;
class General;
class Skill;

#include <cabstractplayer.h>

#include <QList>
#include <QSet>
#include <QMap>
#include <QHash>

class Player : public CAbstractPlayer
{
    Q_OBJECT

    Q_ENUMS(Phase)

    Q_PROPERTY(QString screenName READ screenName WRITE setScreenName NOTIFY screenNameChanged)
    Q_PROPERTY(int hp READ hp WRITE setHp NOTIFY hpChanged)
    Q_PROPERTY(int maxHp READ maxHp WRITE setMaxHp NOTIFY maxHpChanged)
    Q_PROPERTY(int lostHp READ lostHp NOTIFY hpChanged)
    Q_PROPERTY(bool wounded READ isWounded)
    Q_PROPERTY(bool alive READ isAlive WRITE setAlive NOTIFY aliveChanged)
    Q_PROPERTY(bool dead READ isDead WRITE setDead NOTIFY aliveChanged)
    Q_PROPERTY(bool removed READ isRemoved WRITE setRemoved NOTIFY removedChanged)
    Q_PROPERTY(int seat READ seat WRITE setSeat NOTIFY seatChanged)
    Q_PROPERTY(QString phase READ phaseString WRITE setPhaseString NOTIFY phaseChanged)
    Q_PROPERTY(int handcardNum READ handcardNum NOTIFY handcardNumChanged)
    Q_PROPERTY(int equipNum READ equipNum NOTIFY equipNumChanged)
    Q_PROPERTY(int delayedTrickNum READ delayedTrickNum NOTIFY delayedTrickNumChanged)
    Q_PROPERTY(int turnCount READ turnCount WRITE setTurnCount)
    Q_PROPERTY(int faceUp READ faceUp WRITE setFaceUp NOTIFY faceUpChanged)
    Q_PROPERTY(bool drunk READ isDrunk WRITE setDrunk NOTIFY drunkChanged)
    Q_PROPERTY(QString kingdom READ kingdom WRITE setKingdom NOTIFY kingdomChanged)
    Q_PROPERTY(QString role READ role WRITE setRole NOTIFY roleChanged)
    Q_PROPERTY(int attackRange READ attackRange WRITE setAttackRange NOTIFY attackRangeChanged)
    Q_PROPERTY(bool chained READ isChained WRITE setChained NOTIFY chainedChanged)
    Q_PROPERTY(bool dying READ isDying WRITE setDying NOTIFY dyingChanged)
    Q_PROPERTY(QString generalName READ generalName WRITE setGeneralName NOTIFY generalChanged)
    Q_PROPERTY(QString headGeneralName READ headGeneralName WRITE setHeadGeneralName NOTIFY headGeneralChanged)
    Q_PROPERTY(QString deputyGeneralName READ deputyGeneralName WRITE setDeputyGeneralName NOTIFY deputyGeneralChanged)
    Q_PROPERTY(int extraOutDistance READ extraOutDistance WRITE setExtraOutDistance)
    Q_PROPERTY(int extraInDistance READ extraInDistance WRITE setExtraInDistance)

public:
    enum Phase
    {
        InvalidPhase, RoundStart, Start, Judge, Draw, Play, Discard, Finish, Inactive
    };

    Player(QObject *parent = 0);
    ~Player();

    QString screenName() const { return m_screenName; }
    void setScreenName(const QString &name);

    int hp() const { return m_hp; }
    void setHp(int hp);

    int maxHp() const { return m_maxHp; }
    void setMaxHp(int maxHp);

    int lostHp() const { return maxHp() - qMax(hp(), 0); }
    bool isWounded() const { return hp() < 0 || hp() < maxHp(); }

    void setAlive(bool alive);
    bool isAlive() const { return m_alive; }
    void setDead(bool dead) { setAlive(!dead); }
    bool isDead() const { return !m_alive; }

    void setRemoved(bool removed);
    bool isRemoved() const { return m_removed; }

    void setSeat(int seat);
    int seat() const { return m_seat; }

    void setNext(Player *next) { m_next = next; }
    Player *next() const { return m_next; }
    Player *next(bool ignoreRemoved) const;
    Player *nextAlive(int step = 1, bool ignoreRemoved = true) const;

    void setPhase(Phase phase);
    Phase phase() const { return m_phase; }
    void setPhaseString(const QString &phase);
    QString phaseString() const;

    int turnCount() const { return m_turnCount; }
    void setTurnCount(int count) { m_turnCount = count; }

    bool faceUp() const { return m_faceUp; }
    void setFaceUp(bool faceUp);

    void setDrunk(bool drunk);
    bool isDrunk() const { return m_drunk; }

    QString kingdom() const { return m_kingdom; }
    void setKingdom(const QString &kingdom);

    QString role() const { return m_role; }
    void setRole(const QString &role);

    int attackRange() const { return m_attackRange; }
    void setAttackRange(int range);

    bool isChained() const { return m_chained; }
    void setChained(bool chained);

    bool isDying() const { return m_dying; }
    void setDying(bool dying);

    //Alias of head general.
    const General *general() const { return headGeneral(); }
    void setGeneral(const General *general) { setHeadGeneral(general); }
    QString generalName() const { return headGeneralName(); }
    void setGeneralName(const QString &name) { setHeadGeneralName(name); }

    const General *headGeneral() const { return m_headGeneral; }
    void setHeadGeneral(const General *general);
    QString headGeneralName() const;
    void setHeadGeneralName(const QString &name);

    const General *deputyGeneral() const { return m_deputyGeneral; }
    void setDeputyGeneral(const General *general);
    QString deputyGeneralName() const;
    void setDeputyGeneralName(const QString &name);

    bool hasShownHeadGeneral() const { return m_headGeneralShown; }
    void setHeadGeneralShown(bool shown) { m_headGeneralShown = shown; }

    bool hasShownDeputyGeneral() const { return m_deputyGeneralShown; }
    void setDeputyGeneralShown(bool shown) { m_deputyGeneralShown = shown; }

    bool hasShownGeneral() const { return hasShownHeadGeneral() || hasShownDeputyGeneral(); }
    bool hasShownBothGenerals() const { return hasShownHeadGeneral() && hasShownDeputyGeneral(); }

    int cardHistory(const QString &name) const { return m_cardHistory.value(name); }
    void addCardHistory(const QString &name, int times = 1);
    void clearCardHistory() { m_cardHistory.clear(); }

    int distanceTo(const Player *other) const;
    void setFixedDistance(const Player *other, int distance) { m_fixedDistance[other] = distance; }
    void unsetFixedDistance(const Player *other) { m_fixedDistance.remove(other); }

    //Extra distance from you to other players
    int extraOutDistance() const { return m_extraOutDistance; }
    void setExtraOutDistance(int extra) { m_extraOutDistance = extra; }

    //Extra distance from other players to you
    int extraInDistance() const { return m_extraInDistance; }
    void setExtraInDistance(int extra) { m_extraInDistance = extra; }

    CardArea *handcards() { return m_handcards; }
    const CardArea *handcards() const { return m_handcards; }
    int handcardNum() const { return m_handcardNum; }
    bool hasHandcard() const { return handcardNum() > 0; }

    const CardArea *equips() const { return m_equips; }
    CardArea *equips() { return m_equips; }
    int equipNum() const;
    bool hasEquip() const { return equipNum() > 0; }

    CardArea *delayedTricks() { return m_delayedTricks; }
    const CardArea *delayedTricks() const { return m_delayedTricks; }
    int delayedTrickNum() const;

    bool isNude() const { return !hasHandcard() && !hasEquip(); }
    bool isAllNude() const { return isNude() && delayedTrickNum() <= 0; }

    CardArea *judgeCards() { return m_judgeCards; }
    const CardArea *judgeCards() const { return m_judgeCards; }

    bool hasSkill(const Skill *skill) const;
    bool hasShownSkill(const Skill *skill) const;

    void addSkill(const Skill *skill) { addHeadSkill(skill); }
    void removeSkill(const Skill *skill);
    QList<const Skill *> skills() const { return m_headSkills + m_deputySkills; }

    void addHeadSkill(const Skill *skill) { m_headSkills << skill; }
    void removeHeadSkill(const Skill *skill) { m_headSkills.removeOne(skill); }
    QList<const Skill *> headSkills() const { return m_headSkills; }

    void addDeputySkill(const Skill *skill) { m_deputySkills << skill; }
    void removeDeputySkill(const Skill *skill) { m_deputySkills.removeOne(skill); }
    QList<const Skill *> deputySkills() const { return m_deputySkills; }

    void addAcquiredSkill(const Skill *skill) { m_acquiredSkills << skill; }
    void removeAcquiredSkill(const Skill *skill) { m_acquiredSkills.removeOne(skill); }
    QList<const Skill *> acquiredSkills() const { return m_acquiredSkills; }

signals:
    void screenNameChanged();
    void hpChanged();
    void maxHpChanged();
    void lostHpChanged();
    void woundedChanged();
    void aliveChanged();
    void deadChanged();
    void removedChanged();
    void seatChanged();
    void phaseChanged();
    void generalChanged();
    void headGeneralChanged();
    void deputyGeneralChanged();
    void faceUpChanged();
    void handcardNumChanged();
    void equipNumChanged();
    void delayedTrickNumChanged();
    void drunkChanged();
    void kingdomChanged();
    void roleChanged();
    void attackRangeChanged();
    void chainedChanged();
    void dyingChanged();

protected:
    QString m_screenName;
    int m_hp;
    int m_maxHp;
    bool m_alive;
    bool m_removed;
    int m_seat;
    Player *m_next;
    Phase m_phase;
    int m_turnCount;
    bool m_faceUp;
    int m_handcardNum;
    bool m_drunk;
    QString m_kingdom;
    QString m_role;
    int m_attackRange;
    bool m_chained;
    bool m_dying;

    const General *m_headGeneral;
    const General *m_deputyGeneral;
    bool m_headGeneralShown;
    bool m_deputyGeneralShown;

    QMap<const Player *, int> m_fixedDistance;
    int m_extraOutDistance;
    int m_extraInDistance;

    QHash<QString, int> m_cardHistory;
    CardArea *m_handcards;
    CardArea *m_equips;
    CardArea *m_delayedTricks;
    CardArea *m_judgeCards;

    QList<const Skill *> m_headSkills;
    QList<const Skill *> m_deputySkills;
    QList<const Skill *> m_acquiredSkills;
};

#endif // PLAYER_H
