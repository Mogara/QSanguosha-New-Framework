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

#include "engine.h"
#include "gamelogic.h"
#include "maneuveringpackage.h"
#include "serverplayer.h"
#include "standard-trickcard.h"

FireSlash::FireSlash(Card::Suit suit, int number)
    : Slash(suit, number)
{
    setObjectName("fire_slash");
    m_nature = DamageStruct::Fire;
}

ThunderSlash::ThunderSlash(Card::Suit suit, int number)
    : Slash(suit, number)
{
    setObjectName("thunder_slash");
    m_nature = DamageStruct::Thunder;
}

Analeptic::Analeptic(Card::Suit suit, int number)
    : BasicCard(suit, number)
{
    setObjectName("analeptic");
    m_targetFixed = true;
}

void Analeptic::onUse(GameLogic *logic, CardUseStruct &use)
{
    if (use.to.isEmpty())
        use.to << use.from;
    BasicCard::onUse(logic, use);
}

void Analeptic::effect(GameLogic *logic, CardEffectStruct &effect)
{
    if (effect.to->phase() == Player::Play) {
        effect.to->setDrunk(true);
        effect.to->broadcastProperty("drunk");
    } else {
        RecoverStruct recover;
        recover.card = this;
        recover.from = effect.from;
        recover.to = effect.to;
        logic->recover(recover);
    }
}

Fan::Fan(Card::Suit suit, int number)
    : Weapon(suit, number)
{
    setObjectName("fan");
    m_attackRange = 4;
}

GudingBlade::GudingBlade(Card::Suit suit, int number)
    : Weapon(suit, number)
{
    setObjectName("guding_blade");
    m_attackRange = 2;
}

Vine::Vine(Card::Suit suit, int number)
    : Armor(suit, number)
{
    setObjectName("vine");
}

SilverLion::SilverLion(Card::Suit suit, int number)
    : Armor(suit, number)
{
    setObjectName("silver_lion");
}

SupplyShortage::SupplyShortage(Card::Suit suit, int number)
    : DelayedTrick(suit, number)
{
    setObjectName("supply_shortage");
    m_judgePattern = ".|^club";
    m_distanceLimit = 1;
}

void SupplyShortage::takeEffect(GameLogic *, CardEffectStruct &effect)
{
    effect.to->clearCardHistory();
    effect.to->skipPhase(Player::Draw);
}

IronChain::IronChain(Card::Suit suit, int number)
    : TrickCard(suit, number)
{
    setObjectName("iron_chain");
    m_subtype = DamageSpreadType;
    m_canRecast = true;
    m_maxTargetNum = 2;
}

void IronChain::effect(GameLogic *, CardEffectStruct &effect)
{
    effect.to->setChained(!effect.to->isChained());
    effect.to->broadcastProperty("chained");
}

FireAttack::FireAttack(Card::Suit suit, int number)
    : SingleTargetTrick(suit, number)
{
    setObjectName("fire_attack");
}

bool FireAttack::targetFilter(const QList<const Player *> &targets, const Player *toSelect, const Player *self) const
{
    return toSelect->handcardNum() > 0 && SingleTargetTrick::targetFilter(targets, toSelect, self);
}

void FireAttack::effect(GameLogic *logic, CardEffectStruct &effect)
{
    if (effect.to->handcardNum() <= 0)
        return;

    effect.to->showPrompt("fire-attack-show-card", effect.from);
    Card *card = effect.to->askForCard(".", false);
    if (card)
        effect.to->showCard(card);
    else
        return;

    if (effect.from->isAlive()) {
        QString pattern = QString(".|") + card->suitString();
        effect.from->showPrompt("fire-attack-discard-card", effect.to, card);
        Card *discarded = effect.from->askForCard(pattern);
        if (discarded) {
            CardsMoveStruct move;
            move.cards << discarded;
            move.to.type = CardArea::DiscardPile;
            move.isOpen = true;
            logic->moveCards(move);

            DamageStruct damage;
            damage.card = this;
            damage.from = effect.from;
            damage.to = effect.to;
            damage.nature = DamageStruct::Fire;
            logic->damage(damage);
        }
    }
}

ManeuveringPackage::ManeuveringPackage()
    : Package("maneuvering")
{
    QList<Card *> cards;

    // spade
    cards
        //<< new GudingBlade(Card::Spade, 1)
        << new Vine(Card::Spade, 2)
        << new Analeptic(Card::Spade, 3)
        << new ThunderSlash(Card::Spade, 4)
        << new ThunderSlash(Card::Spade, 5)
        << new ThunderSlash(Card::Spade, 6)
        << new ThunderSlash(Card::Spade, 7)
        << new ThunderSlash(Card::Spade, 8)
        << new Analeptic(Card::Spade, 9)
        << new SupplyShortage(Card::Spade, 10)
        << new IronChain(Card::Spade, 11)
        << new IronChain(Card::Spade, 12)
        << new Nullification(Card::Spade, 13);

    // club
    cards << new SilverLion(Card::Club, 1)
        << new Vine(Card::Club, 2)
        << new Analeptic(Card::Club, 3)
        << new SupplyShortage(Card::Club, 4)
        << new ThunderSlash(Card::Club, 5)
        << new ThunderSlash(Card::Club, 6)
        << new ThunderSlash(Card::Club, 7)
        << new ThunderSlash(Card::Club, 8)
        << new Analeptic(Card::Club, 9)
        << new IronChain(Card::Club, 10)
        << new IronChain(Card::Club, 11)
        << new IronChain(Card::Club, 12)
        << new IronChain(Card::Club, 13);

    // heart
    cards << new Nullification(Card::Heart, 1)
        << new FireAttack(Card::Heart, 2)
        << new FireAttack(Card::Heart, 3)
        << new FireSlash(Card::Heart, 4)
        << new Peach(Card::Heart, 5)
        << new Peach(Card::Heart, 6)
        << new FireSlash(Card::Heart, 7)
        << new Jink(Card::Heart, 8)
        << new Jink(Card::Heart, 9)
        << new FireSlash(Card::Heart, 10)
        << new Jink(Card::Heart, 11)
        << new Jink(Card::Heart, 12)
        << new Nullification(Card::Heart, 13);

    // diamond
    cards << new Fan(Card::Diamond, 1)
        << new Peach(Card::Diamond, 2)
        << new Peach(Card::Diamond, 3)
        << new FireSlash(Card::Diamond, 4)
        << new FireSlash(Card::Diamond, 5)
        << new Jink(Card::Diamond, 6)
        << new Jink(Card::Diamond, 7)
        << new Jink(Card::Diamond, 8)
        << new Analeptic(Card::Diamond, 9)
        << new Jink(Card::Diamond, 10)
        << new Jink(Card::Diamond, 11)
        << new FireAttack(Card::Diamond, 12);

    addCards(cards);

    DefensiveHorse *hualiu = new DefensiveHorse(Card::Diamond, 13);
    hualiu->setObjectName("hualiu");
    addCard(hualiu);
}

ADD_PACKAGE(Maneuvering)
