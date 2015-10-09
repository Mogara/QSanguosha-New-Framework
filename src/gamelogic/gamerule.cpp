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
    m_events << AfterHpReduced << AskForPeach << AskForPeachDone;
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
            current->showPrompt("ask_to_discard", discardNum);
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
    if (current->isDrunk()) {
        current->setDrunk(false);
        current->broadcastProperty("drunk");
    }
}

void GameRule::onAfterHpReduced(ServerPlayer *victim, QVariant &data) const
{
    if (victim->hp() > 0)
        return;

    victim->setDying(true);
    victim->broadcastProperty("dying");

    DeathStruct death;
    death.who = victim;
    death.damage = data.value<DamageStruct *>();

    QVariant dyingData = QVariant::fromValue(&death);

    QList<ServerPlayer *> allPlayers = m_logic->allPlayers();
    foreach (ServerPlayer *player, allPlayers) {
        if (m_logic->trigger(EnterDying, player, dyingData) || victim->hp() > 0 || victim->isDead())
            break;
    }

    if (victim->isAlive() && victim->hp() <= 0) {
        QList<ServerPlayer *> allPlayers = m_logic->allPlayers();
        foreach (ServerPlayer *saver, allPlayers) {
            if (victim->hp() > 0 || victim->isDead())
                break;

            m_logic->trigger(AskForPeach, saver, dyingData);
        }
        m_logic->trigger(AskForPeachDone, victim, dyingData);
    }

    victim->setDying(false);
    victim->broadcastProperty("dying");
    m_logic->trigger(QuitDying, victim, dyingData);
}

void GameRule::onAskForPeach(ServerPlayer *current, QVariant &data) const
{
    DeathStruct *dying = data.value<DeathStruct *>();
    while (dying->who->hp() <= 0) {
        Card *peach = nullptr;
        if (dying->who->isAlive()) {
            QVariantList args;
            args << "player" << dying->who->id();
            args << (1 - dying->who->hp());
            if (current != dying->who) {
                current->showPrompt("ask_for_peach", args);
                peach = current->askForCard("Peach");
            } else {
                current->showPrompt("ask_self_for_peach_or_analeptic", 1 - current->hp());
                peach = current->askForCard("Peach,Analeptic");
            }
        }
        if (peach == nullptr)
            break;

        CardUseStruct use;
        use.from = current;
        use.card = peach;
        use.to << dying->who;
        m_logic->useCard(use);
    }
}

void GameRule::onAskForPeachDone(ServerPlayer *victim, QVariant &data) const
{
    if (victim->hp() <= 0 && victim->isAlive()) {
        DeathStruct *death = data.value<DeathStruct *>();
        m_logic->killPlayer(victim, death->damage);
    }
}

#define ADD_GAMERULE(name) m_callbacks[name] = &GameRule::on##name
void GameRule::Init()
{
    ADD_GAMERULE(GameStart);
    ADD_GAMERULE(TurnStart);
    ADD_GAMERULE(PhaseProceeding);
    ADD_GAMERULE(PhaseEnd);
    ADD_GAMERULE(AfterHpReduced);
    ADD_GAMERULE(AskForPeach);
    ADD_GAMERULE(AskForPeachDone);
}
C_INITIALIZE_CLASS(GameRule)
