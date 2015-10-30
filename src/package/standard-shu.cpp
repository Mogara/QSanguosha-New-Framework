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
#include "serverplayer.h"
#include "skill.h"
#include "standard-basiccard.h"
#include "structs.h"

class Wusheng : public OneCardViewAsSkill
{
public:
    Wusheng() : OneCardViewAsSkill("wusheng")
    {
    }

    bool viewFilter(const Card *card, const Player *, const QString &) const override
    {
        return card->color() == Card::Red;
    }

    Card *viewAs(Card *subcard, const Player *) const override
    {
        Slash *slash = new Slash(subcard->suit(), subcard->number());
        slash->setSkillName(name());
        slash->addSubcard(subcard);
        return slash;
    }

    bool isAvailable(const Player *self, const QString &pattern) const override
    {
        if (pattern.isEmpty())
            return CheckAvailability<Slash>(self);
        else
            return pattern == "Slash";
    }
};

class Longdan: public OneCardViewAsSkill
{
public:
    Longdan() : OneCardViewAsSkill("longdan")
    {
    }

    bool viewFilter(const Card *card, const Player *, const QString &pattern) const override
    {
        if (pattern == "Slash" || pattern.isEmpty())
            return card->inherits("Jink");
        else if (pattern == "Jink")
            return card->inherits("Slash");
        return false;
    }

    Card *viewAs(Card *subcard, const Player *) const override
    {
        Card *card = nullptr;
        if (subcard->inherits("Slash"))
            card = new Jink(subcard->suit(), subcard->number());
        else if (subcard->inherits("Jink"))
            card = new Slash(subcard->suit(), subcard->number());
        if (card) {
            card->setSkillName(name());
            card->addSubcard(subcard);
        }
        return card;
    }

    bool isAvailable(const Player *self, const QString &pattern) const override
    {
        if (pattern.isEmpty())
            return CheckAvailability<Slash>(self);
        else
            return pattern == "Slash" || pattern == "Jink";
    }
};

class Jizhi : public TriggerSkill
{
public:
    Jizhi() : TriggerSkill("jizhi")
    {
        m_events << TargetChosen;
    }

    EventList triggerable(GameLogic *, EventType, ServerPlayer *player, QVariant &data, ServerPlayer *) const override
    {
        if (!TriggerSkill::triggerable(player))
            return EventList();

        CardUseStruct *use = data.value<CardUseStruct *>();
        if (use->card && use->card->type() == Card::TrickType && use->card->subtype() != TrickCard::DelayedType)
            return Event(this, player);
        return EventList();
    }

    bool effect(GameLogic *, EventType, ServerPlayer *target, QVariant &, ServerPlayer *) const override
    {
        target->drawCards(1);
        return false;
    }
};

void StandardPackage::addShuGenerals()
{
    // SHU 001
    General *liubei = new General("liubei", "shu", 4);
    liubei->setLord(true);
    addGeneral(liubei);

    // SHU 002
    General *guanyu = new General("guanyu", "shu", 5);
    guanyu->addSkill(new Wusheng);
    addGeneral(guanyu);

    // SHU 003
    General *zhangfei = new General("zhangfei", "shu", 4);
    addGeneral(zhangfei);

    // SHU 004
    General *zhugeliang = new General("zhugeliang", "shu", 3);
    addGeneral(zhugeliang);

    // SHU 005
    General *zhaoyun = new General("zhaoyun", "shu", 4);
    zhaoyun->addSkill(new Longdan);
    addGeneral(zhaoyun);

    // SHU 006
    General *machao = new General("machao", "shu", 4);
    addGeneral(machao);

    // SHU 007
    General *huangyueying = new General("huangyueying", "shu", 3, General::Female);
    huangyueying->addSkill(new Jizhi);
    addGeneral(huangyueying);
}
