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

#include "gamelogic.h"
#include "serverplayer.h"
#include "standardpackage.h"
#include "standard-basiccard.h"

SlashEffectStruct::SlashEffectStruct()
    : from(nullptr)
    , to(nullptr)
    , slash(nullptr)
    , nature(DamageStruct::Normal)
    , drunk(false)
    , jinkNum(1)
{
}

Slash::Slash(Card::Suit suit, int number)
    : BasicCard(suit, number)
    , m_nature(DamageStruct::Normal)
{
    setObjectName("slash");
}

bool Slash::targetFeasible(const QList<const Player *> &targets, const Player *) const
{
    return targets.length() == 1;
}

bool Slash::targetFilter(const QList<const Player *> &targets, const Player *toSelect, const Player *self) const
{
    return targets.isEmpty() && self->distanceTo(toSelect) <= self->attackRange();
}

void Slash::effect(GameLogic *logic, CardEffectStruct &cardEffect)
{
    SlashEffectStruct effect;
    effect.from = cardEffect.from;
    effect.nature = m_nature;
    effect.slash = this;

    effect.to = cardEffect.to;
    effect.drunk = cardEffect.from->isDrunk();
    if (effect.drunk) {
        cardEffect.from->setDrunk(false);
        cardEffect.from->broadcastProperty("drunk");
    }

    QVariant data = QVariant::fromValue(&effect);

    do {
        if (logic->trigger(SlashEffect, effect.from, data))
            return;

        if (logic->trigger(SlashEffected, effect.to, data))
            return;

        if (effect.jinkNum <= 0)
            return;

        if (logic->trigger(SlashProceed, effect.from, data))
            return;

        while (effect.jink.length() < effect.jinkNum) {
            QVariantList args;
            args << "player" << effect.from->id();
            args << effect.jinkNum;
            effect.to->showPrompt("slash-jink", args);
            Card *card = effect.to->askForCard("Jink");
            if (card) {
                CardUseStruct use;
                use.from = effect.to;
                use.card = card;
                use.extra = data;
                logic->useCard(use);
            } else {
                break;
            }
        }
    } while (false);

    if (effect.jink.length() < effect.jinkNum) {
        if (effect.to->isAlive()) {
            logic->trigger(SlashHit, effect.from, data);

            DamageStruct damage;
            damage.from = effect.from;
            damage.to = effect.to;
            damage.nature = effect.nature;
            damage.card = this;
            if (effect.drunk)
                damage.damage++;
            logic->damage(damage);
        }
    } else {
        logic->trigger(SlashMissed, effect.from, data);
    }
}

bool Slash::isAvailable(const Player *player) const
{
    return player->phase() == Player::Play && player->cardHistory("slash") < 1 && BasicCard::isAvailable(player);
}

Jink::Jink(Card::Suit suit, int number)
    : BasicCard(suit, number)
{
    setObjectName("jink");
}

void Jink::onUse(GameLogic *logic, CardUseStruct &use)
{
    SlashEffectStruct *effect = use.extra.value<SlashEffectStruct *>();
    if (effect)
        use.target = effect->slash;
    BasicCard::onUse(logic, use);
}

void Jink::effect(GameLogic *, CardEffectStruct &effect)
{
    SlashEffectStruct *slashEffect = effect.use.extra.value<SlashEffectStruct *>();
    if (slashEffect)
        slashEffect->jink << this;
}

bool Jink::isAvailable(const Player *) const
{
    return false;
}

Peach::Peach(Card::Suit suit, int number)
    : BasicCard(suit, number)
{
    setObjectName("peach");
    m_targetFixed = true;
}

void Peach::onUse(GameLogic *logic, CardUseStruct &use)
{
    if (use.to.isEmpty())
        use.to << use.from;
    BasicCard::onUse(logic, use);
}

void Peach::effect(GameLogic *logic, CardEffectStruct &effect)
{
    RecoverStruct recover;
    recover.card = this;
    recover.from = effect.from;
    recover.to = effect.to;
    logic->recover(recover);
}

bool Peach::isAvailable(const Player *player) const
{
    return player->isWounded() && BasicCard::isAvailable(player);
}

void StandardPackage::addBasicCards()
{
    QList<Card *> cards;
    cards
        << new Slash(Card::Spade, 7)
        << new Slash(Card::Spade, 8)
        << new Slash(Card::Spade, 8)
        << new Slash(Card::Spade, 9)
        << new Slash(Card::Spade, 9)
        << new Slash(Card::Spade, 10)
        << new Slash(Card::Spade, 10)

        << new Slash(Card::Club, 2)
        << new Slash(Card::Club, 3)
        << new Slash(Card::Club, 4)
        << new Slash(Card::Club, 5)
        << new Slash(Card::Club, 6)
        << new Slash(Card::Club, 7)
        << new Slash(Card::Club, 8)
        << new Slash(Card::Club, 8)
        << new Slash(Card::Club, 9)
        << new Slash(Card::Club, 9)
        << new Slash(Card::Club, 10)
        << new Slash(Card::Club, 10)
        << new Slash(Card::Club, 11)
        << new Slash(Card::Club, 11)

        << new Slash(Card::Heart, 10)
        << new Slash(Card::Heart, 10)
        << new Slash(Card::Heart, 11)

        << new Slash(Card::Diamond, 6)
        << new Slash(Card::Diamond, 7)
        << new Slash(Card::Diamond, 8)
        << new Slash(Card::Diamond, 9)
        << new Slash(Card::Diamond, 10)
        << new Slash(Card::Diamond, 13)

        << new Jink(Card::Heart, 2)
        << new Jink(Card::Heart, 2)
        << new Jink(Card::Heart, 13)

        << new Jink(Card::Diamond, 2)
        << new Jink(Card::Diamond, 2)
        << new Jink(Card::Diamond, 3)
        << new Jink(Card::Diamond, 4)
        << new Jink(Card::Diamond, 5)
        << new Jink(Card::Diamond, 6)
        << new Jink(Card::Diamond, 7)
        << new Jink(Card::Diamond, 8)
        << new Jink(Card::Diamond, 9)
        << new Jink(Card::Diamond, 10)
        << new Jink(Card::Diamond, 11)
        << new Jink(Card::Diamond, 11)

        << new Peach(Card::Heart, 3)
        << new Peach(Card::Heart, 4)
        << new Peach(Card::Heart, 6)
        << new Peach(Card::Heart, 7)
        << new Peach(Card::Heart, 8)
        << new Peach(Card::Heart, 9)
        << new Peach(Card::Heart, 12)

        << new Peach(Card::Diamond, 12);

    addCards(cards);
}
