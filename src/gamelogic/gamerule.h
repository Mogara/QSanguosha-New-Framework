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

#ifndef GAMERULE_H
#define GAMERULE_H

#include "eventhandler.h"

class GameLogic;

class GameRule : public EventHandler
{
public:
    GameRule(GameLogic *logic);

    bool triggerable(Player *) const;
    bool effect(GameLogic *logic, EventType event, Player *current, QVariant &data, Player *) const;

protected:
    void onGameStart(GameLogic *logic) const;

    GameLogic *m_logic;
};

#endif // GAMERULE_H
