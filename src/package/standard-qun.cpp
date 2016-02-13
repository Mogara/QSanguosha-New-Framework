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
#include "general.h"
#include "serverplayer.h"
#include "skill.h"
#include "standardpackage.h"
#include "standard-basiccard.h"
#include "standard-trickcard.h"

namespace{

class Qingnang: public ProactiveSkill
{
public:
    Qingnang() : ProactiveSkill("qingnang")
    {
    }

    bool isAvailable(const Player *self, const QString &pattern) const override
    {
        return ProactiveSkill::isAvailable(self, pattern) && self->skillHistory(this) <= 0;
    }

    bool cardFilter(const QList<const Card *> &selected, const Card *card, const Player *source, const QString &) const override
    {
        const CardArea *hand = source->handcardArea();
        return selected.isEmpty() && hand->contains(card);
    }

    bool cardFeasible(const QList<const Card *> &selected, const Player *) const override
    {
        return selected.length() == 1;
    }

    bool playerFilter(const QList<const Player *> &selected, const Player *toSelect, const Player *) const override
    {
        return selected.isEmpty() && toSelect->isWounded();
    }

    bool playerFeasible(const QList<const Player *> &selected, const Player *) const override
    {
        return selected.length() == 1;
    }

    void effect(GameLogic *logic, ServerPlayer *from, const QList<ServerPlayer *> &to, const QList<Card *> &cards) const override
    {
        CardsMoveStruct move;
        move.cards = cards;
        move.to.type = CardArea::DiscardPile;
        move.isOpen = true;
        logic->moveCards(move);

        RecoverStruct recover;
        recover.from = from;
        recover.to = to.first();
        logic->recover(recover);
    }
};

class Jijiu : public OneCardViewAsSkill
{
public:
    Jijiu() : OneCardViewAsSkill("jijiu")
    {
    }

    bool isAvailable(const Player *self, const QString &pattern) const override
    {
        return self->phase() == Player::Inactive && pattern == "Peach";
    }

    bool viewFilter(const Card *card, const Player *, const QString &pattern) const override
    {
        return card->color() == Card::Red && pattern == "Peach";
    }

    Card *viewAs(Card *card, const Player *) const override
    {
        Peach *peach = new Peach(card->suit(), card->number());
        peach->addSubcard(card);
        peach->setSkill(this);
        return peach;
    }
};

class Wushuang : public TriggerSkill
{
public:
    Wushuang() : TriggerSkill("wushuang")
    {
        m_events << SlashEffect << CardResponded;
        setFrequency(Compulsory);
    }

//    EventMap triggerable(GameLogic *, EventType event, ServerPlayer *, QVariant &data) const override
//    {
//        EventMap events;
//        if (event == SlashEffect) {
//            SlashEffectStruct *effect = data.value<SlashEffectStruct *>();
//            if (effect->from && effect->from->hasSkill(this)) {
//                Event e(this, effect->from);
//                e.to << effect->to;
//                events.insert(effect->from, e);
//            }
//        } else if (event == CardResponded) {
//            CardResponseStruct *response = data.value<CardResponseStruct *>();
//            if (response->target && response->target->inherits("Duel")) {
//                if (response->to && response->to->hasSkill(this)) {
//                    if (response->from->tag["wushuang_slash"].toBool()) {
//                        response->from->tag.remove("wushuang_slash");
//                    } else {
//                        Event e(this, response->to);
//                        e.to << response->from;
//                        events.insert(response->to, e);
//                    }
//                }
//            }
//        }

//        return events;
//    }

//    bool effect(GameLogic *logic, EventType event, ServerPlayer *, QVariant &data, ServerPlayer *) const override
//    {
//        if (event == SlashEffect) {
//            SlashEffectStruct *effect = data.value<SlashEffectStruct *>();
//            effect->jinkNum = 2;
//        } else if (event == CardResponded) {
//            CardResponseStruct *response = data.value<CardResponseStruct *>();

//            response->from->showPrompt("duel-slash", response->to);
//            Card *slash = response->from->askForCard("Slash");
//            if (slash == nullptr)
//                return true;

//            response->from->tag["wushuang_slash"] = true;
//            CardResponseStruct extra;
//            extra.card = slash;
//            extra.from = response->from;
//            extra.target = response->target;
//            extra.to = response->to;
//            return logic->respondCard(extra);
//        }

//        return false;
//    }
};

class Lijian: public ProactiveSkill
{
public:
    Lijian() : ProactiveSkill("lijian")
    {
    }

    bool isAvailable(const Player *self, const QString &pattern) const override
    {
        return ProactiveSkill::isAvailable(self, pattern) && self->skillHistory(this) <= 0;
    }

    bool cardFilter(const QList<const Card *> &selected, const Card *, const Player *, const QString &) const override
    {
        return selected.isEmpty();
    }

    bool cardFeasible(const QList<const Card *> &selected, const Player *) const
    {
        return selected.length() == 1;
    }

    bool playerFilter(const QList<const Player *> &selected, const Player *toSelect, const Player *source) const
    {
        if (toSelect == source)
            return false;

        const General *general = toSelect->general();
        return general && general->gender() == General::Male && selected.length() < 2;
    }

    bool playerFeasible(const QList<const Player *> &selected, const Player *) const
    {
        return selected.length() == 2;
    }

    void effect(GameLogic *logic, ServerPlayer *, const QList<ServerPlayer *> &to, const QList<Card *> &cards) const
    {
        CardsMoveStruct move;
        move.cards = cards;
        move.to.type = CardArea::DiscardPile;
        move.isOpen = true;
        logic->moveCards(move);

        Duel *duel = new Duel(Card::NoSuit, 0);
        duel->setSkill(this);

        CardUseStruct use;
        use.from = to.at(1);
        use.to << to.at(0);
        use.card = duel;
        logic->useCard(use);
    }
};

class Biyue: public TriggerSkill
{
public:
    Biyue() : TriggerSkill("biyue")
    {
        m_events << PhaseStart;
    }

//     bool triggerable(ServerPlayer *owner) const override
//     {
//         return TriggerSkill::triggerable(owner) && owner->phase() == Player::Finish;
//     }
// 
//     bool effect(GameLogic *, EventType, ServerPlayer *target, QVariant &, ServerPlayer *) const override
//     {
//         target->drawCards(1);
//         return false;
//     }
};

}

void StandardPackage::addQunGenerals()
{
    // QUN 001
    General *huatuo = new General("huatuo", "qun", 3);
    huatuo->addSkill(new Qingnang);
    huatuo->addSkill(new Jijiu);
    addGeneral(huatuo);

    // QUN 002
    General *lvbu = new General("lvbu", "qun", 4);
    lvbu->addSkill(new Wushuang);
    addGeneral(lvbu);

    // QUN 003
    General *diaochan = new General("diaochan", "qun", 3, General::Female);
    diaochan->addSkill(new Lijian);
    diaochan->addSkill(new Biyue);
    addGeneral(diaochan);
}
