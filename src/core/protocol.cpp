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

#include "protocol.h"

static void registerSanguoshaCommand()
{
    C_REGISTER_COMMAND(SHOW_PROMPT);
    C_REGISTER_COMMAND(ARRANGE_SEAT);
    C_REGISTER_COMMAND(PREPARE_CARDS);
    C_REGISTER_COMMAND(UPDATE_PLAYER_PROPERTY);
    C_REGISTER_COMMAND(CHOOSE_GENERAL);
    C_REGISTER_COMMAND(MOVE_CARDS);
    C_REGISTER_COMMAND(USE_CARD);
    C_REGISTER_COMMAND(ADD_CARD_HISTORY);
    C_REGISTER_COMMAND(DAMAGE);
    C_REGISTER_COMMAND(LOSE_HP);
    C_REGISTER_COMMAND(RECOVER);
    C_REGISTER_COMMAND(ASK_FOR_CARD);
    C_REGISTER_COMMAND(SHOW_AMAZING_GRACE);
    C_REGISTER_COMMAND(TAKE_AMAZING_GRACE);
    C_REGISTER_COMMAND(CLEAR_AMAZING_GRACE);
    C_REGISTER_COMMAND(CHOOSE_PLAYER_CARD);
    C_REGISTER_COMMAND(SHOW_CARD);
    C_REGISTER_COMMAND(ADD_SKILL);
    C_REGISTER_COMMAND(REMOVE_SKILL);
    C_REGISTER_COMMAND(INVOKE_SKILL);
    C_REGISTER_COMMAND(CLEAR_SKILL_HISTORY);
    C_REGISTER_COMMAND(TRIGGER_ORDER);
    C_REGISTER_COMMAND(ARRANGE_CARD);
    C_REGISTER_COMMAND(ARRANGE_CARD_START);
    C_REGISTER_COMMAND(ARRANGE_CARD_MOVE);
    C_REGISTER_COMMAND(ARRANGE_CARD_END);
    C_REGISTER_COMMAND(ASK_FOR_OPTION);
    C_REGISTER_COMMAND(SET_VIRTUAL_CARD);
    C_REGISTER_COMMAND(SET_PLAYER_TAG);
}
Q_COREAPP_STARTUP_FUNCTION(registerSanguoshaCommand)
