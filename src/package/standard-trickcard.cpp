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

#include <croom.h>
#include <cserveragent.h>

#include "gamelogic.h"
#include "protocol.h"
#include "serverplayer.h"
#include "standardpackage.h"
#include "standard-trickcard.h"
#include "eventtype.h"

AmazingGrace::AmazingGrace(Card::Suit suit, int number)
    : GlobalEffect(suit, number)
{
    setObjectName("amazing_grace");
}

void AmazingGrace::onUse(GameLogic *logic, CardUseStruct &use)
{
    GlobalEffect::onUse(logic, use);

    CardsMoveStruct move;
    move.from.type = CardArea::DrawPile;
    move.from.direction = CardArea::Top;
    move.to.type = CardArea::Wugu;
    move.isOpen = true;

    int n = logic->allPlayers().length();
    move.cards = logic->getDrawPileCards(n);

    logic->moveCards(move);
}

void AmazingGrace::use(GameLogic *logic, CardUseStruct &use)
{
    CRoom *room = logic->room();
    room->broadcastNotification(S_COMMAND_SHOW_AMAZING_GRACE);

    try {
        GlobalEffect::use(logic, use);
        clearRestCards(logic);
    } catch (EventType e) {
        if (e == TurnBroken || e == StageChange)
            clearRestCards(logic);
        throw e;
    }
}

void AmazingGrace::onEffect(GameLogic *logic, CardEffectStruct &effect)
{
    CServerAgent *agent = effect.to->agent();
    agent->request(S_COMMAND_TAKE_AMAZING_GRACE, QVariant(), 15000);
    uint cardId = agent->waitForReply(15000).toUInt();

    Card *takenCard = nullptr;
    const CardArea *wugu = logic->wugu();
    QList<Card *> cards = wugu->cards();
    foreach (Card *card, cards) {
        if (card->id() == cardId) {
            takenCard = card;
            break;
        }
    }
    if (takenCard == nullptr)
        takenCard = cards.first();

    CardsMoveStruct move;
    move.from.type = CardArea::Wugu;
    move.cards << takenCard;
    move.to.type = CardArea::Hand;
    move.to.owner = effect.to;
    move.isOpen = true;
    logic->moveCards(move);
}

void AmazingGrace::clearRestCards(GameLogic *logic) const
{
    CRoom *room = logic->room();
    room->broadcastNotification(S_COMMAND_CLEAR_AMAZING_GRACE);

    const CardArea *wugu = logic->wugu();
    if (wugu->length() <= 0)
        return;

    CardsMoveStruct move;
    move.cards = wugu->cards();
    move.from.type = CardArea::Wugu;
    move.to.type = CardArea::DiscardPile;
    move.isOpen = true;
    logic->moveCards(move);
}

GodSalvation::GodSalvation(Card::Suit suit, int number)
    : GlobalEffect(suit, number)
{
    setObjectName("god_salvation");
}

bool GodSalvation::isNullifiable(const CardEffectStruct &effect) const
{
    return effect.to->isWounded() && TrickCard::isNullifiable(effect);
}

void GodSalvation::onEffect(GameLogic *logic, CardEffectStruct &effect)
{
    if (effect.to->isWounded()) {
        RecoverStruct recover;
        recover.card = this;
        recover.from = effect.from;
        recover.to = effect.to;
        logic->recover(recover);
    }
}

SavageAssault::SavageAssault(Card::Suit suit, int number)
    : AreaOfEffect(suit, number)
{
    setObjectName("savage_assault");
}

void SavageAssault::onEffect(GameLogic *logic, CardEffectStruct &effect)
{
    Card *slash = effect.to->askForCard("Slash", "savage-assault-slash");
    if (slash) {
        CardResponseStruct response;
        response.from = effect.from;
        response.to = effect.to;
        response.card = slash;
        response.target = this;
        logic->respondCard(response);
    } else {
        DamageStruct damage;
        damage.card = this;
        damage.from = effect.from->isAlive() ? effect.from : nullptr;
        damage.to = effect.to;
        logic->damage(damage);
    }
}

ArcheryAttack::ArcheryAttack(Card::Suit suit, int number)
    : AreaOfEffect(suit, number)
{
    setObjectName("archery_attack");
}

void ArcheryAttack::onEffect(GameLogic *logic, CardEffectStruct &effect)
{
    Card *jink = effect.to->askForCard("Jink", "archery-attack-jink");
    if (jink) {
        CardResponseStruct response;
        response.from = effect.from;
        response.to = effect.to;
        response.card = jink;
        response.target = this;
        logic->respondCard(response);
    } else {
        DamageStruct damage;
        damage.card = this;
        damage.from = effect.from->isAlive() ? effect.from : nullptr;
        damage.to = effect.to;
        logic->damage(damage);
    }
}

ExNihilo::ExNihilo(Card::Suit suit, int number)
    : SingleTargetTrick(suit, number)
{
    setObjectName("ex_nihilo");
    m_targetFixed = true;
}

void ExNihilo::onUse(GameLogic *logic, CardUseStruct &use)
{
    if (use.to.isEmpty())
        use.to << use.from;
    SingleTargetTrick::onUse(logic, use);
}

void ExNihilo::onEffect(GameLogic *, CardEffectStruct &effect)
{
    effect.to->drawCards(2);
}

Duel::Duel(Card::Suit suit, int number)
    : SingleTargetTrick(suit, number)
{
    setObjectName("duel");
}

bool Duel::targetFilter(const QList<const Player *> &targets, const Player *toSelect, const Player *self) const
{
    return targets.isEmpty() && toSelect != self;
}

void Duel::onEffect(GameLogic *logic, CardEffectStruct &effect)
{
    ServerPlayer *first = effect.to;
    ServerPlayer *second = effect.from;

    forever {
        if (!first->isAlive())
            break;
        Card *slash = first->askForCard("Slash", "duel-slash");
        if (slash == nullptr)
            break;
        CardResponseStruct response;
        response.card = slash;
        response.target = this;
        response.from = first;
        response.to = second;
        logic->respondCard(response);
        qSwap(first, second);
    }

    DamageStruct damage;
    damage.card = this;
    damage.from = second->isAlive() ? second : nullptr;
    damage.to = first;
    if (second != effect.from)
        damage.byUser = false;
    logic->damage(damage);
}

void StandardPackage::addTrickCards()
{
}
