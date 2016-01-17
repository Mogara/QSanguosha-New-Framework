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
        //Choose 7 random generals for each player
        //@to-do: config
        QList<const General *> generals;
        QList<const Package *> packages = logic->packages();
        foreach(const Package *package, packages)
            generals << package->generals();
        qShuffle(generals);

        QList<ServerPlayer *> players = logic->players();
        int candidateLimit = 5;
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

            const General *result = nullptr;

            CServerAgent *agent = logic->findAgent(player);
            if (agent) {
                QVariantList reply = agent->waitForReply(0).toList();
                if (reply.length() > 0) {
                    const QVariant &choice = reply.first();
                    uint id = choice.toUInt();
                    foreach (const General *general, candidates) {
                        if (general->id() == id) {
                            result = general;
                            break;
                        }
                    }
                }
            }

            if (result == nullptr)
                result = candidates.first();

            player->setGeneral(result);
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

        //@to-do: arrange roles
        //current->setRole(general->kingdom());
        //current->unicastPropertyTo("role", current);
        current->broadcastProperty("role", "unknown");

        QList<const Skill *> skills = general->skills();
        foreach (const Skill *skill, skills)
            current->addHeadSkill(skill);

        current->drawCards(4);
    }
};

StandardMode::StandardMode()
{
    m_name = "standard";
    m_rule = new StandardRule;
}

ADD_MODE(Standard)
