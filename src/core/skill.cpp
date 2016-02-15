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
#include "general.h"
#include "skill.h"
#include "serverplayer.h"
#include "event.h"
#include "gamelogic.h"

Skill::Skill(const QString &name)
    : m_id(0)
    , m_name(name)
    , m_type(InvalidType)
    , m_subtype(InvalidType)
    , m_frequency(NotFrequent)
    , m_lordSkill(false)
    , m_topSkill(nullptr)
{
    // Id is only assigned to skills directly attached to a general. Subskill id is 0.
}

Skill::~Skill()
{
    foreach (const Skill *subskill, m_subskills)
        delete subskill;
}

const Skill *Skill::topSkill() const
{
    if (m_topSkill == nullptr)
        return this;
    return m_topSkill;
}

void Skill::addSubskill(Skill *subskill)
{
    subskill->m_topSkill = this;
    m_subskills << subskill;
}

TriggerSkill::TriggerSkill(const QString &name)
    : Skill(name)
{
    EventHandler::m_name = name;
    m_type = TriggerType;
    m_priority = 1;
}

bool TriggerSkill::triggerable(ServerPlayer *owner) const
{
    return owner != nullptr && owner->isAlive() && owner->hasSkill(this);
}

EventList TriggerSkill::triggerable(GameLogic *logic, EventType, const QVariant &, ServerPlayer *player) const
{
    if (triggerable(player))
        return EventList() << Event(logic, this, player, player, m_frequency == Compulsory || m_frequency == Wake);

    return EventList();
}

bool TriggerSkill::onCost(GameLogic *logic, EventType event, EventPtr eventPtr, QVariant &data, ServerPlayer *player) const
{
    bool takeEffect = cost(logic, event, eventPtr, data, player);
    if (takeEffect) {
        // hegemony mode show general
        const General *headGeneral = eventPtr->owner->headGeneral();
        if (headGeneral->hasSkill(this)) {
            if (!eventPtr->owner->hasShownHeadGeneral()) {
                eventPtr->owner->setHeadGeneralShown(true);
                eventPtr->owner->broadcastProperty("headGeneralId");
            }
        } else {
            const General *deputyGeneral = eventPtr->owner->deputyGeneral();
            if (deputyGeneral && deputyGeneral->hasSkill(this) && !eventPtr->owner->hasShownDeputyGeneral()) {
                eventPtr->owner->setDeputyGeneralShown(true);
                eventPtr->owner->broadcastProperty("deputyGeneralId");
            }
        }

        eventPtr->owner->addSkillHistory(this);
    }
    return takeEffect;
}

bool TriggerSkill::cost(GameLogic *, EventType, EventPtr, QVariant &, ServerPlayer *) const
{
    return true;
}

void TriggerSkill::setFrequency(Skill::Frequency frequency)
{
    m_frequency = frequency;
}

StatusSkill::StatusSkill(const QString &name)
    : TriggerSkill(name)
{
    m_events << SkillAdded << SkillRemoved;
    setFrequency(Compulsory);
}

bool StatusSkill::isValid(ServerPlayer *) const
{
    return true;
}

bool StatusSkill::effect(GameLogic *, EventType event, EventPtr eventPtr, QVariant &, ServerPlayer *player) const
{
    if (event == SkillAdded) {
        validate(player);
    } else if (event == SkillRemoved) {
        invalidate(player);
    } else {
        if (isValid(player))
            validate(player);
        else
            invalidate(player);
    }
    return false;
}

MasochismSkill::MasochismSkill(const QString &name)
    : TriggerSkill(name)
{
    m_events << AfterDamaged;
}

EventList MasochismSkill::triggerable(GameLogic *logic, EventType, const QVariant &data, ServerPlayer *player) const
{
    if (!TriggerSkill::triggerable(player))
        return EventList();

    DamageStruct *damage = data.value<DamageStruct *>();
    int times = triggerable(logic, player, *damage);
    if (times <= 0)
        return EventList();

    EventList l;
    for (int i = 0; i < times; ++i)
        l << Event(logic, this, player, player, m_frequency == Compulsory || m_frequency == Wake);

    return l;
}

bool MasochismSkill::effect(GameLogic *logic, EventType, EventPtr eventPtr, QVariant &data, ServerPlayer *player) const
{
    DamageStruct *damage = data.value<DamageStruct *>();
    effect(logic, player, eventPtr, *damage);
    return false;
}

ViewAsSkill::ViewAsSkill(const QString &name)
    : Skill(name)
{
    m_type = ViewAsType;
    m_subtype = ConvertType;
}

bool ViewAsSkill::isAvailable(const Player *, const QString &pattern) const
{
    return pattern.isEmpty();
}

OneCardViewAsSkill::OneCardViewAsSkill(const QString &name)
    : ViewAsSkill(name)
{
}

bool OneCardViewAsSkill::viewFilter(const QList<const Card *> &selected, const Card *card, const Player *self, const QString &pattern) const
{
    return selected.isEmpty() && viewFilter(card, self, pattern);
}

Card *OneCardViewAsSkill::viewAs(const QList<Card *> &cards, const Player *self) const
{
    if (cards.length() == 1)
        return viewAs(cards.first(), self);
    return nullptr;
}

//For the client only. Do not use it on the server side
class SkillCard : public Card
{
public:
    SkillCard(const ProactiveSkill *skill)
        : m_skill(skill)
    {
        m_targetFixed = false;
    }

    bool targetFeasible(const QList<const Player *> &targets, const Player *self) const override
    {
        return m_skill->playerFeasible(targets, self);
    }

    bool targetFilter(const QList<const Player *> &targets, const Player *toSelect, const Player *self) const override
    {
        return m_skill->playerFilter(targets, toSelect, self);
    }

private:
    const ProactiveSkill *m_skill;
};

