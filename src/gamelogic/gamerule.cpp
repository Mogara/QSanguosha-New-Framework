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

using namespace DataValue;

namespace{

void onTurnStart(GameLogic *, ServerPlayer *current, QObject *)
{
    current->setTurnCount(current->turnCount() + 1);
    if (!current->faceUp())
        current->setFaceUp(true);
    else
        current->play();
}

void onPhaseProceeding(GameLogic *logic, ServerPlayer *current, QObject *)
{
    GameLogic::msleep(500);
    switch (current->phase()) {
    case Player::Judge: {
        QList<Card *> tricks = current->delayedTrickArea()->cards();
        while (tricks.length() > 0 && current->isAlive()) {
            Card *trick = tricks.takeLast();

            if (trick->type() == Card::TrickType && trick->subtype() == TrickCard::DelayedType) {
                CardUseValue use;
                use.card = trick;
                use.to << current;
                CardEffectValue effect(use);
                effect.to = current;
                logic->takeCardEffect(effect);
                trick->complete(logic);
            }
        }
        break;
    }
    case Player::Draw: {
        IntValue value(2);
        logic->trigger(EventHandler::DrawNCards, current, &value);
        int num = value.value;
        if (num > 0)
            current->drawCards(num);
        IntValue valueNew(num);
        logic->trigger(EventHandler::AfterDrawNCards, current, &valueNew);
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
        IntValue value(maxCardNum);
        logic->trigger(EventHandler::CountMaxCardNum, current, &value);
        maxCardNum = value.value;
        int discardNum = current->handcardNum() - maxCardNum;
        if (discardNum > 0) {
            current->showPrompt("ask_to_discard", discardNum);
            QList<Card *> cards = current->askForCards(".|.|.|hand", discardNum);

            CardsMoveValue moves;
            foreach (Card *card, cards) {
                CardMove move;
                move.card = card;
                move.toArea = logic->discardPile();
                move.isOpen = true;
                moves.moves << &move;
            }
            logic->moveCards(moves);
        }
        break;
    }
    default:;
    }
}

void onPhaseEnd(GameLogic *, ServerPlayer *current, QObject *)
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

void onAfterHpReduced(GameLogic *logic, ServerPlayer *victim, QObject *data)
{
    if (victim->hp() > 0)
        return;

    victim->setDying(true);
    victim->broadcastProperty("dying");

    DeathValue death;
    death.who = victim;
    death.damage = qobject_cast<DamageValue *>(data);

    QList<ServerPlayer *> allPlayers = logic->allPlayers();
    foreach (ServerPlayer *player, allPlayers) {
        if (logic->trigger(EventHandler::EnterDying, player, &death) || victim->hp() > 0 || victim->isDead())
            break;
    }

    if (victim->isAlive() && victim->hp() <= 0) {
        QList<ServerPlayer *> allPlayers = logic->allPlayers();
        foreach (ServerPlayer *saver, allPlayers) {
            if (victim->hp() > 0 || victim->isDead())
                break;

            logic->trigger(EventHandler::AskForPeach, saver, &death);
        }
        logic->trigger(EventHandler::AskForPeachDone, victim, &death);
    }

    victim->setDying(false);
    victim->broadcastProperty("dying");
    logic->trigger(EventHandler::QuitDying, victim, &death);
}

void onAskForPeach(GameLogic *logic, ServerPlayer *current, QObject *data)
{
    DeathValue *dying = qobject_cast<DeathValue *>(data);
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

        CardUseValue use;
        use.from = current;
        use.card = peach;
        use.to << dying->who;
        logic->useCard(use);
    }
}

void onAskForPeachDone(GameLogic *logic, ServerPlayer *victim, QObject *data)
{
    if (victim->hp() <= 0 && victim->isAlive()) {
        DeathValue *death = qobject_cast<DeathValue *>(data);
        logic->killPlayer(victim, death->damage);
    }
}

} //namespace

GameRule::GameRule()
{
    m_events << TurnStart << PhaseProceeding << PhaseEnd;
    m_events << AfterHpReduced << AskForPeach << AskForPeachDone;

    m_callbacks[TurnStart] = onTurnStart;
    m_callbacks[PhaseProceeding] = onPhaseProceeding;
    m_callbacks[PhaseEnd] = onPhaseEnd;
    m_callbacks[AfterHpReduced] = onAfterHpReduced;
    m_callbacks[AskForPeach] = onAskForPeach;
    m_callbacks[AskForPeachDone] = onAskForPeachDone;
}

EventList GameRule::triggerable(GameLogic *logic, EventType, const QObject *, ServerPlayer *) const
{
    return EventList() << Event(logic, this);
}

bool GameRule::effect(GameLogic *logic, EventType event, const EventPtr, QObject *data, ServerPlayer *player) const
{
    if (logic->skipGameRule())
        return false;

    Callback func = m_callbacks[event];
    if (func)
        (*func)(logic, player, data);
    else
        throw GameFinish;

    return false;
}
