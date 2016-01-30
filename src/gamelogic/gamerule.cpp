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
#include "skill.h"

namespace{

void onTurnStart(GameLogic *, ServerPlayer *current, QVariant &)
{
    current->setTurnCount(current->turnCount() + 1);
    if (!current->faceUp())
        current->setFaceUp(true);
    else
        current->play();
}

void onPhaseProceeding(GameLogic *logic, ServerPlayer *current, QVariant &)
{
    GameLogic::msleep(500);
    switch (current->phase()) {
    case Player::Judge: {
        QList<Card *> tricks = current->delayedTrickArea()->cards();
        while (tricks.length() > 0 && current->isAlive()) {
            Card *trick = tricks.takeLast();

            CardUseStruct use;
            use.card = trick;
            use.to << current;
            CardEffectStruct effect(use);
            effect.to = current;
            logic->takeCardEffect(effect);
            trick->complete(logic);
        }
        break;
    }
    case Player::Draw: {
        int num = 2;
        QVariant data(num);
        logic->trigger(DrawNCards, current, data);
        num = data.toInt();
        if (num > 0)
            current->drawCards(num);
        data = num;
        logic->trigger(AfterDrawNCards, current, data);
        break;
    }
    case Player::Play: {
        while (current->isAlive()) {
            if (current->activate())
                break;
        }
        break;
    }
    case Player::Discard: {
        int maxCardNum = current->hp();
        QVariant data(maxCardNum);
        logic->trigger(CountMaxCardNum, current, data);
        maxCardNum = data.toInt();
        int discardNum = current->handcardNum() - maxCardNum;
        if (discardNum > 0) {
            current->showPrompt("ask_to_discard", discardNum);
            QList<Card *> cards = current->askForCards(".|.|.|hand", discardNum);

            CardsMoveStruct move;
            move.cards = cards;
            move.to.type = CardArea::DiscardPile;
            move.isOpen = true;
            logic->moveCards(move);
        }
        break;
    }
    default:;
    }
}

void onPhaseEnd(GameLogic *, ServerPlayer *current, QVariant &)
{
    switch (current->phase()) {
    case Player::Play: {
        current->clearCardHistory();
        current->clearSkillHistory();
    }
    default:;
    }
    if (current->isDrunk()) {
        current->setDrunk(false);
        current->broadcastProperty("drunk");
    }

}

void onAfterHpReduced(GameLogic *logic, ServerPlayer *victim, QVariant &data)
{
    if (victim->hp() > 0)
        return;

    victim->setDying(true);
    victim->broadcastProperty("dying");

    DeathStruct death;
    death.who = victim;
    death.damage = data.value<DamageStruct *>();

    QVariant dyingData = QVariant::fromValue(&death);

    QList<ServerPlayer *> allPlayers = logic->allPlayers();
    foreach (ServerPlayer *player, allPlayers) {
        if (logic->trigger(EnterDying, player, dyingData) || victim->hp() > 0 || victim->isDead())
            break;
    }

    if (victim->isAlive() && victim->hp() <= 0) {
        QList<ServerPlayer *> allPlayers = logic->allPlayers();
        foreach (ServerPlayer *saver, allPlayers) {
            if (victim->hp() > 0 || victim->isDead())
                break;

            logic->trigger(AskForPeach, saver, dyingData);
        }
        logic->trigger(AskForPeachDone, victim, dyingData);
    }

    victim->setDying(false);
    victim->broadcastProperty("dying");
    logic->trigger(QuitDying, victim, dyingData);
}

void onAskForPeach(GameLogic *logic, ServerPlayer *current, QVariant &data)
{
    DeathStruct *dying = data.value<DeathStruct *>();
    while (dying->who->hp() <= 0) {
        Card *peach = nullptr;
        if (dying->who->isAlive()) {
            int peachNum = 1 - dying->who->hp();
            if (current != dying->who) {
                QVariantList args;
                args << "player" << dying->who->id();
                args << peachNum;
                current->showPrompt("ask_for_peach", args);
                peach = current->askForCard("Peach");
            } else {
                current->showPrompt("ask_self_for_peach_or_analeptic", peachNum);
                peach = current->askForCard("Peach,Analeptic");
            }
        }
        if (peach == nullptr)
            break;

        CardUseStruct use;
        use.from = current;
        use.card = peach;
        use.to << dying->who;
        logic->useCard(use);
    }
}

void onAskForPeachDone(GameLogic *logic, ServerPlayer *victim, QVariant &data)
{
    if (victim->hp() <= 0 && victim->isAlive()) {
        DeathStruct *death = data.value<DeathStruct *>();
        logic->killPlayer(victim, death->damage);
    }
}

} //namespace

GameRule::GameRule()
{
    m_events << TurnStart << PhaseProceeding << PhaseEnd;
    m_events << AfterHpReduced << AskForPeach << AskForPeachDone;
    m_compulsory = true;

    m_callbacks[TurnStart] = onTurnStart;
    m_callbacks[PhaseProceeding] = onPhaseProceeding;
    m_callbacks[PhaseEnd] = onPhaseEnd;
    m_callbacks[AfterHpReduced] = onAfterHpReduced;
    m_callbacks[AskForPeach] = onAskForPeach;
    m_callbacks[AskForPeachDone] = onAskForPeachDone;
}

bool GameRule::triggerable(ServerPlayer *owner) const
{
    return owner != nullptr;
}

bool GameRule::effect(GameLogic *logic, EventType event, ServerPlayer *current, QVariant &data, ServerPlayer *) const
{
    if (logic->skipGameRule())
        return false;

    Callback func = m_callbacks[event];
    if (func)
        (*func)(logic, current, data);
    else
        throw GameFinish;

    return false;
}
