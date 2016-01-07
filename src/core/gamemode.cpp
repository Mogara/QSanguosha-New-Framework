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

#include <QCoreApplication>

static void registerGameMode()
{
    Engine *engine = Engine::instance();
    engine->addMode(new GameMode("standard", 2, 10));
    engine->addMode(new GameMode("hegemony", 2, 10));
    engine->addMode(new GameMode("1v1", 2, 2));
    engine->addMode(new GameMode("3v3", 6, 6));
}

Q_COREAPP_STARTUP_FUNCTION(registerGameMode)
