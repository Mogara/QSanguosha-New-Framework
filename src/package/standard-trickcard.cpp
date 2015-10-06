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

void AmazingGrace::effect(GameLogic *logic, CardEffectStruct &effect)
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

void GodSalvation::effect(GameLogic *logic, CardEffectStruct &effect)
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

void SavageAssault::effect(GameLogic *logic, CardEffectStruct &effect)
{
    Card *slash = effect.to->askForCard("Slash", "savage-assault-slash");
    if (slash) {
        CardResponseStruct response;
        response.from = effect.to;
        response.to = effect.from;
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

void ArcheryAttack::effect(GameLogic *logic, CardEffectStruct &effect)
{
    Card *jink = effect.to->askForCard("Jink", "archery-attack-jink");
    if (jink) {
        CardResponseStruct response;
        response.from = effect.to;
        response.to = effect.from;
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

void ExNihilo::effect(GameLogic *, CardEffectStruct &effect)
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

void Duel::effect(GameLogic *logic, CardEffectStruct &effect)
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

Indulgence::Indulgence(Card::Suit suit, int number)
    : DelayedTrick(suit, number)
{
    setObjectName("indulgence");
    m_judgePattern = ".|^heart";
}

void Indulgence::takeEffect(GameLogic *, CardEffectStruct &effect)
{
    effect.to->clearCardHistory();
    effect.to->skipPhase(Player::Play);
}

Snatch::Snatch(Card::Suit suit, int number)
    : SingleTargetTrick(suit, number)
{
    setObjectName("snatch");
}

bool Snatch::targetFilter(const QList<const Player *> &targets, const Player *toSelect, const Player *self) const
{
    return toSelect != self && self->distanceTo(toSelect) <= 1 && !toSelect->isAllNude() && SingleTargetTrick::targetFilter(targets, toSelect, self);
}

void Snatch::effect(GameLogic *logic, CardEffectStruct &effect)
{
    if (effect.from->isDead())
        return;
    if (effect.to->isAllNude())
        return;

    Card *card = effect.from->askToChooseCard(effect.to);
    if (card) {
        CardsMoveStruct move;
        move.cards << card;
        move.to.owner = effect.from;
        move.to.type = CardArea::Hand;
        logic->moveCards(move);
    }
}

Dismantlement::Dismantlement(Card::Suit suit, int number)
    : SingleTargetTrick(suit, number)
{
    setObjectName("dismantlement");
}

bool Dismantlement::targetFilter(const QList<const Player *> &targets, const Player *toSelect, const Player *self) const
{
    return toSelect != self && !toSelect->isAllNude() && SingleTargetTrick::targetFilter(targets, toSelect, self);
}

void Dismantlement::effect(GameLogic *logic, CardEffectStruct &effect)
{
    if (effect.from->isDead())
        return;
    if (effect.to->isAllNude())
        return;

    Card *card = effect.from->askToChooseCard(effect.to);
    if (card) {
        CardsMoveStruct move;
        move.cards << card;
        move.to.type = CardArea::DiscardPile;
        move.isOpen = true;
        logic->moveCards(move);
    }
}

Collateral::Collateral(Card::Suit suit, int number)
    : SingleTargetTrick(suit, number)
    , m_victim(nullptr)
{
    setObjectName("collateral");
}

bool Collateral::isAvailable(const Player *player) const
{
    bool canUse = false;
    const Player *next = player->nextAlive();
    while (next && next != player) {
        const CardArea *equips = next->equips();
        if (equips->contains("Weapon")) {
            canUse = true;
            break;
        }
        next = next->nextAlive();
    }
    return canUse && SingleTargetTrick::isAvailable(player);
}

bool Collateral::targetFeasible(const QList<const Player *> &targets, const Player *) const
{
    return targets.length() == 2;
}

bool Collateral::targetFilter(const QList<const Player *> &targets, const Player *toSelect, const Player *self) const
{
    if (!targets.isEmpty()) {
        if (targets.length() >= 2)
            return false;
        const Player *slashSource = targets.first();
        // @to-do: Check prohibit skills like Kongcheng
        return slashSource->distanceTo(toSelect) <= slashSource->attackRange();
    } else {
        const CardArea *equips = toSelect->equips();
        return equips->contains("Weapon") && toSelect != self;
    }
}

void Collateral::onUse(GameLogic *logic, CardUseStruct &use)
{
    m_victim = use.to.at(1);
    use.to.removeAt(1);
    SingleTargetTrick::onUse(logic, use);
}

bool Collateral::doCollateral(ServerPlayer *killer, const QString &prompt) const
{
    if (killer->distanceTo(m_victim) > killer->attackRange())
        return false;
    QList<ServerPlayer *> targets;
    targets << m_victim;
    Card *slash = killer->askToUseCard("Slash", prompt, targets);
    return slash != nullptr;
}

void Collateral::effect(GameLogic *logic, CardEffectStruct &effect)
{
    Card *weapon = nullptr;
    foreach (Card *card, effect.to->equips()->cards()) {
        if (card->subtype() == EquipCard::WeaponType) {
            weapon = card;
            break;
        }
    }

    QString prompt = QString("collateral-slash:%1:%2").arg(effect.from->id()).arg(m_victim->id());
    if (m_victim->isDead()) {
        if (effect.from->isAlive() && effect.to->isAlive() && weapon) {
            CardsMoveStruct move;
            move.cards << weapon;
            move.to.type = CardArea::Hand;
            move.to.owner = effect.from;
            logic->moveCards(move);
        }
    } else if (effect.from->isDead()) {
        if (effect.to->isAlive())
            doCollateral(effect.to, prompt);
    } else {
        if (effect.to->isDead()) {
            ; // do nothing
        } else if (weapon == nullptr) {
            doCollateral(effect.to, prompt);
        } else {
            if (!doCollateral(effect.to, prompt)) {
                if (weapon) {
                    CardsMoveStruct move;
                    move.cards << weapon;
                    move.to.type = CardArea::Hand;
                    move.to.owner = effect.from;
                    logic->moveCards(move);
                }
            }
        }
    }
}

Nullification::Nullification(Card::Suit suit, int number)
    : SingleTargetTrick(suit, number)
{
    setObjectName("nullification");
}

bool Nullification::isAvailable(const Player *) const
{
    return false;
}

bool Nullification::targetFeasible(const QList<const Player *> &targets, const Player *) const
{
    return targets.isEmpty();
}

bool Nullification::targetFilter(const QList<const Player *> &, const Player *, const Player *) const
{
    return false;
}

void Nullification::onUse(GameLogic *logic, CardUseStruct &use)
{
    CardUseStruct *trickUse = use.extra.value<CardUseStruct *>();
    if (trickUse)
        use.card = trickUse->card;
    SingleTargetTrick::onUse(logic, use);
}

void Nullification::effect(GameLogic *, CardEffectStruct &effect)
{
    CardEffectStruct *trickEffect = effect.use.extra.value<CardEffectStruct *>();
    if (trickEffect) {
        if (trickEffect->to)
            trickEffect->use.nullifiedList << trickEffect->to;
        else if (trickEffect->use.card->inherits("Nullification"))
            trickEffect->use.isNullified = true;
    }
}

Lightning::Lightning(Card::Suit suit, int number)
    : MovableDelayedTrick(suit, number)
{
    setObjectName("lightning");
    m_judgePattern = ".|spade|2~9";
}

void Lightning::takeEffect(GameLogic *logic, CardEffectStruct &effect)
{
    DamageStruct damage;
    damage.to = effect.to;
    damage.card = this;
    damage.damage = 3;
    damage.nature = DamageStruct::Thunder;
    logic->damage(damage);
}

void StandardPackage::addTrickCards()
{
}
