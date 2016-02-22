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
#include "engine.h"

#include <QThreadStorage>
#include <QCache>
#include <CJSEngine>
#include <QFile>
#include <QJSValueIterator>

namespace
{
    QThreadStorage<QCache<const Skill *, QJSValue> > skillFuncStorage;
}

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

const QJSValue &Skill::skillFuncs() const
{
    QCache<const Skill *, QJSValue> &skillFuncCache = skillFuncStorage.localData();
    if (skillFuncCache.contains(this))
        return *(skillFuncCache[this]);

    CJSEngine *jsEngine = Sanguosha.JsEngineInstance();
    // todo:check if the skills are loaded

    QJSValue r = jsEngine->globalObject().property(m_name);
    if (r.isObject()) {
        QJSValue *copy = new QJSValue(r);
        skillFuncCache.insert(this, copy);
        return *copy;
    }

    return QJSValue();
}

void Skill::addSubskill(Skill *subskill)
{
    subskill->m_topSkill = this;
    m_subskills << subskill;
}

void Skill::setFrequency(Frequency frequency)
{
    m_frequency = frequency;
}


TriggerSkill::TriggerSkill(const QString &name)
    : Skill(name)
{
    EventHandler::m_name = name;
    m_type = TriggerType;
    m_priority = 1;
}

EventList TriggerSkill::triggerable(GameLogic *logic, EventType event, const QObject *data, ServerPlayer *player /* = nullptr */) const
{
    const QJSValue &funcs = skillFuncs();
    if (funcs.hasProperty("triggerable") && funcs.property("triggerable").isCallable()) {
        QJSEngine *engine = funcs.engine();
        QJSValue logicValue = engine->newQObject(logic);
        QJSValue eventValue = QJSValue(static_cast<int>(event));
        QJSValue dataValue = engine->newQObject(const_cast<QObject *>(data)); // warning!! make sure the JS script won't modify this data
        QJSValue playerValue = engine->newQObject(player);
        QJSValue returnValue = funcs.property("triggerable").callWithInstance(funcs, QJSValueList() << logicValue << eventValue << dataValue << playerValue);

        if (returnValue.isObject()) {
            Event *event = qobject_cast<Event *>(returnValue.toQObject());
            if (event == nullptr || !event->isValid())
                return EventList();
            else
                return EventList() << *event;
        } else if (returnValue.isArray()) {
            EventList l;
            QJSValueIterator it(returnValue);
            while (it.hasNext()) {
                if (!it.next())
                    break;
                
                Event *event = qobject_cast<Event *>(returnValue.toQObject());
                if (event != nullptr && event->isValid())
                    l << *event;
            }
            return l;
        } // any other conditions falls into default
    }

    // default
    if (player != nullptr && player->isAlive() && player->hasSkill(this))
        return EventList() << Event(logic, this, player, player, m_frequency == Skill::Compulsory || m_frequency == Skill::Wake);

    return EventList();
}

bool TriggerSkill::onCost(GameLogic *logic, EventType event, EventPtr eventPtr, QObject *data, ServerPlayer *player) const
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

bool TriggerSkill::cost(GameLogic *logic, EventType event, EventPtr eventPtr, QObject *data, ServerPlayer *player /* = nullptr */) const
{
    const QJSValue &funcs = skillFuncs();
    if (funcs.hasProperty("cost") && funcs.property("cost").isCallable()) {
        QJSEngine *engine = funcs.engine();
        QJSValue logicValue = engine->newQObject(logic);
        QJSValue eventValue = QJSValue(static_cast<int>(event));
        QJSValue eventPtrValue = engine->newQObject(eventPtr.data());
        QJSValue dataValue = engine->newQObject(data);
        QJSValue playerValue = engine->newQObject(player);
        QJSValue returnValue = funcs.property("cost").callWithInstance(funcs, QJSValueList() << logicValue << eventValue << eventPtrValue << dataValue << playerValue);

        if (returnValue.isBool())
            return returnValue.toBool();
    }

    // default
    return true;
}

