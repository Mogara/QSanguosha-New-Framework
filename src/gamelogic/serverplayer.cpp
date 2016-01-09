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
#include "cardpattern.h"
#include "gamelogic.h"
#include "protocol.h"
#include "serverplayer.h"
#include "skill.h"

#include <croom.h>
#include <cserveragent.h>

ServerPlayer::ServerPlayer(GameLogic *logic, CServerAgent *agent)
    : Player(logic)
    , m_logic(logic)
    , m_room(logic->room())
    , m_agent(agent)
{
}

ServerPlayer::~ServerPlayer()
{
}

CServerAgent *ServerPlayer::agent() const
{
    return m_agent;
}

void ServerPlayer::setAgent(CServerAgent *agent)
{
    m_agent = agent;
}

CRoom *ServerPlayer::room() const
{
    if (m_room->isAbandoned())
        throw GameFinish;
    return m_room;
}

void ServerPlayer::drawCards(int n)
{
    CardsMoveStruct move;
    move.from.type = CardArea::DrawPile;
    move.from.direction = CardArea::Top;
    move.to.type = CardArea::Hand;
    move.to.owner = this;
    move.cards = m_logic->getDrawPileCards(n);

    m_logic->moveCards(move);
}

void ServerPlayer::recastCard(Card *card)
{
    CardsMoveStruct recast;
    recast.cards << card;
    recast.to.type = CardArea::Table;
    recast.isOpen = true;
    m_logic->moveCards(recast);

    const CardArea *table = m_logic->table();
    if (table->contains(card)) {
        CardsMoveStruct discard;
        discard.cards << card;
        discard.to.type = CardArea::DiscardPile;
        discard.isOpen = true;
        m_logic->moveCards(discard);
    }

    drawCards(1);
}

void ServerPlayer::showCard(Card *card)
{
    QVariantList cardData;
    cardData << card->id();

    QVariantMap data;
    data["from"] = id();
    data["cards"] = cardData;
    m_room->broadcastNotification(S_COMMAND_SHOW_CARD, data);
}

void ServerPlayer::showCards(const QList<Card *> &cards)
{
    QVariantList cardData;
    foreach (Card *card, cards)
        cardData << card->id();

    QVariantMap data;
    data["from"] = id();
    data["cards"] = cardData;
    m_agent->notify(S_COMMAND_SHOW_CARD, data);
}

void ServerPlayer::play()
{
    QList<Phase> phases;
    phases << RoundStart
           << Start
           << Judge
           << Draw
           << Play
           << Discard
           << Finish;
    play(phases);
}

void ServerPlayer::play(const QList<Player::Phase> &phases)
{
    PhaseChangeStruct change;
    foreach (Phase to, phases) {
        if (to == Inactive)
            break;
        change.from = phase();
        change.to = to;

        QVariant data = QVariant::fromValue(&change);
        bool skip = m_logic->trigger(PhaseChanging, this, data);

        setPhase(change.to);
        broadcastProperty("phase");

        if ((skip || isPhaseSkipped(change.to)) && !m_logic->trigger(PhaseSkipping, this, data))
            continue;

        if (!m_logic->trigger(PhaseStart, this))
            m_logic->trigger(PhaseProceeding, this);
        m_logic->trigger(PhaseEnd, this);
    }

    change.from = phase();
    change.to = Inactive;

    QVariant data = QVariant::fromValue(&change);
    m_logic->trigger(PhaseChanging, this, data);

    setPhase(change.to);
    broadcastProperty("phase");

    clearSkippedPhase();
}

