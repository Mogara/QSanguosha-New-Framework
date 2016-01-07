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
#include "gamemode.h"
#include "general.h"
#include "hegstandardpackage.h"
#include "standard-basiccard.h"
#include "standard-equipcard.h"
#include "standard-trickcard.h"
#include "maneuveringpackage.h"

HegStandardPackage::HegStandardPackage()
    : Package("hegstandard")
{
    QList<Card *> basicCards;

    //Basic Cards
    basicCards
        << new Slash(Card::Spade, 5)
        << new Slash(Card::Spade, 7)
        << new Slash(Card::Spade, 8)
        << new Slash(Card::Spade, 8)
        << new Slash(Card::Spade, 9)
        << new Slash(Card::Spade, 10)
        << new Slash(Card::Spade, 11)

        << new Slash(Card::Club, 2)
        << new Slash(Card::Club, 3)
        << new Slash(Card::Club, 4)
        << new Slash(Card::Club, 5)
        << new Slash(Card::Club, 8)
        << new Slash(Card::Club, 9)
        << new Slash(Card::Club, 10)
        << new Slash(Card::Club, 11)
        << new Slash(Card::Club, 11)

        << new Slash(Card::Heart, 10)
        << new Slash(Card::Heart, 12)

        << new Slash(Card::Diamond, 10)
        << new Slash(Card::Diamond, 11)
        << new Slash(Card::Diamond, 12)

        << new FireSlash(Card::Heart, 4)

        << new FireSlash(Card::Diamond, 4)
        << new FireSlash(Card::Diamond, 5)

        << new ThunderSlash(Card::Spade, 6)
        << new ThunderSlash(Card::Spade, 7)

        << new ThunderSlash(Card::Club, 6)
        << new ThunderSlash(Card::Club, 7)
        << new ThunderSlash(Card::Club, 8)

        << new Jink(Card::Heart, 2)
        << new Jink(Card::Heart, 11)
        << new Jink(Card::Heart, 13)

        << new Jink(Card::Diamond, 2)
        << new Jink(Card::Diamond, 3)
        << new Jink(Card::Diamond, 6)
        << new Jink(Card::Diamond, 7)
        << new Jink(Card::Diamond, 7)
        << new Jink(Card::Diamond, 8)
        << new Jink(Card::Diamond, 8)
        << new Jink(Card::Diamond, 9)
        << new Jink(Card::Diamond, 10)
        << new Jink(Card::Diamond, 11)
        << new Jink(Card::Diamond, 13)

        << new Peach(Card::Heart, 4)
        << new Peach(Card::Heart, 6)
        << new Peach(Card::Heart, 7)
        << new Peach(Card::Heart, 8)
        << new Peach(Card::Heart, 9)
        << new Peach(Card::Heart, 10)
        << new Peach(Card::Heart, 12)

        << new Peach(Card::Diamond, 2)
        << new Analeptic(Card::Spade, 9)
        << new Analeptic(Card::Club, 9)
        << new Analeptic(Card::Diamond, 9);
    addCards(basicCards);

    //Equip Cards
    QList<Card *> equipCards;
    equipCards
        << new Crossbow(Card::Diamond, 1)
        << new DoubleSword
        << new QinggangSword
        << new IceSword
        << new Spear
        << new Fan
        << new Axe
        << new KylinBow
        << new SixSwords
        << new Triblade

        << new EightDiagram(Card::Spade, 2)
        << new RenwangShield
        << new Vine
        << new SilverLion;
    addCards(equipCards);

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

    addQunGenerals();
    addShuGenerals();
    addWeiGenerals();
    addWuGenerals();
}

bool HegStandardPackage::isAvailable(const GameMode *mode) const
{
    return mode->name == "hegemony";
}

ADD_PACKAGE(HegStandard)