bool TriggerSkill::effect(GameLogic *logic, EventType event, const EventPtr eventPtr, QObject *data, ServerPlayer *player /* = nullptr */) const
{
    const QJSValue &funcs = skillFuncs();
    if (funcs.hasProperty("effect") && funcs.property("effect").isCallable()) {
        QJSEngine *engine = funcs.engine();
        QJSValue logicValue = engine->newQObject(logic);
        QJSValue eventValue = QJSValue(static_cast<int>(event));
        QJSValue eventPtrValue = engine->newQObject(eventPtr.data());
        QJSValue dataValue = engine->newQObject(data);
        QJSValue playerValue = engine->newQObject(player);
        QJSValue returnValue = funcs.property("effect").callWithInstance(funcs, QJSValueList() << logicValue << eventValue << eventPtrValue << dataValue << playerValue);

        if (returnValue.isBool())
            return returnValue.toBool();
    }

    //default
    return false;
}

StatusSkill::StatusSkill(const QString &name)
    : TriggerSkill(name)
{
    m_events << SkillAdded << SkillRemoved;
    setFrequency(Compulsory);
}

void StatusSkill::validate(ServerPlayer *target) const
{
    const QJSValue &funcs = skillFuncs();
    if (funcs.hasProperty("validate") && funcs.property("validate").isCallable()) {
        QJSEngine *engine = funcs.engine();
        QJSValue targetValue = engine->newQObject(target);
        funcs.property("validate").callWithInstance(funcs, QJSValueList() << targetValue);
    }

    // default: do nothing
}

void StatusSkill::invalidate(ServerPlayer *target) const
{
    const QJSValue &funcs = skillFuncs();
    if (funcs.hasProperty("invalidate") && funcs.property("invalidate").isCallable()) {
        QJSEngine *engine = funcs.engine();
        QJSValue targetValue = engine->newQObject(target);
        funcs.property("invalidate").callWithInstance(funcs, QJSValueList() << targetValue);
    }

    // default: do nothing
}

bool StatusSkill::isValid(ServerPlayer *target) const
{
    const QJSValue &funcs = skillFuncs();
    if (funcs.hasProperty("isValid")) {
        if (funcs.property("isValid").isCallable()) {
            QJSEngine *engine = funcs.engine();
            QJSValue targetValue = engine->newQObject(target);
            QJSValue returnValue = funcs.property("isValid").callWithInstance(funcs, QJSValueList() << targetValue);

            if (returnValue.isBool())
                return returnValue.toBool();
        } else if (funcs.property("isValid").isBool())
            return funcs.property("isValid").toBool();
    }
    // default
    return true;
}

bool StatusSkill::effect(GameLogic *, EventType event, EventPtr eventPtr, QObject *, ServerPlayer *player) const
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

/*
MasochismSkill::MasochismSkill(const QString &name)
    : TriggerSkill(name)
{
    m_events << AfterDamaged;
}

int MasochismSkill::triggerable(GameLogic *logic, ServerPlayer *player, const DamageValue *damage) const
{
//#error    Todo!!
    return MasochismSkill::triggerable(logic, player, damage);
}

void MasochismSkill::effect(GameLogic *logic, ServerPlayer *player, EventPtr eventPtr, const DamageValue *damage) const
{
//#error    Todo!!
    return MasochismSkill::effect(logic, player, eventPtr, damage);
}

EventList MasochismSkill::triggerable(GameLogic *logic, EventType, const QObject *data, ServerPlayer *player) const
{
    if (!(player != nullptr && player->isAlive() && player->hasSkill(this)))
        return EventList();

    const DamageValue *damage = qobject_cast<const DamageValue *>(data);
    int times = triggerable(logic, player, damage);
    if (times <= 0)
        return EventList();

    EventList l;
    for (int i = 0; i < times; ++i)
        l << Event(logic, this, player, player, m_frequency == Compulsory || m_frequency == Wake);

    return l;
}

bool MasochismSkill::effect(GameLogic *logic, EventType, EventPtr eventPtr, QObject *data, ServerPlayer *player) const
{
    DamageValue *damage = qobject_cast<DamageValue *>(data);
    effect(logic, player, eventPtr, damage);
    return false;
}
*/