bool ServerPlayer::activate()
{
    int timeout = 15 * 1000;
    m_agent->request(S_COMMAND_USE_CARD, QVariant(), timeout);
    QVariant replyData = m_agent->waitForReply(timeout);
    if (replyData.isNull())
        return true;
    const QVariantMap reply = replyData.toMap();
    if (reply.isEmpty())
        return true;

    QList<ServerPlayer *> targets;
    QVariantList tos = reply["to"].toList();
    foreach (const QVariant &to, tos) {
        uint toId = to.toUInt();
        ServerPlayer *target = m_logic->findPlayer(toId);
        if (target)
            targets << target;
    }

    QList<Card *> cards = m_logic->findCards(reply["cards"]);

    const Skill *skill = nullptr;
    uint skillId = reply["skillId"].toUInt();
    if (skillId)
        skill = getSkill(skillId);

    Card *card = nullptr;
    if (skill) {
        if (skill->type() == Skill::ViewAsType) {
            if (skill->subtype() == ViewAsSkill::ProactiveType) {
                const ProactiveSkill *proactiveSkill = static_cast<const ProactiveSkill *>(skill);
                proactiveSkill->effect(m_logic, this, targets, cards);
                return false;
            } else if (skill->subtype() == ViewAsSkill::ConvertType) {
                const ViewAsSkill *viewAsSkill = static_cast<const ViewAsSkill *>(skill);
                card = viewAsSkill->viewAs(cards, this);
            }
        }
    } else {
        card = cards.length() > 0 ? cards.first() : nullptr;
    }

    if (card != nullptr) {
        if (card->canRecast() && targets.isEmpty()) {
            recastCard(card);
        } else {
            CardUseStruct use;
            use.from = this;
            use.to = targets;
            use.card = card;
            m_logic->useCard(use);
        }
        return false;
    } else {
        return true;
    }
}

void ServerPlayer::showPrompt(const QString &message, int number)
{
    QVariantList data;
    data << message;
    data << number;
    m_agent->notify(S_COMMAND_SHOW_PROMPT, data);
}

void ServerPlayer::showPrompt(const QString &message, const QVariantList &args)
{
    QVariantList data;
    data << message;
    data << args;
    m_agent->notify(S_COMMAND_SHOW_PROMPT, data);
}

void ServerPlayer::showPrompt(const QString &message, const Card *card)
{
    QVariantList args;
    args << "card" << card->id();
    showPrompt(message, args);
}

void ServerPlayer::showPrompt(const QString &message, const ServerPlayer *from, const Card *card)
{
    QVariantList args;
    args << "player" << from->id();
    if (card)
        args << "card" << card->id();
    showPrompt(message, args);
}

void ServerPlayer::showPrompt(const QString &message, const ServerPlayer *p1, const ServerPlayer *p2, const Card *card)
{
    QVariantList args;
    args << "player" << p1->id();
    args << "player" << p2->id();
    if (card)
        args << "card" << card->id();
    showPrompt(message, args);
}

Event ServerPlayer::askForTriggerOrder(const QString &reason, const EventList &options, bool cancelable)
{
    QVariantMap data;
    data["reason"] = reason;
    data["cancelable"] = cancelable;

    QVariantList optionData;
    foreach (const Event &e, options) {
        QVariantMap eventData;
        eventData["name"] = e.handler->name();
        QVariantList targetData;
        foreach (ServerPlayer *to, e.to)
            targetData << to->id();
        eventData["to"] = targetData;
        optionData << eventData;
    }
    data["options"] = optionData;

    m_agent->request(S_COMMAND_TRIGGER_ORDER, data, 15000);
    QVariant replyData = m_agent->waitForReply(15000);
    if (replyData.isNull())
        return cancelable ? Event() : options.first();

    int eventId = replyData.toInt();
    if (eventId >= 0 && eventId < options.length())
        return options.at(eventId);

    return cancelable ? Event() : options.first();
}

