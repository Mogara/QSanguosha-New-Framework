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

#include "hegemonymode.h"
#include "engine.h"
#include "gamelogic.h"
#include "gamerule.h"
#include "general.h"
#include "protocol.h"
#include "serverplayer.h"
#include "systempackage.h"
#include "util.h"

#include <CRoom>
#include <CServerAgent>

class HegemonyRule : public GameRule
{
public:
    HegemonyRule() : GameRule()
    {
        m_name = "hegemony_rule";
        m_events << GameStart;
        m_callbacks[GameStart] = &onGameStart;
    }

    void prepareToStart(GameLogic *logic) const override
    {
        //Choose 7 random generals for each player
        //@to-do: config
        QMap<uint, GeneralList> replies = logic->broadcastRequestForGenerals(logic->players(), 2, 7);
        QMapIterator<uint, GeneralList> iter(replies);
        while (iter.hasNext()) {
            iter.next();
            ServerPlayer *player = logic->findPlayer(iter.key());
            const GeneralList &generals = iter.value();
            player->setHeadGeneral(generals.at(0));
            player->setDeputyGeneral(generals.at(1));
        }
    }

    static void onGameStart(GameLogic *, ServerPlayer *current, QVariant &)
    {
        current->broadcastProperty("headGeneralId", SystemPackage::HiddenGeneralId(), current);
        current->broadcastProperty("deputyGeneralId", SystemPackage::HiddenGeneralId(), current);

        current->unicastPropertyTo("headGeneralId", current);
        current->unicastPropertyTo("deputyGeneralId", current);

        const General *headGeneral = current->headGeneral();
        const General *deputyGeneral = current->deputyGeneral();

        int headHp = headGeneral->headMaxHp();
        int deputyHp = deputyGeneral->deputyMaxHp();
        int hp = (headHp + deputyHp) / 2;
        current->setMaxHp(hp);
        current->setHp(hp);
        current->broadcastProperty("maxHp");
        current->broadcastProperty("hp");

        current->setKingdom(headGeneral->kingdom());
        current->unicastPropertyTo("kingdom", current);
        current->broadcastProperty("kingdom", "unknown", current);

        QList<const Skill *> skills = headGeneral->skills();
        foreach (const Skill *skill, skills)
            current->addSkill(skill, Player::HeadSkillArea);
        skills = deputyGeneral->skills();
        foreach (const Skill *skill, skills)
            current->addSkill(skill, Player::DeputySkillArea);

        current->drawCards(4);
    }
};

HegemonyMode::HegemonyMode()
{
    m_name = "hegemony";
    m_rule = new HegemonyRule;
}

ADD_MODE(Hegemony)