ViewAsSkill::ViewAsSkill(const QString &name)
    : Skill(name)
{
    m_type = ViewAsType;
    m_subtype = ConvertType;
}

bool ViewAsSkill::isAvailable(const Player *self, const QString &pattern) const
{
    const QJSValue &funcs = skillFuncs();
    if (funcs.hasProperty("isAvailable")) {
        if (funcs.property("isAvailable").isCallable()) {
            QJSEngine *engine = funcs.engine();
            QJSValue selfValue = engine->newQObject(const_cast<Player *>(self)); // warning!! make sure the JS script won't modify this data
            QJSValue patternValue = QJSValue(pattern);
            QJSValue returnValue = funcs.property("isAvailable").callWithInstance(funcs, QJSValueList() << selfValue << patternValue);

            if (returnValue.isBool())
                return returnValue.toBool();
        } else if (funcs.property("isAvailable").isBool())
            return funcs.property("isAvailable").toBool();
    }

    // default
    return pattern.isEmpty();
}

bool ViewAsSkill::isValid(const QList<Card *> &cards, const Player *self, const QString &pattern) const
{
    QList<const Card *> selected;
    foreach (const Card *toSelect, cards) {
        if (viewFilter(selected, toSelect, self, pattern))
            selected << toSelect;
        else
            return false;
    }

    return true;
}

bool ViewAsSkill::viewFilter(const QList<const Card *> &selected, const Card *card, const Player *self, const QString &pattern) const
{
    const QJSValue &funcs = skillFuncs();
    if (funcs.hasProperty("viewFilter")) {
        if (funcs.property("viewFilter").isCallable()) {
            QJSEngine *engine = funcs.engine();
            QJSValue selectedValue = engine->newArray(selected.length());
            for (int i = 0; i < selected.length(); ++i)
                selectedValue.setProperty(i, engine->newQObject(const_cast<Card *>(selected.value(i))));
            QJSValue cardValue = engine->newQObject(const_cast<Card *>(card));
            QJSValue selfValue = engine->newQObject(const_cast<Player *>(self));
            QJSValue patternValue = QJSValue(pattern);
            QJSValue returnValue = funcs.property("viewFilter").callWithInstance(funcs, QJSValueList() << selectedValue << cardValue << selfValue << patternValue);

            if (returnValue.isBool())
                return returnValue.toBool();
        } else if (funcs.property("viewFilter").isBool())
            return funcs.property("viewFilter").toBool();
    }

    // default
    return false;
}

Card *ViewAsSkill::viewAs(const QList<Card *> &cards, const Player *self) const
{
    const QJSValue &funcs = skillFuncs();
    if (funcs.hasProperty("viewAs") && funcs.property("viewAs").isCallable()) {
        QJSEngine *engine = funcs.engine();
        QJSValue cardsValue = engine->newArray(cards.length());
        for (int i = 0; i < cards.length(); ++i)
            cardsValue.setProperty(i, engine->newQObject(const_cast<Card *>(cards.value(i))));
        QJSValue selfValue = engine->newQObject(const_cast<Player *>(self));
        QJSValue returnValue = funcs.property("viewAs").callWithInstance(funcs, QJSValueList() << cardsValue << selfValue);

        if (returnValue.isQObject()) {
            Card *card = qobject_cast<Card *>(returnValue.toQObject());
            if (card != nullptr)
                return card;
        }
    }
    
    // default
    return nullptr;
}