Card *ServerPlayer::askForCard(const QString &pattern, bool optional)
{
    QVariantMap data;
    data["pattern"] = pattern;
    data["optional"] = optional;

    QVariant replyData;
    forever {
        m_agent->request(S_COMMAND_ASK_FOR_CARD, data, 15000);
        replyData = m_agent->waitForReply(15000);
        if (replyData.isNull())
            break;

        const QVariantMap reply = replyData.toMap();
        QList<Card *> cards = m_logic->findCards(reply["cards"]);
        uint skillId = reply["skillId"].toUInt();
        if (skillId) {
            const Skill *skill = getSkill(skillId);
            if (skill->type() == Skill::ViewAsType) {
                const ViewAsSkill *viewAsSkill = static_cast<const ViewAsSkill *>(skill);
                return viewAsSkill->viewAs(cards, this);
            }
        }
        if (cards.length() != 1)
            break;
        return cards.first();
    }

    if (!optional) {
        CardPattern p(pattern);

        QList<Card *> allCards = handcardArea()->cards();
        foreach (Card *card, allCards) {
            if (p.match(this, card))
                return card;
        }

        allCards = equipArea()->cards();
        foreach (Card *card, allCards) {
            if (p.match(this, card))
                return card;
        }
    }

    return nullptr;
}

QList<Card *> ServerPlayer::askForCards(const QString &pattern, int num, bool optional)
{
    return askForCards(pattern, num, num, optional);
}

QList<Card *> ServerPlayer::askForCards(const QString &pattern, int minNum, int maxNum, bool optional)
{
    if (maxNum < minNum)
        maxNum = minNum;

    QVariantMap data;
    data["pattern"] = pattern;
    data["minNum"] = minNum;
    data["maxNum"] = maxNum;
    data["optional"] = optional;

    m_agent->request(S_COMMAND_ASK_FOR_CARD, data, 15000);
    const QVariantMap replyData = m_agent->waitForReply(15000).toMap();

    if (optional) {
        if (replyData.isEmpty())
            return QList<Card *>();
        return m_logic->findCards(replyData["cards"]);
    } else {
        QList<Card *> cards = m_logic->findCards(replyData["cards"]);
        if (!optional) {
            if (cards.length() < minNum) {
                QList<Card *> allCards = handcardArea()->cards() + equipArea()->cards();
                CardPattern p(pattern);
                foreach (Card *card, allCards) {
                    if (!cards.contains(card) && p.match(this, card)) {
                        cards << card;
                        if (cards.length() >= minNum)
                            break;
                    }
                }
            } else if (cards.length() > maxNum) {
                cards = cards.mid(0, maxNum);
            }
        }
        return cards;
    }
}

Card *ServerPlayer::askToChooseCard(ServerPlayer *owner, const QString &areaFlag, bool handcardVisible)
{
    const CardArea *handcards = owner->handcardArea();
    const CardArea *equips = owner->equipArea();
    const CardArea *delayedTricks = owner->delayedTrickArea();

    QVariantMap data;

    if (areaFlag.contains('h')) {
        QVariantList handcardData;
        if (handcardVisible) {
            QList<Card *> cards = handcards->cards();
            foreach (const Card *card, cards)
                handcardData << card->id();
            data["handcards"] = handcardData;
        } else {
            data["handcards"] = owner->handcardNum();
        }
    }

    if (areaFlag.contains('e')) {
        QVariantList equipData;
        QList<Card *> cards = equips->cards();
        foreach (const Card *card, cards)
            equipData << card->id();
        data["equips"] = equipData;
    }

    if (areaFlag.contains('j')) {
        QVariantList trickData;
        QList<Card *> cards = delayedTricks->cards();
        foreach (const Card *card, cards)
            trickData << card->id();
        data["delayedTricks"] = trickData;
    }

    m_agent->request(S_COMMAND_CHOOSE_PLAYER_CARD, data, 15000);
    uint cardId = m_agent->waitForReply(15000).toUInt();
    if (cardId > 0) {
        if (areaFlag.contains('h') && handcardVisible) {
            Card *card = handcards->findCard(cardId);
            if (card)
                return card;
        }
        if (areaFlag.contains('e')) {
            Card *card = equips->findCard(cardId);
            if (card)
                return card;
        }
        if (areaFlag.contains('j')) {
            Card *card = delayedTricks->findCard(cardId);
            if (card)
                return card;
        }
    }

    if (areaFlag.contains('h') && handcards->length() > 0)
        return handcards->rand();

    if (areaFlag.contains('e') && equips->length() > 0)
        return equips->rand();

    if (areaFlag.contains('j') && delayedTricks->length() > 0)
        return delayedTricks->rand();

    return nullptr;
}

