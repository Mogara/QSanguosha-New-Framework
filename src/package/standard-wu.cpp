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
#include "standard-trickcard.h"
#include "structs.h"

namespace {

class Zhiheng : public ProactiveSkill
{
public:
    Zhiheng() : ProactiveSkill("zhiheng")
    {
    }

    bool isAvailable(const Player *self, const QString &pattern) const override
    {
        return ProactiveSkill::isAvailable(self, pattern) && self->skillHistory(this) <= 0;
    }

    bool cardFilter(const QList<const Card *> &, const Card *, const Player *, const QString &) const override
    {
        return true;
    }

    bool cardFeasible(const QList<const Card *> &selected, const Player *) const override
    {
        return selected.length() > 0;
    }

    bool playerFilter(const QList<const Player *> &, const Player *, const Player *) const override
    {
        return false;
    }

    bool playerFeasible(const QList<const Player *> &selected, const Player *) const override
    {
        return selected.isEmpty();
    }

    void effect(GameLogic *logic, ServerPlayer *from, const QList<ServerPlayer *> &, const QList<Card *> &cards) const override
    {
        CardsMoveStruct move;
        move.cards = cards;
        move.to.type = CardArea::DiscardPile;
        move.isOpen = true;
        logic->moveCards(move);

        from->drawCards(cards.length());
    }
};

class Qixi : public OneCardViewAsSkill
{
public:
    Qixi() : OneCardViewAsSkill("qixi")
    {
    }

    bool viewFilter(const Card *card, const Player *, const QString &) const override
    {
        return card->color() == Card::Black;
    }

    Card *viewAs(Card *card, const Player *) const override
    {
        Dismantlement *dismantlement = new Dismantlement(card->suit(), card->number());
        dismantlement->addSubcard(card);
        dismantlement->setSkill(this);
        return dismantlement;
    }
};

class Keji : public TriggerSkill
{
    class Record : public TriggerSkill
    {
    public:
        Record() : TriggerSkill("keji_record")
        {
            m_events << PreCardUsed << CardResponded;
            setFrequency(Compulsory);
        }

        EventList triggerable(GameLogic *, EventType event, ServerPlayer *player, QVariant &data, ServerPlayer *) const override
        {
            if (player->phase() == Player::Play && TriggerSkill::triggerable(player)) {
                const Card *card = nullptr;
                if (event == PreCardUsed)
                    card = data.value<CardUseStruct *>()->card;
                else
                    card = data.value<CardResponseStruct *>()->card;
                if (card->inherits("Slash"))
                    player->tag["KejiSlashInPlayPhase"] = true;
            }
            return EventList();
        }
    };

public:
    Keji() : TriggerSkill("keji")
    {
        m_events << PhaseStart;
        addSubskill(new Record);
    }

    bool triggerable(ServerPlayer *owner) const override
    {
        if (owner->phase() != Player::Discard)
            return false;

        bool slashInPlayPhase = owner->tag.value("KejiSlashInPlayPhase").toBool();
        if (slashInPlayPhase) {
            owner->tag.remove("KejiSlashInPlayPhase");
            return false;
        }

        return TriggerSkill::triggerable(owner);
    }

    bool effect(GameLogic *, EventType, ServerPlayer *, QVariant &, ServerPlayer *) const override
    {
        return true;
    }
};

class Kurou : public ProactiveSkill
{
public:
    Kurou() : ProactiveSkill("kurou")
    {
    }

    bool isAvailable(const Player *self, const QString &pattern) const
    {
        return ProactiveSkill::isAvailable(self, pattern) && self->hp() > 0;
    }

    bool cardFeasible(const QList<const Card *> &selected, const Player *) const override
    {
        return selected.isEmpty();
    }

    void effect(GameLogic *logic, ServerPlayer *from, const QList<ServerPlayer *> &, const QList<Card *> &) const override
    {
        logic->loseHp(from, 1);
        from->drawCards(2);
    }
};

class Yingzi : public TriggerSkill
{
public:
    Yingzi() : TriggerSkill("yingzi")
    {
        m_events << DrawNCards;
    }

    bool effect(GameLogic *, EventType, ServerPlayer *, QVariant &data, ServerPlayer *) const override
    {
        int drawNum = data.toInt();
        drawNum++;
        data = drawNum;
        return false;
    }
};

class Fanjian : public ProactiveSkill
{
public:
    Fanjian() : ProactiveSkill("fanjian")
    {
    }

    bool isAvailable(const Player *self, const QString &pattern) const override
    {
        return ProactiveSkill::isAvailable(self, pattern) && self->skillHistory(this) <= 0;
    }

    bool cardFeasible(const QList<const Card *> &selected, const Player *) const override
    {
        return selected.isEmpty();
    }

    bool playerFeasible(const QList<const Player *> &selected, const Player *) const override
    {
        return selected.length() == 1;
    }

    bool playerFilter(const QList<const Player *> &selected, const Player *toSelect, const Player *source) const override
    {
        return selected.isEmpty() && toSelect != source;
    }

    void effect(GameLogic *logic, ServerPlayer *from, const QList<ServerPlayer *> &to, const QList<Card *> &) const override
    {
        ServerPlayer *victim = to.first();

        QStringList options;
        options << "spade" << "heart" << "club" << "diamond";
        QString suit = victim->askForOption(options);
        //@to-do: add a log that shows the result

        Card *card = victim->askToChooseCard(from, "h");
        victim->showCard(card);
        if (card) {
            CardsMoveStruct move;
            move.cards << card;
            move.to.owner = victim;
            move.to.type = CardArea::Hand;
            logic->moveCards(move);

            if (card->suitString() != suit) {
                DamageStruct damage;
                damage.from = from;
                damage.to = victim;
                logic->damage(damage);
            }
        }
    }
};

class Guose : public OneCardViewAsSkill
{
public:
    Guose() : OneCardViewAsSkill("guose")
    {
    }