/*
OneCardViewAsSkill::OneCardViewAsSkill(const QString &name)
    : ViewAsSkill(name)
{

}

bool OneCardViewAsSkill::viewFilter(const QList<const Card *> &selected, const Card *card, const Player *self, const QString &pattern) const
{
    return selected.isEmpty() && viewFilter(card, self, pattern);
}

bool OneCardViewAsSkill::viewFilter(const Card *card, const Player *self, const QString &pattern) const
{
//#error    Todo!!
    return OneCardViewAsSkill::viewFilter(card, self, pattern);
}

Card *OneCardViewAsSkill::viewAs(const QList<Card *> &cards, const Player *self) const
{
    if (cards.length() == 1)
        return viewAs(cards.first(), self);
    return nullptr;
}

Card *OneCardViewAsSkill::viewAs(Card *card, const Player *self) const
{
//#error    Todo!!
    return OneCardViewAsSkill::viewAs(card, self);
}
*/

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

bool ProactiveSkill::cardFeasible(const QList<const Card *> &selected, const Player *source) const
{
    const QJSValue &funcs = skillFuncs();
    if (funcs.hasProperty("cardFeasible")) {
        if (funcs.property("cardFeasible").isCallable()) {
            QJSEngine *engine = funcs.engine();
            QJSValue selectedValue = engine->newArray(selected.length());
            for (int i = 0; i < selected.length(); ++i)
                selectedValue.setProperty(i, engine->newQObject(const_cast<Card *>(selected.value(i))));
            QJSValue sourceValue = engine->newQObject(const_cast<Player *>(source));
            QJSValue returnValue = funcs.property("cardFeasible").callWithInstance(funcs, QJSValueList() << selectedValue << sourceValue);

            if (returnValue.isBool())
                return returnValue.toBool();
        } else if (funcs.property("cardFeasible").isBool())
            return funcs.property("cardFeasible").toBool();
    }

    // default
    return true;
}

bool ProactiveSkill::cardFilter(const QList<const Card *> &selected, const Card *card, const Player *source, const QString &pattern) const
{
    const QJSValue &funcs = skillFuncs();
    if (funcs.hasProperty("cardFilter")) {
        if (funcs.property("cardFilter").isCallable()) {
            QJSEngine *engine = funcs.engine();
            QJSValue selectedValue = engine->newArray(selected.length());
            for (int i = 0; i < selected.length(); ++i)
                selectedValue.setProperty(i, engine->newQObject(const_cast<Card *>(selected.value(i))));
            QJSValue cardValue = engine->newQObject(const_cast<Card *>(card));
            QJSValue sourceValue = engine->newQObject(const_cast<Player *>(source));
            QJSValue patternValue = QJSValue(pattern);
            QJSValue returnValue = funcs.property("cardFilter").callWithInstance(funcs, QJSValueList() << selectedValue << cardValue << sourceValue << patternValue);

            if (returnValue.isBool())
                return returnValue.toBool();
        } else if (funcs.property("cardFilter").isBool())
            return funcs.property("cardFilter").toBool();
    }

    // default
    return false;
}

bool ProactiveSkill::isValid(const QList<Card *> &cards, const Player *source, const QString &pattern) const
{
    QList<const Card *> selected;
    foreach (const Card *toSelect, cards) {
        if (cardFilter(selected, toSelect, source, pattern))
            selected << toSelect;
        else
            return false;
    }
    return cardFeasible(selected, source);
}

bool ProactiveSkill::playerFeasible(const QList<const Player *> &selected, const Player *source) const
{
    const QJSValue &funcs = skillFuncs();
    if (funcs.hasProperty("playerFeasible")) {
        if (funcs.property("playerFeasible").isCallable()) {
            QJSEngine *engine = funcs.engine();
            QJSValue selectedValue = engine->newArray(selected.length());
            for (int i = 0; i < selected.length(); ++i)
                selectedValue.setProperty(i, engine->newQObject(const_cast<Player *>(selected.value(i))));
            QJSValue sourceValue = engine->newQObject(const_cast<Player *>(source));
            QJSValue returnValue = funcs.property("playerFeasible").callWithInstance(funcs, QJSValueList() << selectedValue << sourceValue);

            if (returnValue.isBool())
                return returnValue.toBool();
        } else if (funcs.property("playerFeasible").isBool())
            return funcs.property("playerFeasible").toBool();
    }

    // default
    return true;
}

