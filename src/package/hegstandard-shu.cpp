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
#include "cardarea.h"
#include "gamelogic.h"
#include "general.h"
#include "hegstandardpackage.h"
#include "serverplayer.h"
#include "skill.h"
#include "structs.h"

class Rende : public ProactiveSkill
{
    class Reset : public TriggerSkill
    {
    public:
        Reset() : TriggerSkill("#rende_reset")
        {
            m_events << PhaseChanging;
        }

        EventList triggerable(GameLogic *, EventType, ServerPlayer *owner, QVariant &data, ServerPlayer *) const override
        {
            if (owner->tag.contains("rende_count")) {
                PhaseChangeStruct *change = data.value<PhaseChangeStruct *>();
                if (change->to == Player::Inactive)
                    owner->tag.remove("rende_count");
            }
            return EventList();
        }
    };

public:
    Rende() : ProactiveSkill("rende")
    {
        addSubskill(new Reset);
    }

    bool isAvailable(const Player *player, const QString &pattern) const override
    {
        return player->handcardNum() > 0 && pattern.isEmpty();
    }

    bool cardFilter(const QList<const Card *> &, const Card *card, const Player *self, const QString &) const override
    {
        const CardArea *handcard = self->handcards();
        return handcard->contains(card);
    }

    bool cardFeasible(const QList<const Card *> &cards, const Player *) const override
    {
        return cards.length() > 0;
    }

    bool playerFilter(const QList<const Player *> &targets, const Player *, const Player *) const override
    {
        return targets.isEmpty();
    }

    bool playerFeasible(const QList<const Player *> &targets, const Player *) const override
    {
        return targets.length() == 1;
    }

    void effect(GameLogic *logic, ServerPlayer *from, const QList<ServerPlayer *> &to, const QList<Card *> &cards) const override
    {
        if (to.isEmpty() || cards.isEmpty())
            return;

        CardsMoveStruct move;
        move.cards = cards;
        move.to.owner = to.first();
        move.to.type = CardArea::Hand;
        logic->moveCards(move);

        int oldValue = from->tag["rende_count"].toInt();
        int newValue = oldValue + cards.length();
        from->tag["rende_count"] = newValue;
        if (oldValue < 3 && newValue >= 3 && from->isWounded()) {
            RecoverStruct recover;
            recover.from = from;
            recover.to = from;
            logic->recover(recover);
        }
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
        if (use->card && use->card->type() == Card::TrickType && use->card->subtype() != TrickCard::DelayedType && !use->card->isVirtual())
            return Event(this, player);
        return EventList();
    }

    bool effect(GameLogic *, EventType, ServerPlayer *target, QVariant &, ServerPlayer *) const override
    {
        target->drawCards(1);
        return false;
    }
};

void HegStandardPackage::addShuGenerals()
{
    General *liubei = new General("liubei", "shu", 4);
    liubei->addSkill(new Rende);
    addGeneral(liubei);

    General *huangyueying = new General("huangyueying", "shu", 3, General::Female);
    huangyueying->addSkill(new Jizhi);
    addGeneral(huangyueying);

    General *zhugeliang = new General("zhugeliang", "shu", 3);
    addGeneral(zhugeliang);

    General *guanyu = new General("guanyu", "shu", 5);
    addGeneral(guanyu);

    General *zhangfei = new General("zhangfei", "shu", 4);
    addGeneral(zhangfei);

    General *zhaoyun = new General("zhaoyun", "shu", 4);
    addGeneral(zhaoyun);

    General *huangzhong = new General("huangzhong", "shu", 4);
    addGeneral(huangzhong);

    General *weiyan = new General("weiyan", "shu", 4);
    addGeneral(weiyan);

    General *pangtong = new General("pangtong", "shu", 3);
    addGeneral(pangtong);

    General *wolong = new General("wolong", "shu", 3);
    addGeneral(wolong);

    General *liushan = new General("liushan", "shu", 3);
    addGeneral(liushan);

    General *menghuo = new General("menghuo", "shu", 4);
    addGeneral(menghuo);

    General *zhurong = new General("zhurong", "shu", 4, General::Female);
    addGeneral(zhurong);

    General *ganfuren = new General("ganfuren", "shu", 3, General::Female);
    addGeneral(ganfuren);
}
