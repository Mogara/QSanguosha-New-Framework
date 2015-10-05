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

#include "standardpackage.h"
#include "standard-equipcard.h"
#include "card.h"

Crossbow::Crossbow(Card::Suit suit, int number)
    : Weapon(suit, number)
{
    setObjectName("crossbow");
    m_attackRange = 1;
}

DoubleSword::DoubleSword(Card::Suit suit, int number)
    : Weapon(suit, number)
{
    setObjectName("double_sword");
    m_attackRange = 2;
}

QinggangSword::QinggangSword(Card::Suit suit, int number)
    : Weapon(suit, number)
{
    setObjectName("qinggang_sword");
    m_attackRange = 2;
}

IceSword::IceSword(Card::Suit suit, int number)
    : Weapon(suit, number)
{
    setObjectName("ice_sword");
    m_attackRange = 2;
}

SixSwords::SixSwords(Card::Suit suit, int number)
    : Weapon(suit, number)
{
    setObjectName("six_swords");
    m_attackRange = 2;
}

Spear::Spear(Card::Suit suit, int number)
    : Weapon(suit, number)
{
    setObjectName("spear");
    m_attackRange = 3;
}

Axe::Axe(Card::Suit suit, int number)
    : Weapon(suit, number)
{
    setObjectName("axe");
    m_attackRange = 3;
}

Triblade::Triblade(Card::Suit suit, int number)
    : Weapon(suit, number)
{
    setObjectName("triblade");
    m_attackRange = 3;
}

Fan::Fan(Card::Suit suit, int number)
    : Weapon(suit, number)
{
    setObjectName("fan");
    m_attackRange = 4;
}

KylinBow::KylinBow(Card::Suit suit, int number)
    : Weapon(suit, number)
{
    setObjectName("kylin_bow");
    m_attackRange = 5;
}

EightDiagram::EightDiagram(Card::Suit suit, int number)
    : Armor(suit, number)
{
    setObjectName("eight_diagram");
}

RenwangShield::RenwangShield(Card::Suit suit, int number)
    : Armor(suit, number)
{
    setObjectName("renwang_shield");
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

void StandardPackage::addEquipCards()
{
    QList<Card *> cards;
    cards
        << new Crossbow
        << new DoubleSword
        << new QinggangSword
        << new IceSword
        << new Spear
        << new Fan
        << new Axe
        << new KylinBow
        << new SixSwords
        << new Triblade

        << new EightDiagram
        << new RenwangShield
        << new Vine
        << new SilverLion;
    addCards(cards);

    Card *jueying = new DefensiveHorse(Card::Spade, 5);
    jueying->setObjectName("jueying");
    addCard(jueying);

    Card *dilu = new DefensiveHorse(Card::Club, 5);
    dilu->setObjectName("dilu");
    addCard(dilu);

    Card *zhuahuangfeidian = new DefensiveHorse(Card::Heart, 13);
    zhuahuangfeidian->setObjectName("zhuahuangfeidian");
    addCard(zhuahuangfeidian);

    Card *chitu = new OffensiveHorse(Card::Heart, 5);
    chitu->setObjectName("chitu");
    addCard(chitu);

    Card *dayuan = new OffensiveHorse(Card::Spade, 13);
    dayuan->setObjectName("dayuan");
    addCard(dayuan);

    Card *zixing = new OffensiveHorse(Card::Diamond, 13);
    zixing->setObjectName("zixing");
    addCard(zixing);
}