bool ProactiveSkill::playerFilter(const QList<const Player *> &selected, const Player *toSelect, const Player *source) const
{
    const QJSValue &funcs = skillFuncs();
    if (funcs.hasProperty("playerFilter")) {
        if (funcs.property("playerFilter").isCallable()) {
            QJSEngine *engine = funcs.engine();
            QJSValue selectedValue = engine->newArray(selected.length());
            for (int i = 0; i < selected.length(); ++i)
                selectedValue.setProperty(i, engine->newQObject(const_cast<Player *>(selected.value(i))));
            QJSValue toSelectValue = engine->newQObject(const_cast<Player *>(toSelect));
            QJSValue sourceValue = engine->newQObject(const_cast<Player *>(source));
            QJSValue returnValue = funcs.property("playerFilter").callWithInstance(funcs, QJSValueList() << selectedValue << toSelectValue << sourceValue);

            if (returnValue.isBool())
                return returnValue.toBool();
        } else if (funcs.property("playerFilter").isBool())
            return funcs.property("cost").toBool();
    }

    // default
    return false;
}

bool ProactiveSkill::isValid(const QList<ServerPlayer *> &targets, ServerPlayer *source) const
{
    QList<const Player *> selected;
    foreach (ServerPlayer *target, targets)
        selected << target;
    return isValid(selected, source);
}

bool ProactiveSkill::isValid(const QList<const Player *> &targets, const Player *source) const
{
    QList<const Player *> selected;
    foreach (const Player *toSelect, targets) {
        if (playerFilter(selected, toSelect, source))
            selected << toSelect;
        else
            return false;
    }

    return playerFeasible(targets, source);
}

bool ProactiveSkill::cost(GameLogic *logic, ServerPlayer *from, const QList<ServerPlayer *> &to, const QList<Card *> &cards) const
{
    const QJSValue &funcs = skillFuncs();
    if (funcs.hasProperty("cost")) {
        if (funcs.property("cost").isCallable()) {
            QJSEngine *engine = funcs.engine();
            QJSValue logicValue = engine->newQObject(logic);
            QJSValue fromValue = engine->newQObject(from);
            QJSValue toValue = engine->newArray(to.length());
            for (int i = 0; i < to.length(); ++i)
                toValue.setProperty(i, engine->newQObject(to.value(i)));
            QJSValue cardsValue = engine->newArray(cards.length());
            for (int i = 0; i < cards.length(); ++i)
                cardsValue.setProperty(i, engine->newQObject(cards.value(i)));
            QJSValue returnValue = funcs.property("cost").callWithInstance(funcs, QJSValueList() << logicValue << fromValue << toValue << cardsValue);

            if (returnValue.isBool())
                return returnValue.toBool();
        } else if (funcs.property("cost").isBool())
            return funcs.property("cost").toBool();
    }

    // default
    return false;
}

