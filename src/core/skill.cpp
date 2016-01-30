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
    m_defaultPriority = 1;
}

bool TriggerSkill::triggerable(ServerPlayer *owner) const
{
    return owner != nullptr && owner->isAlive() && owner->hasSkill(this);
}

bool TriggerSkill::onCost(GameLogic *logic, EventType event, ServerPlayer *target, QVariant &data, ServerPlayer *invoker) const
{
    const General *headGeneral = target->headGeneral();
    if (headGeneral->hasSkill(this)) {
        if (!target->hasShownHeadGeneral()) {
            target->setHeadGeneralShown(true);
            target->broadcastProperty("headGeneralId");
        }
    } else {
        const General *deputyGeneral = target->deputyGeneral();
        if (deputyGeneral && deputyGeneral->hasSkill(this) && !target->hasShownDeputyGeneral()) {
            target->setDeputyGeneralShown(true);
            target->broadcastProperty("deputyGeneralId");
        }
    }

    bool takeEffect = cost(logic, event, target, data, invoker);
    if (takeEffect) {
        if (invoker == target) {
            invoker->addSkillHistory(this);
        } else {
            QList<ServerPlayer *> targets;
            targets << target;
            invoker->addSkillHistory(this, targets);
        }
    }
    return takeEffect;
}

bool TriggerSkill::cost(GameLogic *logic, EventType event, ServerPlayer *target, QVariant &data, ServerPlayer *invoker) const
{
    C_UNUSED(logic);
    C_UNUSED(event);
    C_UNUSED(target);
    C_UNUSED(data);
    C_UNUSED(invoker);
    return true;
}

void TriggerSkill::setFrequency(Skill::Frequency frequency)
{
    m_frequency = frequency;
    m_compulsory = (m_frequency == Compulsory || m_frequency == Wake);
}

StatusSkill::StatusSkill(const QString &name)
    : TriggerSkill(name)
{
    m_events << SkillAdded << SkillRemoved;
    setFrequency(Compulsory);
}

bool StatusSkill::isValid(ServerPlayer *target) const
{
    Q_UNUSED(target)
    return true;
}

bool StatusSkill::effect(GameLogic *, EventType event, ServerPlayer *target, QVariant &, ServerPlayer *) const
{
    if (event == SkillAdded) {
        validate(target);
    } else if (event == SkillRemoved) {
        invalidate(target);
    } else {
        if (isValid(target))
            validate(target);
        else
            invalidate(target);
    }
    return false;
}

ViewAsSkill::ViewAsSkill(const QString &name)
    : Skill(name)
{
    m_type = ViewAsType;
    m_subtype = ConvertType;
}

bool ViewAsSkill::isAvailable(const Player *self, const QString &pattern) const
{
    Q_UNUSED(self)
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

bool ProactiveSkill::isAvailable(const Player *self, const QString &pattern) const
{
    C_UNUSED(self);
    return pattern.isEmpty();
}

bool ProactiveSkill::cardFeasible(const QList<const Card *> &selected, const Player *source) const
{
    C_UNUSED(source);
    return selected.length() > 1;
}

bool ProactiveSkill::cardFilter(const QList<const Card *> &selected, const Card *card, const Player *source, const QString &pattern) const
{
    C_UNUSED(selected);
    C_UNUSED(card);
    C_UNUSED(source);
    C_UNUSED(pattern);
    return false;
}

bool ProactiveSkill::playerFeasible(const QList<const Player *> &selected, const Player *source) const
{
    C_UNUSED(selected);
    C_UNUSED(source);
    return true;
}

bool ProactiveSkill::playerFilter(const QList<const Player *> &selected, const Player *toSelect, const Player *source) const
{
    C_UNUSED(selected);
    C_UNUSED(toSelect);
    C_UNUSED(source);
    return false;
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


CardModSkill::CardModSkill(const QString &name)
    : Skill(name)
{
    m_type = CardModType;
}

bool CardModSkill::targetFilter(const Card *card, const QList<const Player *> &selected, const Player *toSelect, const Player *source) const
{
    C_UNUSED(card);
    C_UNUSED(selected);
    C_UNUSED(toSelect);
    C_UNUSED(source);
    return true;
}

int CardModSkill::extraDistanceLimit(const Card *card, const QList<const Player *> &selected, const Player *toSelect, const Player *source) const
{
    C_UNUSED(card);
    C_UNUSED(selected);
    C_UNUSED(toSelect);
    C_UNUSED(source);
    return 0;
}

int CardModSkill::extraMaxTargetNum(const Card *card, const QList<const Player *> &selected, const Player *toSelect, const Player *source) const
{
    C_UNUSED(card);
    C_UNUSED(selected);
    C_UNUSED(toSelect);
    C_UNUSED(source);
    return 0;
}

int CardModSkill::extraUseNum(const Card *card, const Player *player) const
{
    C_UNUSED(card);
    C_UNUSED(player);
    return 0;
}