Card *ServerPlayer::askToUseCard(const QString &pattern, const QList<ServerPlayer *> &assignedTargets)
{
    QVariantMap data;
    data["pattern"] = pattern;

    QVariantList targetIds;
    foreach (ServerPlayer *target, assignedTargets)
        targetIds << target->id();
    data["assignedTargets"] = targetIds;

    m_agent->request(S_COMMAND_USE_CARD, data, 15000);
    const QVariantMap reply = m_agent->waitForReply(15000).toMap();
    if (reply.isEmpty())
        return nullptr;

    uint cardId = reply["cardId"].toUInt();
    Card *card = m_logic->findCard(cardId);
    if (card == nullptr)
        return nullptr;

    CardUseStruct use;
    use.from = this;
    use.card = card;

    QVariantList targets = reply["to"].toList();
    foreach (const QVariant &target, targets) {
        ServerPlayer *player = m_logic->findPlayer(target.toUInt());
        if (player)
            use.to << player;
    }
    foreach (ServerPlayer *target, assignedTargets) {
        if (!use.to.contains(target))
            return nullptr;
    }
    m_logic->useCard(use);
    return card;
}

void ServerPlayer::broadcastProperty(const char *name) const
{
    QVariantList data;
    data << id();
    data << name;
    data << property(name);
    m_room->broadcastNotification(S_COMMAND_UPDATE_PLAYER_PROPERTY, data);
}

void ServerPlayer::broadcastProperty(const char *name, const QVariant &value, ServerPlayer *except) const
{
    QVariantList data;
    data << id();
    data << name;
    data << value;
    m_room->broadcastNotification(S_COMMAND_UPDATE_PLAYER_PROPERTY, data, except->agent());
}

void ServerPlayer::unicastPropertyTo(const char *name, ServerPlayer *player)
{
    QVariantList data;
    data << id();
    data << name;
    data << property(name);
    CServerAgent *agent = player->agent();
    if (agent)
        agent->notify(S_COMMAND_UPDATE_PLAYER_PROPERTY, data);
}

void ServerPlayer::addCardHistory(const QString &name, int times)
{
    Player::addCardHistory(name, times);
    QVariantList data;
    data << name;
    data << times;

    m_agent->notify(S_COMMAND_ADD_CARD_HISTORY, data);
}

void ServerPlayer::clearCardHistory()
{
    Player::clearCardHistory();
    m_agent->notify(S_COMMAND_ADD_CARD_HISTORY);
}

void ServerPlayer::addHeadSkill(const Skill *skill)
{
    Player::addHeadSkill(skill);
    addTriggerSkill(skill);

    QVariantMap data;
    data["playerId"] = id();
    data["skillId"] = skill->id();
    data["position"] = "head";
    m_room->broadcastNotification(S_COMMAND_ADD_SKILL, data);
}

void ServerPlayer::addDeputySkill(const Skill *skill)
{
    Player::addDeputySkill(skill);
    addTriggerSkill(skill);

    QVariantMap data;
    data["playerId"] = id();
    data["skillId"] = skill->id();
    data["position"] = "deputy";
    m_room->broadcastNotification(S_COMMAND_ADD_SKILL, data);
}

void ServerPlayer::addAcquiredSkill(const Skill *skill)
{
    Player::addAcquiredSkill(skill);
    addTriggerSkill(skill);

    QVariantMap data;
    data["playerId"] = id();
    data["skillId"] = skill->id();
    data["position"] = "acquired";
    m_room->broadcastNotification(S_COMMAND_ADD_SKILL, data);
}

void ServerPlayer::addTriggerSkill(const Skill *skill)
{
    if (skill->type() == Skill::TriggerType)
        m_logic->addEventHandler(static_cast<const TriggerSkill *>(skill));

    QList<const Skill *> subskills = skill->subskills();
    foreach (const Skill *subskill, subskills) {
        if (subskill->type() == Skill::TriggerType)
            m_logic->addEventHandler(static_cast<const TriggerSkill *>(subskill));
    }
}