    bool viewFilter(const Card *card, const Player *, const QString &pattern) const override
    {
        return card->suit() == Card::Diamond && pattern.isEmpty();
    }

    Card *viewAs(Card *card, const Player *) const override
    {
        Card *indulgence = new Indulgence(card->suit(), card->number());
        indulgence->addSubcard(card);
        indulgence->setSkill(this);
        return indulgence;
    }
};

class Liuli : public ProactiveSkill
{
    class Timing : public TriggerSkill
    {
    public:
        Timing() : TriggerSkill("liuli")
        {
            m_events << TargetConfirming;
        }

        EventList triggerable(GameLogic *logic, EventType, ServerPlayer *target, QVariant &data, ServerPlayer *) const override
        {
            EventList events;
            if (!TriggerSkill::triggerable(target))
                return events;

            CardUseStruct *use = data.value<CardUseStruct *>();
            if (use->card->inherits("Slash") && use->to.contains(target)) {
                QList<ServerPlayer *> players = logic->otherPlayers(target);
                players.removeOne(use->from);

                foreach (ServerPlayer *p, players) {
                    if (p->inAttackRangeOf(target)) {
                        Event e(this, target);
                        events << e;
                        break;
                    }
                }
            }

            return events;
        }

        bool cost(GameLogic *, EventType, ServerPlayer *target, QVariant &data, ServerPlayer *) const override
        {
            CardUseStruct *use = data.value<CardUseStruct *>();
            use->from->tag["liuli_slash_source"] = true;
            use->from->unicastTagTo("liuli_slash_source", target);
            target->showPrompt("invoke_liuli", use->from);
            return target->askToUseCard("@liuli");
        }

        bool effect(GameLogic *logic, EventType, ServerPlayer *daqiao, QVariant &data, ServerPlayer *) const override
        {
            CardUseStruct *use = data.value<CardUseStruct *>();

            QList<ServerPlayer *> players = logic->otherPlayers(daqiao);
            foreach (ServerPlayer *target, players) {
                if (target->tag.contains("liuli_target")) {
                    target->tag.remove("liuli_target");
                    use->to.removeOne(daqiao);
                    use->to.append(target);
                    logic->sortByActionOrder(use->to);
                    logic->trigger(TargetConfirming, target, data);
                    return false;
                }
            }

            return false;
        }
    };

public:
    Liuli() : ProactiveSkill("liuli")
    {
        addSubskill(new Timing);
    }

    bool isAvailable(const Player *, const QString &pattern) const override
    {
        return pattern == "@liuli";
    }

    bool cardFeasible(const QList<const Card *> &selected, const Player *) const override
    {
        return selected.length() == 1;
    }

    bool cardFilter(const QList<const Card *> &selected, const Card *, const Player *, const QString &pattern) const override
    {
        return selected.isEmpty() && pattern == "@liuli";
    }

    bool playerFeasible(const QList<const Player *> &selected, const Player *) const override
    {
        return selected.length() == 1;
    }

    bool playerFilter(const QList<const Player *> &selected, const Player *toSelect, const Player *source) const override
    {
        if (!selected.isEmpty())
            return false;

        return !toSelect->tag.contains("liuli_slash_source") && toSelect->inAttackRangeOf(source);
    }

    void effect(GameLogic *logic, ServerPlayer *, const QList<ServerPlayer *> &to, const QList<Card *> &cards) const
    {
        CardsMoveStruct move;
        move.cards = cards;
        move.to.type = CardArea::DiscardPile;
        move.isOpen = true;
        logic->moveCards(move);

        foreach (ServerPlayer *target, to)
            target->tag["liuli_target"] = true;
    }
};

class Qianxun : public CardModSkill
{
public:
    Qianxun() : CardModSkill("qianxun")
    {
    }

    bool targetFilter(const Card *card, const QList<const Player *> &, const Player *toSelect, const Player *) const override
    {
        if (card->inherits("Indulgence") || card->inherits("Snatch"))
            return !toSelect->hasSkill(this);
        return true;
    }
};

}

void StandardPackage::addWuGenerals()
{
    //WU 001
    General *sunquan = new General("sunquan", "wu", 4);
    sunquan->setLord(true);
    sunquan->addSkill(new Zhiheng);
    addGeneral(sunquan);

    //WU 002
    General *ganning = new General("ganning", "wu", 4);
    ganning->addSkill(new Qixi);
    addGeneral(ganning);

    //WU 003
    General *lvmeng = new General("lvmeng", "wu", 4);
    lvmeng->addSkill(new Keji);
    addGeneral(lvmeng);

    //WU 004
    General *huanggai = new General("huanggai", "wu", 4);
    huanggai->addSkill(new Kurou);
    addGeneral(huanggai);

    //WU 005
    General *zhouyu = new General("zhouyu", "wu", 3);
    zhouyu->addSkill(new Yingzi);
    zhouyu->addSkill(new Fanjian);
    addGeneral(zhouyu);

    //WU 006
    General *daqiao = new General("daqiao", "wu", 3, General::Female);
    daqiao->addSkill(new Guose);
    daqiao->addSkill(new Liuli);
    addGeneral(daqiao);

    //WU 007
    General *luxun = new General("luxun", "wu", 3);
    luxun->addSkill(new Qianxun);
    addGeneral(luxun);

    //WU 008
    General *sunshangxiang = new General("sunshangxiang", "wu", 3, General::Female);
    addGeneral(sunshangxiang);
}
