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
#include "serverplayer.h"
#include "standardpackage.h"
#include "gamelogic.h"
#include "general.h"
#include "skill.h"
#include "standard-basiccard.h"

namespace {

class Jianxiong : public MasochismSkill
{
public:
    Jianxiong() : MasochismSkill("jianxiong")
    {
    }

    int triggerable(GameLogic *, ServerPlayer *, DamageStruct &damage) const override
    {
        if (damage.card)
            return 1;

        return 0;
    }

    bool effect(GameLogic *logic, ServerPlayer *target, DamageStruct &damage) const override
    {
        CardsMoveStruct obtain;
        obtain.cards << damage.card;
        obtain.to.owner = target;
        obtain.to.type = CardArea::Hand;
        obtain.isOpen = true;
        logic->moveCards(obtain);
        return false;
    }
};

class Fankui : public MasochismSkill
{
public:
    Fankui() : MasochismSkill("fankui")
    {
    }

    int triggerable(GameLogic *, ServerPlayer *, DamageStruct &damage) const override
    {
        if (damage.from && damage.from->handcardNum() + damage.from->equipNum() > 0)
            return 1;

        return 0;
    }

    bool effect(GameLogic *logic, ServerPlayer *target, DamageStruct &damage) const override
    {
        Card *card = target->askToChooseCard(damage.from, "he");
        if (card) {
            CardsMoveStruct obtain;
            obtain.cards << card;
            obtain.to.owner = target;
            obtain.to.type = CardArea::Hand;
            logic->moveCards(obtain);
        }

        return false;
    }
};

class Guicai : public ProactiveSkill
{
    class Timing : public TriggerSkill
    {
    public:
        Timing() : TriggerSkill("guicai")
        {
            m_events << AskForRetrial;
            setFrequency(Compulsory);
        }

        bool cost(GameLogic *, EventType, ServerPlayer *target, QVariant &data, ServerPlayer *) const override
        {
            JudgeStruct *judge = data.value<JudgeStruct *>();
            target->showPrompt("guicai_ask_for_retrial", judge->who);
            return target->askToUseCard("@guicai");
        }

        bool effect(GameLogic *logic, EventType, ServerPlayer *target, QVariant &data, ServerPlayer *) const override
        {
            uint cardId = target->tag["guicai_card"].toUInt();
            target->tag.remove("guicai_card");
            Card *card = logic->findCard(cardId);
            if (card) {
                JudgeStruct *judge = data.value<JudgeStruct *>();
                judge->card = card;
                judge->updateResult();
            }
            return false;
        }
    };

public:
    Guicai() : ProactiveSkill("guicai")
    {
        addSubskill(new Timing);
    }

    bool isAvailable(const Player *self, const QString &pattern) const override
    {
        return self->handcardNum() > 0 && pattern == "@guicai";
    }

    bool cardFilter(const QList<const Card *> &selected, const Card *card, const Player *source, const QString &) const override
    {
        return selected.isEmpty() && source->handcardArea()->contains(card);
    }

    bool cardFeasible(const QList<const Card *> &selected, const Player *) const override
    {
        return selected.length() == 1;
    }

    void effect(GameLogic *logic, ServerPlayer *from, const QList<ServerPlayer *> &, const QList<Card *> &cards) const override
    {
        Card *card = cards.first();
        CardResponseStruct response;
        response.card = card;
        response.from = from;
        logic->respondCard(response);

        from->tag["guicai_card"] = cards.first()->id();
    }
};

class Qingguo : public OneCardViewAsSkill
{
public:
    Qingguo() : OneCardViewAsSkill("qingguo")
    {
    }

    bool viewFilter(const Card *card, const Player *, const QString &) const override
    {
        return card->color() == Card::Black;
    }

    Card *viewAs(Card *subcard, const Player *) const override
    {
        Jink *jink = new Jink(subcard->suit(), subcard->number());
        jink->setSkill(this);
        jink->addSubcard(subcard);
        return jink;
    }

    bool isAvailable(const Player *, const QString &pattern) const override
    {
        return pattern == "Jink";
    }
};

class Ganglie : public TriggerSkill // MasochismSkill // for compiling
{
public:
    Ganglie() : TriggerSkill("ganglie")
    {
    }

//    int triggerable(GameLogic *, ServerPlayer *, DamageStruct &) const override
//    {
//        return 1;
//    }

//    bool effect(GameLogic *logic, ServerPlayer *target, DamageStruct &damage) const override
//    {
//        JudgeStruct judge(".|^heart");
//        judge.who = target;
//        logic->judge(judge);

//        if (judge.matched && damage.from) {
//            damage.from->showPrompt("ganglie_discard_cards", target);
//            QList<Card *> cards = damage.from->askForCards(".|.|.|hand", 2, true);
//            if (cards.length() == 2) {
//                CardsMoveStruct discard;
//                discard.cards = cards;
//                discard.to.type = CardArea::DiscardPile;
//                discard.isOpen = true;
//                logic->moveCards(discard);
//            } else {
//                DamageStruct revenge;
//                revenge.from = target;
//                revenge.to = damage.from;
//                logic->damage(revenge);
//            }
//        }

//        return false;
//    }
};

class Tuxi : public ProactiveSkill
{
    class Timing : public TriggerSkill
    {
    public:
        Timing() : TriggerSkill("tuxi")
        {
            m_events << PhaseStart;
        }

//        bool triggerable(ServerPlayer *owner) const override
//        {
//            return TriggerSkill::triggerable(owner) && owner->phase() == Player::Draw;
//        }

//        bool cost(GameLogic *, EventType, ServerPlayer *target, QVariant &, ServerPlayer *) const override
//        {
//            target->showPrompt("tuxi_select_players");
//            return target->askToUseCard("@tuxi");
//        }

//        bool effect(GameLogic *logic, EventType, ServerPlayer *from, QVariant &, ServerPlayer *) const override
//        {
//            if (!from->tag.contains("tuxi_victims"))
//                return false;

//            QList<ServerPlayer *> victims;
//            QVariantList data = from->tag.value("tuxi_victims").toList();
//            foreach (const QVariant &playerId, data) {
//                ServerPlayer *victim = logic->findPlayer(playerId.toUInt());
//                if (victim)
//                    victims << victim;
//            }
//            logic->sortByActionOrder(victims);

//            foreach (ServerPlayer *victim, victims) {
//                Card *card = from->askToChooseCard(victim, "h");

//                CardsMoveStruct obtain;
//                obtain.cards << card;
//                obtain.to.owner = from;
//                obtain.to.type = CardArea::Hand;
//                logic->moveCards(obtain);
//            }

//            return true;
//        }
    };

public:
    Tuxi() : ProactiveSkill("tuxi")
    {
        addSubskill(new Timing);
    }

