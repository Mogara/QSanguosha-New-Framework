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

bool Analeptic::isAvailable(const Player *player) const
{
    return player->phase() == Player::Play && player->cardHistory("analeptic") < 1 && BasicCard::isAvailable(player);
}

Fan::Fan(Card::Suit suit, int number)
    : Weapon(suit, number)
{
    setObjectName("fan");
    m_attackRange = 4;
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

ManeuveringPackage::ManeuveringPackage()
    : Package("maneuvering")
{
}

ADD_PACKAGE(Maneuvering)
