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
#include "standardpackage.h"
#include "general.h"
#include "skill.h"
#include "standard-basiccard.h"

class Qingguo : public OneCardViewAsSkill
{
public:
    Qingguo() : OneCardViewAsSkill("qingguo")
    {
    }

    bool viewFilter(const Card *card, const Player *) const override
    {
        return card->color() == Card::Black;
    }

    Card *viewAs(Card *subcard, const Player *) const override
    {
        Jink *jink = new Jink(subcard->suit(), subcard->number());
        jink->setSkillName(name());
        jink->addSubcard(subcard);
        return jink;
    }

    bool isAvailable(const Player *, const QString &pattern) const override
    {
        return pattern == "Jink";
    }
};

void StandardPackage::addWeiGenerals()
{
    // WEI 001
    General *caocao = new General("caocao", "wei", 4);
    caocao->setLord(true);
    addGeneral(caocao);

    // WEI 002
    General *simayi = new General("simayi", "wei", 3);
    addGeneral(simayi);

    // WEI 003
    General *xiahoudun = new General("xiahoudun", "wei", 4);
    addGeneral(xiahoudun);

    // WEI 004
    General *zhangliao = new General("zhangliao", "wei", 4);
    addGeneral(zhangliao);

    // WEI 005
    General *xuchu = new General("xuchu", "wei", 4);
    addGeneral(xuchu);

    // WEI 006
    General *guojia = new General("guojia", "wei", 3);
    addGeneral(guojia);

    // WEI 007
    General *zhenji = new General("zhenji", "wei", 3, General::Female);
    zhenji->addSkill(new Qingguo);
    addGeneral(zhenji);
}
