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

#include "gamemode.h"
#include "engine.h"
#include "eventhandler.h"

GameMode::GameMode()
    : m_minPlayerNum(2)
    , m_maxPlayerNum(10)
{
}

GameMode::~GameMode()
{
    delete m_rule;

    foreach (const EventHandler *rule, m_extraRules)
        delete rule;
}