    bool isAvailable(const Player *, const QString &pattern) const override
    {
        return pattern == "@tuxi";
    }

    bool playerFilter(const QList<const Player *> &selected, const Player *toSelect, const Player *source) const override
    {
        return selected.length() < 2 && toSelect != source && toSelect->handcardNum() > 0;
    }

    bool playerFeasible(const QList<const Player *> &selected, const Player *) const override
    {
        int num = selected.length();
        return 1 <= num && num <= 2;
    }

    void effect(GameLogic *, ServerPlayer *from, const QList<ServerPlayer *> &to, const QList<Card *> &) const override
    {
        QVariantList victims;
        foreach (ServerPlayer *victim, to)
            victims << victim->id();
        from->tag["tuxi_victims"] = victims;
    }
};

class Luoyi : public TriggerSkill
{
    class Effect : public TriggerSkill
    {
    public:
        Effect() : TriggerSkill("luoyi")
        {
            m_events << Damaging;
            setFrequency(Compulsory);
        }

//        EventList triggerable(GameLogic *, EventType, ServerPlayer *owner, QVariant &data, ServerPlayer *) const override
//        {
//            if (!TriggerSkill::triggerable(owner) || !owner->tag.contains("luoyi_effect"))
//                return EventList();

//            DamageStruct *damage = data.value<DamageStruct *>();
//            if (damage->card && (damage->card->inherits("Slash") || damage->card->inherits("Duel")) && damage->to != owner)
//                damage->damage++;

//            return EventList();
//        }
    };

    class Reset : public TriggerSkill
    {
    public:
        Reset() : TriggerSkill("luoyi")
        {
            m_events << PhaseEnd;
            setFrequency(Compulsory);
        }

//        bool triggerable(ServerPlayer *owner) const override
//        {
//            if (TriggerSkill::triggerable(owner) && owner->phase() == Player::Play && owner->tag.contains("luoyi_effect"))
//                owner->tag.remove("luoyi_effect");
//            return false;
//        }
    };

public:
    Luoyi() : TriggerSkill("luoyi")
    {
        m_events << DrawNCards;

        addSubskill(new Effect);
        addSubskill(new Reset);
    }

//    bool effect(GameLogic *, EventType, ServerPlayer *target, QVariant &data, ServerPlayer *) const override
//    {
//        int drawNum = data.toInt();
//        drawNum--;
//        data = drawNum;

//        target->tag["luoyi_effect"] = true;

//        return false;
//    }
};

class Tiandu : public TriggerSkill
{
public:
    Tiandu() : TriggerSkill("tiandu")
    {
        m_events << FinishJudge;
    }

//    EventList triggerable(GameLogic *, EventType, ServerPlayer *player, QVariant &data, ServerPlayer *) const override
//    {
//        if (TriggerSkill::triggerable(player)) {
//            JudgeStruct *judge = data.value<JudgeStruct *>();
//            if (judge->card && player->judgeCards()->contains(judge->card))
//                return Event(this, player);
//        }
//        return EventList();
//    }

//    bool effect(GameLogic *logic, EventType, ServerPlayer *player, QVariant &data, ServerPlayer *) const override
//    {
//        JudgeStruct *judge = data.value<JudgeStruct *>();
//        CardsMoveStruct obtain;
//        obtain.cards << judge->card;
//        obtain.to.owner = player;
//        obtain.to.type = CardArea::Hand;
//        logic->moveCards(obtain);
//        return false;
//    }
};

} //namespace

void StandardPackage::addWeiGenerals()
{
    // WEI 001
    General *caocao = new General("caocao", "wei", 4);
    caocao->setLord(true);
    caocao->addSkill(new Jianxiong);
    addGeneral(caocao);

    // WEI 002
    General *simayi = new General("simayi", "wei", 3);
    simayi->addSkill(new Fankui);
    simayi->addSkill(new Guicai);
    addGeneral(simayi);

    // WEI 003
    General *xiahoudun = new General("xiahoudun", "wei", 4);
    xiahoudun->addSkill(new Ganglie);
    addGeneral(xiahoudun);

    // WEI 004
    General *zhangliao = new General("zhangliao", "wei", 4);
    zhangliao->addSkill(new Tuxi);
    addGeneral(zhangliao);

    // WEI 005
    General *xuchu = new General("xuchu", "wei", 4);
    xuchu->addSkill(new Luoyi);
    addGeneral(xuchu);

    // WEI 006
    General *guojia = new General("guojia", "wei", 3);
    guojia->addSkill(new Tiandu);
    addGeneral(guojia);

    // WEI 007
    General *zhenji = new General("zhenji", "wei", 3, General::Female);
    zhenji->addSkill(new Qingguo);
    addGeneral(zhenji);
}
