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

#include "standardmode.h"
#include "engine.h"
#include "gamelogic.h"
#include "gamerule.h"
#include "general.h"
#include "package.h"
#include "protocol.h"
#include "serverplayer.h"
#include "util.h"

#include <croom.h>
#include <cserveragent.h>

class StandardRule : public GameRule
{
public:
    StandardRule() : GameRule()
    {
        m_name = "hegemony_rule";
        m_events << GameStart;

        m_callbacks[GameStart] = &onGameStart;
    }

    void prepareToStart(GameLogic *logic) const override
    {


        QMap<uint, GeneralList> replies = logic->broadcastRequestForGenerals(logic->players(), 1, 5);
        QMapIterator<uint, GeneralList> iter(replies);
        while (iter.hasNext()) {
            iter.next();
            ServerPlayer *player = logic->findPlayer(iter.key());
            const GeneralList &generals = iter.value();
            player->setGeneral(generals.first());
        }
    }

    static void onGameStart(GameLogic *, ServerPlayer *current, QVariant &)
    {
        current->broadcastProperty("generalId");

        const General *general = current->general();

        int hp = general->maxHp();
        current->setMaxHp(hp);
        current->setHp(hp);
        current->broadcastProperty("maxHp");
        current->broadcastProperty("hp");

        current->setKingdom(general->kingdom());
        current->broadcastProperty("kingdom");

        QList<const Skill *> skills = general->skills();
        foreach (const Skill *skill, skills)
            current->addSkill(skill);

        current->drawCards(4);
    }
};

StandardMode::StandardMode()
{
    m_name = "standard";
    m_rule = new StandardRule;
}

ADD_MODE(Standard)
