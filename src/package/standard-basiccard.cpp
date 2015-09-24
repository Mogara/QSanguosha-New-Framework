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
#include "serverplayer.h"
#include "standardpackage.h"
#include "standard-basiccard.h"


SlashEffectStruct::SlashEffectStruct()
    : from(nullptr)
    , to(nullptr)
    , slash(nullptr)
    , nature(DamageStruct::Normal)
    , drank(false)
    , jinkNum(1)
    , nullified(false)
{
}


Slash::Slash(Card::Suit suit, int number)
    : BasicCard(suit, number)
    , m_nature(DamageStruct::Normal)
{
    setObjectName("slash");
}

void Slash::onEffect(GameLogic *logic, CardEffectStruct &cardEffect)
{
    SlashEffectStruct effect;
    effect.from = cardEffect.from;
    effect.nature = m_nature;
    effect.slash = this;

    effect.to = cardEffect.to;
    effect.drank = cardEffect.from->isDrank();
    if (effect.drank) {
        cardEffect.from->setDrank(false);
        cardEffect.from->broadcastProperty("isDrank");
    }
    effect.nullified = cardEffect.nullified;

    QVariant data = QVariant::fromValue(&effect);

    do {
        if (logic->trigger(SlashEffect, effect.from, data))
            break;

        if (logic->trigger(SlashEffected, effect.to, data))
            break;

        if (effect.jinkNum <= 0)
            break;

        if (logic->trigger(SlashProceed, effect.from, data))
            break;

        while (effect.jink.length() < effect.jinkNum) {
            Card *card = effect.to->askForCard("Jink", "slash-jink");
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
            if (effect.drank)
                damage.damage++;
            logic->damage(damage);
        }
    } else {
        logic->trigger(SlashMissed, effect.from, data);
    }
}

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

void Jink::onEffect(GameLogic *, CardEffectStruct &effect)
{
    SlashEffectStruct *slashEffect = effect.extra.value<SlashEffectStruct *>();
    if (slashEffect)
        slashEffect->jink << this;
}

Peach::Peach(Card::Suit suit, int number)
    : BasicCard(suit, number)
{
    setObjectName("peach");
}

void Peach::onUse(GameLogic *logic, CardUseStruct &use)
{
    if (use.to.isEmpty())
        use.to << use.from;
    BasicCard::onUse(logic, use);
}

void Peach::onEffect(GameLogic *logic, CardEffectStruct &effect)
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

Analeptic::Analeptic(Card::Suit suit, int number)
    : BasicCard(suit, number)
{
    setObjectName("analeptic");
}

void Analeptic::onUse(GameLogic *logic, CardUseStruct &use)
{
    if (use.to.isEmpty())
        use.to << use.from;
    BasicCard::onUse(logic, use);
}

void Analeptic::onEffect(GameLogic *logic, CardEffectStruct &effect)
{
    if (effect.to->phase() == Player::Play) {
        effect.to->setDrank(true);
        effect.to->broadcastProperty("isDrank");
    } else {
        RecoverStruct recover;
        recover.card = this;
        recover.from = effect.from;
        recover.to = effect.to;
        logic->recover(recover);
    }
}

void StandardPackage::addBasicCards()
{
    for (int i = 1; i <= 100; i++) {
        switch (i % 3){
        case 0: addCard(new Slash(Card::Club, i % 13 + 1));
        case 1: addCard(new Jink(Card::Diamond, i % 13 + 1));
        case 2: addCard(new Peach(Card::Heart, i % 13 + 1));
        }
    }
}