void ProactiveSkill::effect(GameLogic *logic, ServerPlayer *from, const QList<ServerPlayer *> &to, const QList<Card *> &cards) const
{
    const QJSValue &funcs = skillFuncs();
    if (funcs.hasProperty("effect") && funcs.property("effect").isCallable()) {
        QJSEngine *engine = funcs.engine();
        QJSValue logicValue = engine->newQObject(logic);
        QJSValue fromValue = engine->newQObject(from);
        QJSValue toValue = engine->newArray(to.length());
        for (int i = 0; i < to.length(); ++i)
            toValue.setProperty(i, engine->newQObject(to.value(i)));
        QJSValue cardsValue = engine->newArray(cards.length());
        for (int i = 0; i < cards.length(); ++i)
            cardsValue.setProperty(i, engine->newQObject(cards.value(i)));
        funcs.property("effect").callWithInstance(funcs, QJSValueList() << logicValue << fromValue << toValue << cardsValue);
    }
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

/*
class RetrialSkill::Timing : public TriggerSkill
{
public:
    Timing(const QString &skillName, const RetrialSkill *parent)
        : TriggerSkill(skillName), m_parent(parent)
    {
        m_events << AskForRetrial;
    }

    EventList triggerable(GameLogic *logic, EventType, const QObject *, ServerPlayer * / * = nullptr * /) const final override
    {
        // player is nullptr in this method so we don't use this player
        EventList l;
        foreach (ServerPlayer *p, logic->allPlayers()) {
            if (p->hasSkill(m_parent) && m_parent->isAvailable(p, "@" + name()))
                l << Event(logic, this, p, p);
        }
        return l;
    }

    bool cost(GameLogic *logic, EventType, EventPtr eventPtr, QObject *data, ServerPlayer * / * = nullptr * /) const final override
    {
        JudgeValue *judge = qobject_cast<JudgeValue *>(data);
        eventPtr->invoker->showPrompt(name() + "_ask_for_retrial", judge->who);
        SkillInvokeValue s = eventPtr->invoker->askToInvokeSkill(m_parent);
        if (s.skill == m_parent && s.cards.length() == 1) {
            logic->retrialCost(*judge, s.cards.first(), m_parent->m_isReplace);
            Card *card = s.cards.first();
            eventPtr->invoker->tag[name() + "_retrialCard"] = QVariant::fromValue(card);

            return true;
        }

        return false;
    }

    bool effect(GameLogic *logic, EventType, const EventPtr eventPtr, QObject *data, ServerPlayer * / * = nullptr * /) const final override
    {
        JudgeValue *judge = qobject_cast<JudgeValue *>(data);
        Card *card = eventPtr->invoker->tag.value(name() + "_retrialCard").value<Card *>();
        logic->retrialEffect(*judge, card);

        return false;
    }

private:
    const RetrialSkill *m_parent;
};

RetrialSkill::RetrialSkill(const QString &name, bool replace / *= false* /)
    : ProactiveSkill(name), m_isReplace(replace)
{
    Timing *timing = new Timing(name, this);
    addSubskill(timing);
}

ProactiveSkillTiming::ProactiveSkillTiming(const QString &name, const ProactiveSkill *parent)
    : TriggerSkill(name), m_parent(parent)
{

}

bool ProactiveSkillTiming::cost(GameLogic *logic, EventType, EventPtr eventPtr, QObject *, ServerPlayer * / * = nullptr * /) const
{
    SkillInvokeValue *s = new SkillInvokeValue(eventPtr->invoker->askToInvokeSkill(m_parent));
    const ProactiveSkill *proactiveSkill = dynamic_cast<const ProactiveSkill *>(s->skill);
    if (proactiveSkill != nullptr && proactiveSkill->cost(logic, s->player, s->targets, s->cards)) {
        eventPtr->tag["invoke"] = QVariant::fromValue(s);
        return true;
    }

    return false;
}

bool ProactiveSkillTiming::effect(GameLogic *logic, EventType, EventPtr eventPtr, QObject *, ServerPlayer * / * = nullptr * /) const
{
    SkillInvokeValue *s = eventPtr->tag.value("invoke").value<SkillInvokeValue *>();
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
*/

CardModSkill::CardModSkill(const QString &name)
    : Skill(name)
{
    m_type = CardModType;
}

bool CardModSkill::targetFilter(const Card *card, const QList<const Player *> &selected, const Player *toSelect, const Player *source) const
{
    const QJSValue &funcs = skillFuncs();
    if (funcs.hasProperty("targetFilter")) {
        if (funcs.property("targetFilter").isCallable()) {
            QJSEngine *engine = funcs.engine();
            QJSValue cardValue = engine->newQObject(const_cast<Card *>(card));
            QJSValue selectedValue = engine->newArray(selected.length());
            for (int i = 0; i < selected.length(); ++i)
                selectedValue.setProperty(i, engine->newQObject(const_cast<Player *>(selected.value(i))));
            QJSValue toSelectValue = engine->newQObject(const_cast<Player *>(toSelect));
            QJSValue sourceValue = engine->newQObject(const_cast<Player *>(source));
            QJSValue returnValue = funcs.property("targetFilter").callWithInstance(funcs, QJSValueList() << cardValue << selectedValue << toSelectValue << sourceValue);

            if (returnValue.isBool())
                return returnValue.toBool();
        } else if (funcs.property("targetFilter").isBool())
            return funcs.property("targetFilter").toBool();
    }

    return true;
}

int CardModSkill::extraDistanceLimit(const Card *card, const QList<const Player *> &selected, const Player *toSelect, const Player *source) const
{
    const QJSValue &funcs = skillFuncs();
    if (funcs.hasProperty("extraDistanceLimit")) {
        if (funcs.property("extraDistanceLimit").isCallable()) {
            QJSEngine *engine = funcs.engine();
            QJSValue cardValue = engine->newQObject(const_cast<Card *>(card));
            QJSValue selectedValue = engine->newArray(selected.length());
            for (int i = 0; i < selected.length(); ++i)
                selectedValue.setProperty(i, engine->newQObject(const_cast<Player *>(selected.value(i))));
            QJSValue toSelectValue = engine->newQObject(const_cast<Player *>(toSelect));
            QJSValue sourceValue = engine->newQObject(const_cast<Player *>(source));
            QJSValue returnValue = funcs.property("extraDistanceLimit").callWithInstance(funcs, QJSValueList() << cardValue << selectedValue << toSelectValue << sourceValue);

            if (returnValue.isNumber())
                return returnValue.toInt();
        } else if (funcs.property("extraDistanceLimit").isNumber())
            return funcs.property("extraDistanceLimit").toInt();
    }

    return 0;
}

int CardModSkill::extraMaxTargetNum(const Card *card, const QList<const Player *> &selected, const Player *toSelect, const Player *source) const
{
    const QJSValue &funcs = skillFuncs();
    if (funcs.hasProperty("extraMaxTargetNum")) {
        if (funcs.property("extraMaxTargetNum").isCallable()) {
            QJSEngine *engine = funcs.engine();
            QJSValue cardValue = engine->newQObject(const_cast<Card *>(card));
            QJSValue selectedValue = engine->newArray(selected.length());
            for (int i = 0; i < selected.length(); ++i)
                selectedValue.setProperty(i, engine->newQObject(const_cast<Player *>(selected.value(i))));
            QJSValue toSelectValue = engine->newQObject(const_cast<Player *>(toSelect));
            QJSValue sourceValue = engine->newQObject(const_cast<Player *>(source));
            QJSValue returnValue = funcs.property("extraMaxTargetNum").callWithInstance(funcs, QJSValueList() << cardValue << selectedValue << toSelectValue << sourceValue);

            if (returnValue.isNumber())
                return returnValue.toInt();
        } else if (funcs.property("extraMaxTargetNum").isNumber())
            return funcs.property("extraMaxTargetNum").toInt();
    }

    return 0;
}

int CardModSkill::extraUseNum(const Card *card, const Player *player) const
{
    const QJSValue &funcs = skillFuncs();
    if (funcs.hasProperty("extraUseNum")) {
        if (funcs.property("extraUseNum").isCallable()) {
            QJSEngine *engine = funcs.engine();
            QJSValue cardValue = engine->newQObject(const_cast<Card *>(card));
            QJSValue playerValue = engine->newQObject(const_cast<Player *>(player));
            QJSValue returnValue = funcs.property("extraUseNum").callWithInstance(funcs, QJSValueList() << cardValue << playerValue);

            if (returnValue.isNumber())
                return returnValue.toInt();
        } else if (funcs.property("extraUseNum").isNumber())
            return funcs.property("extraUseNum").toInt();
    }

    return 0;
}
