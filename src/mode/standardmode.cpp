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

#include <CRoom>
#include <CServerAgent>

class StandardRule : public GameRule
{
public:
    StandardRule() : GameRule()
    {
        m_name = "standard_rule";
        m_events << GameStart << BuryVictim << GameOverJudge;

        m_callbacks[GameStart] = onGameStart;
        m_callbacks[BuryVictim] = onBuryVictim;
        m_callbacks[GameOverJudge] = onGameOverJudge;
    }

    void prepareToStart(GameLogic *logic) const override
    {
        QList<ServerPlayer *> players = logic->players();
        qShuffle(players);
        int playerNum = players.length();
        ServerPlayer *lord = players.first();
        players.removeFirst();

        lord->setRole("lord");
        lord->broadcastProperty("role");

        int renegadeNum = playerNum > 4 ? 1 : 0;
        int rebelNum = playerNum / 2;
        int loyalistNum = playerNum - 1 - renegadeNum - rebelNum;

        QMap<QString, int> roleMap;
        roleMap["renagade"] = renegadeNum;
        roleMap["rebel"] = rebelNum;
        roleMap["loyalist"] = loyalistNum;

        int playerIndex = 0;
        for (QMapIterator<QString, int> iter(roleMap); iter.hasNext(); ) {
            iter.next();
            int num = iter.value();
            for (int i = 0; i < num; i++) {
                ServerPlayer *player = players.at(playerIndex);
                playerIndex++;
                player->setRole(iter.key());
            }
        }
        foreach (ServerPlayer *player, players) {
            player->unicastPropertyTo("role", player);
            player->broadcastProperty("role", "unknown");
        }

        GeneralList generals;
        QList<const Package *> packages = logic->packages();
        foreach (const Package *package, packages)
            generals << package->generals();
        qShuffle(generals);

        GeneralList lordCandidates;
        foreach (const General *general, generals) {
            if (general->isLord())
                lordCandidates << general;
        }
        lordCandidates << generals.mid(0, 2);
        GeneralList reply = lord->askForGeneral(lordCandidates, 1);
        lord->setGeneral(reply.first());
        lord->broadcastProperty("generalId");

        QMap<uint, GeneralList> replies = logic->broadcastRequestForGenerals(players, 1, 5);
        for (QMapIterator<uint, GeneralList> iter(replies); iter.hasNext(); ) {
            iter.next();
            ServerPlayer *player = logic->findPlayer(iter.key());
            const GeneralList &generals = iter.value();
            player->setGeneral(generals.first());
            player->broadcastProperty("generalId");
        }
    }

private:
    static void onGameStart(GameLogic *, ServerPlayer *current, QObject *)
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

    static QList<ServerPlayer *> getPlayersByRole(GameLogic *logic, const QString &role)
    {
        QList<ServerPlayer *> result;
        QList<ServerPlayer *> allPlayers = logic->allPlayers(true);
        foreach (ServerPlayer *player, allPlayers) {
            if (player->role() == role)
                result << player;
        }
        return result;
    }

    static void onBuryVictim(GameLogic *logic, ServerPlayer *victim, QObject *data)
    {
        DeathValue *death = qobject_cast<DeathValue *>(data);
        if (death->damage == nullptr)
            return;

        ServerPlayer *killer = death->damage->from;
        if (killer->isDead())
            return;

        if (victim->role() == "rebel") {
            killer->drawCards(3);
        } else if (victim->role() == "loyalist" && killer->role() == "lord") {
            CardsMoveValue moves;
            foreach (Card *c, killer->handcardArea()->cards() + killer->equipArea()->virtualCards()) {
                CardMove move;
                move.card = c;
                move.toArea = logic->discardPile();
                move.isOpen = true;
                moves.moves << move;
            }
            logic->moveCards(moves);
        }
    }

    static void onGameOverJudge(GameLogic *logic, ServerPlayer *victim, QObject *)
    {
        QList<ServerPlayer *> winners;
        QList<ServerPlayer *> alivePlayers = logic->allPlayers();
        if (victim->role() == "lord") {
            if (alivePlayers.length() == 1 && alivePlayers.first()->role() == "renagade") {
                winners = alivePlayers;
            } else {
                winners = getPlayersByRole(logic, "rebel");
            }
        } else {
            victim->broadcastProperty("role");

            bool lordWin = true;
            foreach (ServerPlayer *player, alivePlayers) {
                if (player->role() == "rebel" || player->role() == "renagade") {
                    lordWin = false;
                    break;
                }
            }
            if (lordWin) {
                winners << getPlayersByRole(logic, "lord");
                winners << getPlayersByRole(logic, "loyalist");
            }
        }

        if (winners.length() > 0) {
            foreach (ServerPlayer *player, alivePlayers)
                player->broadcastProperty("role");

            ServerPlayer *current = logic->currentPlayer();
            current->setPhase(Player::Inactive);
            current->broadcastProperty("phase");

            logic->gameOver(winners);
        }
    }
};

StandardMode::StandardMode()
{
    m_name = "standard";
    m_rule = new StandardRule;

    m_availablePackages << "nos_standard" << "standard_cards";
}

ADD_MODE(Standard)
