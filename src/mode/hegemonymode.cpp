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

#include <croom.h>
#include <cserveragent.h>

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
        QList<const General *> generals;
        QList<const Package *> packages = logic->packages();
        foreach(const Package *package, packages)
            generals << package->generals();
        qShuffle(generals);

        QList<ServerPlayer *> players = logic->players();
        int candidateLimit = 7;
        int minCandidateNum = candidateLimit * players.length();
        while (minCandidateNum > generals.length())
            generals << generals.mid(0, minCandidateNum - generals.length());

        QMap<ServerPlayer *, QList<const General *>> playerCandidates;

        foreach (ServerPlayer *player, players) {
            QList<const General *> candidates = generals.mid((player->seat() - 1) * candidateLimit, candidateLimit);
            playerCandidates[player] = candidates;

            QVariantList candidateData;
            foreach (const General *general, candidates)
                candidateData << general->id();

            QVariantList bannedPairData;
            //@todo: load banned pairs

            QVariantList data;
            data << QVariant(candidateData);
            data << QVariant(bannedPairData);

            CServerAgent *agent = logic->findAgent(player);
            agent->prepareRequest(S_COMMAND_CHOOSE_GENERAL, data);
        }

        //@to-do: timeout should be loaded from config
        CRoom *room = logic->room();
        room->broadcastRequest(room->agents(), 15000);

        foreach (ServerPlayer *player, players) {
            const QList<const General *> &candidates = playerCandidates[player];
            QList<const General *> generals;

            CServerAgent *agent = logic->findAgent(player);
            if (agent) {
                QVariantList reply = agent->waitForReply(0).toList();
                foreach (const QVariant &choice, reply) {
                    uint id = choice.toUInt();
                    foreach (const General *general, candidates) {
                        if (general->id() == id)
                            generals << general;
                    }
                }
            }

            //@to-do: handle banned pairs
            if (generals.length() < 2)
                generals = candidates.mid(0, 2);

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

        current->setRole(headGeneral->kingdom());
        current->unicastPropertyTo("role", current);
        current->setKingdom(headGeneral->kingdom());
        current->unicastPropertyTo("kingdom", current);

        QList<const Skill *> skills = headGeneral->skills();
        foreach (const Skill *skill, skills)
            current->addHeadSkill(skill);
        skills = deputyGeneral->skills();
        foreach (const Skill *skill, skills)
            current->addDeputySkill(skill);

        current->drawCards(4);
    }
};

HegemonyMode::HegemonyMode()
{
    m_name = "hegemony";
    m_rule = new HegemonyRule;
}

ADD_MODE(Hegemony)
