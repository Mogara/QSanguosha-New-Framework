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

#include <cabstractplayer.h>

#include <QList>
#include <QSet>

class Player : public CAbstractPlayer
{
    Q_OBJECT

    Q_ENUMS(Phase)

    Q_PROPERTY(QString screenName READ screenName WRITE setScreenName NOTIFY screenNameChanged)
    Q_PROPERTY(int hp READ hp WRITE setHp NOTIFY hpChanged)
    Q_PROPERTY(int maxHp READ maxHp WRITE setMaxHp NOTIFY maxHpChanged)
    Q_PROPERTY(int lostHp READ lostHp NOTIFY hpChanged)
    Q_PROPERTY(bool isWounded READ isWounded)
    Q_PROPERTY(bool isAlive READ isAlive WRITE setAlive NOTIFY aliveChanged)
    Q_PROPERTY(bool isDead READ isDead WRITE setDead NOTIFY aliveChanged)
    Q_PROPERTY(bool isRemoved READ isRemoved WRITE setRemoved NOTIFY removedChanged)
    Q_PROPERTY(int seat READ seat WRITE setSeat NOTIFY seatChanged)
    Q_PROPERTY(Phase phase READ phase WRITE setPhase NOTIFY phaseChanged)
    Q_PROPERTY(QString generalName READ generalName NOTIFY generalChanged)
    Q_PROPERTY(QString headGeneralName READ headGeneralName NOTIFY headGeneralChanged)
    Q_PROPERTY(QString deputyGeneralName READ deputyGeneralName NOTIFY deputyGeneralChanged)
    Q_PROPERTY(int handcardNum READ handcardNum NOTIFY handCardNumChanged)
    Q_PROPERTY(int equipNum READ equipNum NOTIFY equipNumChanged)
    Q_PROPERTY(int delayedTrickNum READ delayedTrickNum NOTIFY delayedTrickNumChanged)
    Q_PROPERTY(int turnCount READ turnCount WRITE setTurnCount)
    Q_PROPERTY(int faceUp READ faceUp WRITE setFaceUp NOTIFY faceUpChanged)

    Q_ENUMS(Phase)
    Q_ENUMS(Role)

public:
    enum Phase
    {
        InvalidPhase, RoundStart, Start, Judge, Draw, Play, Discard, Finish, NotActive
    };

    enum Role
    {
        Lord, Loyalist, Rebel, Renegade
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

    bool hasSkill(const EventHandler *skill) const;
    bool hasShownSkill(const EventHandler *skill) const;

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

    //Alias of head general.
    const General *general() const { return headGeneral(); }
    QString generalName() const { return headGeneralName(); }
    void setGeneral(const General *general) { setHeadGeneral(general); }

    const General *headGeneral() const { return m_headGeneral; }
    QString headGeneralName() const;
    void setHeadGeneral(const General *general);

    const General *deputyGeneral() const { return m_deputyGeneral; }
    QString deputyGeneralName() const;
    void setDeputyGeneral(const General *general);

    bool hasShownHeadGeneral() const { return m_headGeneralShown; }
    void setHeadGeneralShown(bool shown) { m_headGeneralShown = shown; }

    bool hasShownDeputyGeneral() const { return m_deputyGeneralShown; }
    void setDeputyGeneralShown(bool shown) { m_deputyGeneralShown = shown; }

    bool hasShownGeneral() const { return hasShownHeadGeneral() || hasShownDeputyGeneral(); }
    bool hasShownBothGenerals() const { return hasShownHeadGeneral() && hasShownDeputyGeneral(); }

    int turnCount() const { return m_turnCount; }
    void setTurnCount(int count) { m_turnCount = count; }

    bool faceUp() const { return m_faceUp; }
    void setFaceUp(bool faceUp);

    CardArea *handcards() { return m_handCards; }
    const CardArea *handcards() const { return m_handCards; }
    int handcardNum() const;

    const CardArea *equips() const { return m_equips; }
    CardArea *equips() { return m_equips; }
    int equipNum() const;

    CardArea *delayedTricks() { return m_delayedTricks; }
    const CardArea *delayedTricks() const { return m_delayedTricks; }
    int delayedTrickNum() const;

    CardArea *judgeCards() { return m_judgeCards; }
    const CardArea *judgeCards() const { return m_judgeCards; }

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
    void handCardNumChanged();
    void equipNumChanged();
    void delayedTrickNumChanged();

protected:
    QString m_screenName;
    int m_hp;
    int m_maxHp;
    bool m_alive;
    bool m_removed;
    int m_seat;
    Player *m_next;
    Phase m_phase;
    const General *m_headGeneral;
    const General *m_deputyGeneral;
    bool m_headGeneralShown;
    bool m_deputyGeneralShown;
    int m_turnCount;
    bool m_faceUp;

    CardArea *m_handCards;
    CardArea *m_equips;
    CardArea *m_delayedTricks;
    CardArea *m_judgeCards;
};

#endif // PLAYER_H