ProactiveSkill::ProactiveSkill(const QString &name)
    : ViewAsSkill(name)
{
    m_subtype = ProactiveType;
}

bool ProactiveSkill::isAvailable(const Player *, const QString &pattern) const
{
    return pattern.isEmpty();
}

bool ProactiveSkill::cardFeasible(const QList<const Card *> &, const Player *) const
{
    return true;
}

bool ProactiveSkill::cardFilter(const QList<const Card *> &, const Card *, const Player *, const QString &) const
{
    return false;
}

bool ProactiveSkill::playerFeasible(const QList<const Player *> &, const Player *) const
{
    return true;
}

bool ProactiveSkill::playerFilter(const QList<const Player *> &, const Player *, const Player *) const
{
    return false;
}

bool ProactiveSkill::cost(GameLogic *, ServerPlayer *, const QList<ServerPlayer *> &, const QList<Card *> &) const
{
    return false;
}

void ProactiveSkill::effect(GameLogic *, ServerPlayer *, const QList<ServerPlayer *> &, const QList<Card *> &) const
{

}

bool ProactiveSkill::viewFilter(const QList<const Card *> &selected, const Card *card, const Player *source, const QString &pattern) const
{
    return cardFilter(selected, card, source, pattern);
}

Card *ProactiveSkill::viewAs(const QList<Card *> &cards, const Player *source) const
{
    QList<const Card *> constCards;
    constCards.reserve(cards.length());
    foreach (Card *card, cards)
        constCards << card;

    if (cardFeasible(constCards, source))
        return new SkillCard(this);

    return nullptr;
}

class RetrialSkill::Timing : public TriggerSkill
{
public:
    Timing(const QString &skillName, const RetrialSkill *parent)
        : TriggerSkill(skillName), m_parent(parent)
    {
        m_events << AskForRetrial;
    }

    EventList triggerable(GameLogic *logic, EventType, const QVariant &, ServerPlayer * /* = nullptr */) const final override
    {
        // player is nullptr in this method so we don't use this player
        EventList l;
        foreach (ServerPlayer *p, logic->allPlayers()) {
            if (p->hasSkill(m_parent) && m_parent->isAvailable(p, "@" + name()))
                l << Event(logic, this, p, p);
        }
        return l;
    }

    bool cost(GameLogic *logic, EventType, EventPtr eventPtr, QVariant &data, ServerPlayer * /* = nullptr */) const final override
    {
        JudgeStruct *judge = data.value<JudgeStruct *>();
        eventPtr->invoker->showPrompt(name() + "_ask_for_retrial", judge->who);
        SkillInvokeStruct s = eventPtr->invoker->askToInvokeSkill(m_parent);
        if (s.skill == m_parent && s.cards.length() == 1) {
            logic->retrialCost(*judge, s.cards.first(), m_parent->m_isReplace);
            Card *card = s.cards.first();
            eventPtr->invoker->tag[name() + "_retrialCard"] = QVariant::fromValue(card);

            return true;
        }

        return false;
    }

    bool effect(GameLogic *logic, EventType, EventPtr eventPtr, QVariant &data, ServerPlayer * /* = nullptr */) const final override
    {
        JudgeStruct *judge = data.value<JudgeStruct *>();
        Card *card = eventPtr->invoker->tag.value(name() + "_retrialCard").value<Card *>();
        logic->retrialEffect(*judge, card);

        return false;
    }

private:
    const RetrialSkill *m_parent;
};

RetrialSkill::RetrialSkill(const QString &name, bool isReplace /* = false */)
    : ProactiveSkill(name), m_isReplace(isReplace)
{
    Timing *timing = new Timing(name, this);
    addSubskill(timing);
}

CardModSkill::CardModSkill(const QString &name)
    : Skill(name)
{
    m_type = CardModType;
}

bool CardModSkill::targetFilter(const Card *, const QList<const Player *> &, const Player *, const Player *) const
{
    return true;
}

int CardModSkill::extraDistanceLimit(const Card *, const QList<const Player *> &, const Player *, const Player *) const
{
    return 0;
}

int CardModSkill::extraMaxTargetNum(const Card *, const QList<const Player *> &, const Player *, const Player *) const
{
    return 0;
}

int CardModSkill::extraUseNum(const Card *, const Player *) const
{
    return 0;
}

ProactiveSkillTiming::ProactiveSkillTiming(const QString &name, const ProactiveSkill *parent)
    : TriggerSkill(name)
    , m_parent(parent)
{

}

bool ProactiveSkillTiming::cost(GameLogic *logic, EventType, EventPtr eventPtr, QVariant &, ServerPlayer * /* = nullptr */) const
{
    SkillInvokeStruct *s = new SkillInvokeStruct(eventPtr->invoker->askToInvokeSkill(m_parent));
    const ProactiveSkill *proactiveSkill = dynamic_cast<const ProactiveSkill *>(s->skill);
    if (proactiveSkill != nullptr && proactiveSkill->cost(logic, s->player, s->targets, s->cards)) {
        eventPtr->tag["invoke"] = QVariant::fromValue(s);
        return true;
    }

    return false;
}

bool ProactiveSkillTiming::effect(GameLogic *logic, EventType, EventPtr eventPtr, QVariant &, ServerPlayer * /* = nullptr */) const
{
    SkillInvokeStruct *s = eventPtr->tag.value("invoke").value<SkillInvokeStruct *>();
    const ProactiveSkill *proactiveSkill = dynamic_cast<const ProactiveSkill *>(s->skill);
    try {
        if (proactiveSkill != nullptr)
            proactiveSkill->effect(logic, s->player, s->targets, s->cards);

        delete s;
    }
    catch (...) {
        delete s;
        throw;
    }

    return false;
}
