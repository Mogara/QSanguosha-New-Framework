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

class EventHandler;
class General;

#include <cabstractplayer.h>

class Player : public CAbstractPlayer
{
    Q_OBJECT

    Q_PROPERTY(int hp READ hp WRITE setHp)
    Q_PROPERTY(int maxHp READ maxHp WRITE setMaxHp)

    Q_ENUMS(Phase)
    Q_ENUMS(Area)
    Q_ENUMS(Role)

public:
    enum Phase
    {
        InvalidPhase, RoundStart, Start, Judge, Draw, Play, Discard, Finish, NotActive
    };

    enum Area
    {
        UnknownArea,
        HandArea, EquipArea, DelayedTrickArea, JudgeArea,
        DiscardPile, DrawPileTop, DrawPileBottom,
        SpecialArea, TableArea, WuguArea
    };

    enum Role
    {
        Lord, Loyalist, Rebel, Renegade
    };

    Player(QObject *parent = 0);

    int hp() const { return m_hp; }
    void setHp(int hp);

    int maxHp() const { return m_maxHp; }
    void setMaxHp(int maxHp);

    int lostHp() const { return maxHp() - qMax(hp(), 0); }
    bool isWounded() const { return hp() < 0 || hp() < maxHp(); }
    bool isAlive() const { return m_alive; }
    bool isDead() const { return !m_alive; }

    bool hasSkill(const EventHandler *skill) const;
    bool hasShownSkill(const EventHandler *skill) const;

    void setRemoved(bool removed) { m_removed = removed; }
    bool isRemoved() const { return m_removed; }

    void setNext(Player *next) { m_next = next; }
    Player *next() const { return m_next; }
    Player *next(bool ignoreRemoved) const;
    Player *nextAlive(int step = 1, bool ignoreRemoved = true) const;

    void setPhase(Phase phase);
    Phase phase() const { return m_phase; }

    //Alias of head general.
    const General *general() const { return headGeneral(); }
    void setGeneral(const General *general) { setHeadGeneral(general); }

    const General *headGeneral() const { return m_headGeneral; }
    void setHeadGeneral(const General *general) { m_headGeneral = general; }

    const General *deputyGeneral() const { return m_deputyGeneral; }
    void setDeputyGeneral(const General *general) { m_deputyGeneral = general; }

    bool hasShownHeadGeneral() const { return m_headGeneralShown; }
    void setHeadGeneralShown(bool shown) { m_headGeneralShown = shown; }

    bool hasShownDeputyGeneral() const { return m_deputyGeneralShown; }
    void setDeputyGeneralShown(bool shown) { m_deputyGeneralShown = shown; }

    bool hasShownGeneral() const { return hasShownHeadGeneral() || hasShownDeputyGeneral(); }
    bool hasShownBothGenerals() const { return hasShownHeadGeneral() && hasShownDeputyGeneral(); }

signals:
    void hpChanged();
    void maxHpChanged();
    void phaseChanged();

protected:
    int m_hp;
    int m_maxHp;
    bool m_alive;
    bool m_removed;
    Player *m_next;
    Phase m_phase;
    const General *m_headGeneral;
    const General *m_deputyGeneral;
    bool m_headGeneralShown;
    bool m_deputyGeneralShown;
};

#endif // PLAYER_H
