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
    , drank(0)
    , jinkNum(1)
    , nullified(false)
{
}


Slash::Slash(Card::Suit suit, int number)
    : BasicCard(suit, number)
    , m_nature(DamageStruct::Normal)
    , m_drank(0)
{
    setObjectName("slash");
}

void Slash::onEffect(GameLogic *logic, CardEffectStruct &cardEffect)
{
    if (cardEffect.from->drank() > 0) {
        m_drank = cardEffect.from->drank();
        cardEffect.from->setDrank(0);
        cardEffect.from->broadcastProperty("drank");
    }

    SlashEffectStruct effect;
    effect.from = cardEffect.from;
    effect.nature = m_nature;
    effect.slash = this;

    effect.to = cardEffect.to;
    effect.drank = m_drank;
    effect.nullified = cardEffect.nullified;

    QVariant data = QVariant::fromValue(&effect);

    if (!logic->trigger(SlashEffect, effect.from, data)) {
        if (!logic->trigger(SlashEffected, effect.to, data)) {
            if (effect.jinkNum > 0) {
                if (!logic->trigger(SlashProceed, effect.from, data)) {
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
                }
            }
        }
    }

    if (effect.jink.length() < effect.jinkNum) {
        if (effect.to->isAlive()) {
            logic->trigger(SlashHit, effect.from, data);

            DamageStruct damage;
            damage.from = effect.from;
            damage.to = effect.to;
            damage.nature = effect.nature;
            damage.card = this;
            logic->damage(damage);
        }
    } else {
        logic->trigger(SlashMissed, effect.from, data);
    }
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

void StandardPackage::addBasicCards()
{
    for (int i = 1; i <= 100; i++) {
        if ((i & 1) == 1)
            addCard(new Slash(Card::Club, i % 13 + 1));
        else
            addCard(new Jink(Card::Diamond, i % 13 + 1));
    }
}
