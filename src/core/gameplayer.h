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

#ifndef CGAMEPLAYER_H
#define CGAMEPLAYER_H

#include <cabstractgameplayer.h>

class GamePlayer : public CAbstractGamePlayer
{
    Q_OBJECT

    Q_PROPERTY(int hp READ hp WRITE setHp)
    Q_PROPERTY(int maxHp READ maxHp WRITE setMaxHp)

public:
    GamePlayer(QObject *parent = 0);

    int hp() const { return m_hp; }
    void setHp(int hp) { m_hp = hp; }

    int maxHp() const { return m_maxHp; }
    void setMaxHp(int maxHp) { m_maxHp = maxHp; }

protected:
    int m_hp;
    int m_maxHp;
};

#endif // CGAMEPLAYER_H
