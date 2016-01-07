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

#ifndef GAMEMODE_H
#define GAMEMODE_H

#include <QString>

struct GameMode
{
    const QString name;
    const int minPlayerNum;
    const int maxPlayerNum;

    GameMode(const QString &name, int minPlayerNum, int maxPlayerNum)
        : name(name)
        , minPlayerNum(minPlayerNum)
        , maxPlayerNum(maxPlayerNum)
    {
    }
};

#endif // GAMEMODE_H
