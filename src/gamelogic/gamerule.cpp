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

#include "card.h"
#include "gamelogic.h"
#include "gamerule.h"
#include "general.h"
#include "serverplayer.h"

#include <QThread>

QMap<EventType, GameRule::Callback> GameRule::m_callbacks;

GameRule::GameRule(GameLogic *logic)
    : m_logic(logic)
{
    m_events << GameStart;
    m_events << TurnStart << PhaseProceeding << PhaseEnd;
}

bool GameRule::triggerable(ServerPlayer *) const
{
    return true;
}

bool GameRule::effect(GameLogic *logic, EventType event, ServerPlayer *current, QVariant &data, Player *) const
{
    if (logic->skipGameRule())
        return false;

    Callback func = m_callbacks[event];
    if (func)
        (this->*func)(current, data);
    else
        throw GameFinish;

    return false;
}

void GameRule::onGameStart(ServerPlayer *current, QVariant &) const
{
    current->broadcastProperty("headGeneralName", "anjiang", current);
    current->broadcastProperty("deputyGeneralName", "anjiang", current);

    current->unicastPropertyTo("headGeneralName", current);
    current->unicastPropertyTo("deputyGeneralName", current);

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

    current->drawCards(4);
}

void GameRule::onTurnStart(ServerPlayer *current, QVariant &) const
{
    current->setTurnCount(current->turnCount() + 1);
    if (!current->faceUp())
        current->setFaceUp(true);
    else
        current->play();
}

void GameRule::onPhaseProceeding(ServerPlayer *current, QVariant &) const
{
    m_logic->delay(500);
    switch (current->phase()) {
    case Player::Judge: {
        QList<Card *> tricks = current->delayedTricks()->cards();
        while (tricks.length() > 0 && current->isAlive()) {
            Card *trick = tricks.takeLast();

            CardUseStruct use;
            use.card = trick;
            use.to << current;
            CardEffectStruct effect(use);
            effect.to = current;
            m_logic->takeCardEffect(effect);
            trick->complete(m_logic);
        }
        break;
    }
    case Player::Draw: {
        int num = 2;
        QVariant data(num);
        m_logic->trigger(DrawNCards, current, data);
        num = data.toInt();
        if (num > 0)
            current->drawCards(num);
        data = num;
        m_logic->trigger(AfterDrawNCards, current, data);
        break;
    }
    case Player::Play: {
        while (current->isAlive()) {
            CardUseStruct use;
            current->activate(use);
            if (use.card != nullptr) {
                if (use.card->canRecast() && use.target == nullptr && use.to.isEmpty())
                    current->recastCard(use.card);
                else
                    m_logic->useCard(use);
            } else {
                break;
            }
        }
        break;
    }
    case Player::Discard: {
        int maxCardNum = current->hp();
        QVariant data(maxCardNum);
        m_logic->trigger(CountMaxCardNum, current, data);
        maxCardNum = data.toInt();
        int discardNum = current->handcardNum() - maxCardNum;
        if (discardNum > 0) {
            current->showPrompt("ask_to_discard", QVariantList() << discardNum);
            QList<Card *> cards = current->askForCards(".|.|.|hand", discardNum);

            CardsMoveStruct move;
            move.cards = cards;
            move.to.type = CardArea::DiscardPile;
            move.isOpen = true;
            m_logic->moveCards(move);
        }
        break;
    }
    default:;
    }
}

void GameRule::onPhaseEnd(ServerPlayer *current, QVariant &) const
{
    switch (current->phase()) {
    case Player::Play: {
        current->clearCardHistory();
    }
    default:;
    }
}

#define ADD_GAMERULE(name) m_callbacks[name] = &GameRule::on##name
void GameRule::Init()
{
    ADD_GAMERULE(GameStart);
    ADD_GAMERULE(TurnStart);
    ADD_GAMERULE(PhaseProceeding);
    ADD_GAMERULE(PhaseEnd);
}
C_INITIALIZE_CLASS(GameRule)
