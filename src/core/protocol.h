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

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <cprotocol.h>

enum SanguoshaCommand
{
    S_COMMAND_INVALID_SANGUOSHA_COMMAND = CARDIRECTOR_SYSTEM_COMMAND_COUNT,

    S_COMMAND_SHOW_PROMPT,
    S_COMMAND_ARRANGE_SEAT,
    S_COMMAND_PREPARE_CARDS,
    S_COMMAND_UPDATE_PLAYER_PROPERTY,
    S_COMMAND_CHOOSE_GENERAL,
    S_COMMAND_MOVE_CARDS,
    S_COMMAND_USE_CARD,
    S_COMMAND_ADD_CARD_HISTORY,
    S_COMMAND_DAMAGE,
    S_COMMAND_RECOVER,
    S_COMMAND_ASK_FOR_CARD,
    S_COMMAND_SHOW_AMAZING_GRACE,
    S_COMMAND_TAKE_AMAZING_GRACE,
    S_COMMAND_CLEAR_AMAZING_GRACE,
    S_COMMAND_CHOOSE_PLAYER_CARD,

    SANGUOSHA_COMMAND_COUNT
};

#endif // PROTOCOL_H
