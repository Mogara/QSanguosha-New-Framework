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
#include "gamelogic.h"
#include "general.h"
#include "serverplayer.h"
#include "skill.h"
#include "standard-basiccard.h"
#include "structs.h"

namespace
{

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
        const CardArea *handcard = self->handcardArea();
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
        if (oldValue < 2 && newValue >= 2 && from->isWounded()) {
            RecoverStruct recover;
            recover.from = from;
            recover.to = from;
            logic->recover(recover);
        }
    }
};

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
        slash->setSkill(this);
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

class Kongcheng : public CardModSkill
{
public:
    Kongcheng() : CardModSkill("kongcheng")
    {
        m_frequency = Compulsory;
    }

    bool targetFilter(const Card *card, const QList<const Player *> &, const Player *toSelect, const Player *) const override
    {
        if (card->inherits("Slash") || card->inherits("Duel")) {
            if (toSelect->handcardNum() <= 0 && toSelect->hasSkill(this))
                return false;
        }
        return true;
    }
};

class Longdan : public OneCardViewAsSkill
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
            card->setSkill(this);
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

class Mashu : public StatusSkill
{
public:
    Mashu() : StatusSkill("mashu")
    {
    }

    void validate(ServerPlayer *target) const override
    {
        int distance = target->extraOutDistance();
        target->setExtraOutDistance(distance - 1);
        target->broadcastProperty("extraOutDistance");
    }

    void invalidate(ServerPlayer *target) const override
    {
        int distance = target->extraOutDistance();
        target->setExtraOutDistance(distance + 1);
        target->broadcastProperty("extraOutDistance");
    }
};

class Tieqi : public TriggerSkill
{
    class Effect : public TriggerSkill
    {
    public:
        Effect() : TriggerSkill("tieqi_effect")
        {
            m_events << SlashProceed;
            setFrequency(Compulsory);
        }

        EventList triggerable(GameLogic *, EventType, ServerPlayer *, QVariant &data, ServerPlayer *) const override
        {
            EventList events;
            SlashEffectStruct *effect = data.value<SlashEffectStruct *>();
            if (effect->from->tag.contains("tieqi_victims")) {
                QVariantList victims = effect->from->tag.value("tieqi_victims").toList();
                int max = victims.length();
                for (int i = 0; i < max; i++) {
                    if (victims.at(i).toUInt() == effect->to->id()) {
                        victims.removeAt(i);
                        if (victims.isEmpty())
                            effect->from->tag.remove("tieqi_victims");
                        else
                            effect->from->tag["tieqi_victims"] = victims;

                        Event e(this, effect->from);
                        e.to << effect->to;
                        events << e;
                        break;
                    }
                }
            }
            return events;
        }

        bool effect(GameLogic *, EventType, ServerPlayer *, QVariant &, ServerPlayer *) const override
        {
            return true;
        }
    };

public:
    Tieqi() : TriggerSkill("tieqi")
    {
        m_events << TargetChosen;
        setFrequency(Frequent);

        addSubskill(new Effect);
    }

    EventList triggerable(GameLogic *, EventType, ServerPlayer *player, QVariant &data, ServerPlayer *) const override
    {
        EventList events;

        CardUseStruct *use = data.value<CardUseStruct *>();
        if (TriggerSkill::triggerable(player) && use->card && use->card->inherits("Slash")) {
            foreach(ServerPlayer *to, use->to) {
                Event e(this, player);
                e.to << to;
                events << e;
            }
        }

        return events;
    }

    bool effect(GameLogic *logic, EventType, ServerPlayer *target, QVariant &, ServerPlayer *invoker) const override
    {
        JudgeStruct judge(".|red");
        judge.who = invoker;
        logic->judge(judge);

        if (judge.matched) {
            QVariantList victims = invoker->tag.value("tieqi_victims").toList();
            victims << target->id();
            invoker->tag["tieqi_victims"] = victims;
        }

        return false;
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
        if (use->card && use->card->type() == Card::TrickType)
            return Event(this, player);
        return EventList();
    }

    bool effect(GameLogic *, EventType, ServerPlayer *target, QVariant &, ServerPlayer *) const override
    {
        target->drawCards(1);
        return false;
    }
};

class Qicai : public CardModSkill
{
public:
    Qicai() : CardModSkill("qicai")
    {
        m_frequency = Compulsory;
    }

    int extraDistanceLimit(const Card *card, const QList<const Player *> &, const Player *, const Player *source) const override
    {
        if (card->type() == Card::TrickType && source->hasSkill(this))
            return Card::InfinityNum;
        return 0;
    }
};

}

void StandardPackage::addShuGenerals()
{
    // SHU 001
    General *liubei = new General("liubei", "shu", 4);
    liubei->setLord(true);
    liubei->addSkill(new Rende);
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
    zhugeliang->addSkill(new Kongcheng);
    addGeneral(zhugeliang);

    // SHU 005
    General *zhaoyun = new General("zhaoyun", "shu", 4);
    zhaoyun->addSkill(new Longdan);
    addGeneral(zhaoyun);

    // SHU 006
    General *machao = new General("machao", "shu", 4);
    machao->addSkill(new Tieqi);
    machao->addSkill(new Mashu);
    addGeneral(machao);

    // SHU 007
    General *huangyueying = new General("huangyueying", "shu", 3, General::Female);
    huangyueying->addSkill(new Jizhi);
    huangyueying->addSkill(new Qicai);
    addGeneral(huangyueying);
}
