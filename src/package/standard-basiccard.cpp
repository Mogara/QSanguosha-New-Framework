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
    , jink(nullptr)
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
                    //@to-do: ask for jink here
                }
            }
        }
    }

    if (effect.jink == nullptr) {
        if (effect.to->isAlive())
            logic->trigger(SlashHit, effect.from, data);
    } else {
        logic->trigger(SlashMissed, effect.from, data);
    }
}

void StandardPackage::addBasicCards()
{
    for (int i = 1; i <= 100; i++)
        addCard(new Slash(Card::Spade, i % 13 + 1));
}
