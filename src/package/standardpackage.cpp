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

#include "standardpackage.h"
#include "engine.h"
#include "gamemode.h"

StandardPackage::StandardPackage()
    : Package("standard")
{
    addShuGenerals();
    addWeiGenerals();
    addWuGenerals();
    addQunGenerals();

    addBasicCards();
    addEquipCards();
    addTrickCards();
}

bool StandardPackage::isAvailable(const GameMode *mode) const
{
    return mode->name() == "standard";
}

ADD_PACKAGE(Standard)
