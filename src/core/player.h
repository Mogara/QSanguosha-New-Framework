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
    void setHp(int hp) { m_hp = hp; }

    int maxHp() const { return m_maxHp; }
    void setMaxHp(int maxHp) { m_maxHp = maxHp; }

    int lostHp() const { return maxHp() - qMax(hp(), 0); }
    bool isWounded() const { return hp() < 0 || hp() < maxHp(); }

protected:
    int m_hp;
    int m_maxHp;
};

#endif // PLAYER_H
