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
#include "engine.h"
#include "eventhandler.h"
#include "gamelogic.h"
#include "gamemode.h"
#include "gamerule.h"
#include "general.h"
#include "package.h"
#include "protocol.h"
#include "roomsettings.h"
#include "serverplayer.h"
#include "util.h"

#include <CRoom>
#include <CServer>
#include <CServerRobot>
#include <CServerUser>

#include <QDateTime>
#include <QThread>
#include "skill.h"

GameLogic::GameLogic(CRoom *parent)
    : CAbstractGameLogic(parent)
    , m_currentPlayer(nullptr)
    , m_gameRule(nullptr)
    , m_skipGameRule(false)
    , m_round(0)
    , m_reshufflingCount(0)
{
    m_drawPile = new CardArea(CardArea::DrawPile);
    m_discardPile = new CardArea(CardArea::DiscardPile);
    m_table = new CardArea(CardArea::Table);
    m_table->setIsVirtualCardArea(true);
    m_wugu = new CardArea(CardArea::Wugu);
}

GameLogic::~GameLogic()
{
    delete m_wugu;
    delete m_table;
    delete m_discardPile;
    delete m_drawPile;

    foreach (Card *card, m_cards)
        delete card;
}

void GameLogic::setGameRule(const GameRule *rule) {
    if (m_gameRule) {
        foreach (EventType e, m_gameRule->events()) {
            if (m_handlers[e].contains(m_gameRule))
                m_handlers[e].removeOne(m_gameRule);
        }
    }

    m_gameRule = rule;
    if (rule) {
        foreach (EventType e, rule->events())
            m_handlers[e].append(m_gameRule);
    }
}

void GameLogic::addEventHandler(const EventHandler *handler)
{
    QSet<EventType> events = handler->events();
    foreach(EventType event, events) {
        if (!m_handlers[event].contains(handler))
            m_handlers[event] << handler;
    }
}

void GameLogic::removeEventHandler(const EventHandler *handler)
{
    QSet<EventType> events = handler->events();
    foreach (EventType event, events)
        m_handlers[event].removeOne(handler);
}


void GameLogic::getEventHandlersAndSort(EventType event, EventPtrList &detailsList, const EventPtrList &triggered, const QObject *data, ServerPlayer *player /*= nullptr*/)
{
    // used to get all the event handlers which can be triggered now, and sort them.
    // everytime this function is called, it will get all the event handlers and judge the triggerable one by one
    QList<const EventHandler *> skillList = m_handlers[event];
    QList<QSharedPointer<Event> > details; // We create a new list everytime this function is called
    foreach (const EventHandler *eh, skillList) {
        // judge every Event Handler
        EventList triggerable = eh->triggerable(this, event, data, player);

        QMutableListIterator<Event> it_triggerable(triggerable);
        while (it_triggerable.hasNext()) {
            const Event &t = it_triggerable.next();
            if (!t.isValid())
                it_triggerable.remove();  // remove the invalid item from the list
        }

        if (triggerable.isEmpty()) // i.e. there is no valid item returned from the skill's triggerable
            continue;

        EventPtrList r; // we create a list for every skill
        foreach (const Event &t, triggerable) {
            // we copy construct a new Event in the heap area (because the return value from triggerable is in the stack). use a shared pointer to point to it, and add it to the new list.
            // because the shared pointer will destroy the item it point to when all the instances of the pointer is destroyed, so there is no memory leak.
            EventPtr ptr(new Event(t));
            r << ptr;
        }
        if (r.length() == 1) {
            // if the skill has only one instance of the invokedetail, we copy the tag to the old instance(overwrite the old ones), and use the old instance, delete the new one
            foreach (const EventPtr &detail, (detailsList + triggered).toSet()) {
                if (detail->sameSkill(*r.first())) {
                    foreach (const QString &key, r.first()->tag.keys())
                        detail->tag[key] = r.first()->tag.value(key);
                    r.clear();
                    r << detail;
                    break;
                }
            }
        } else {
            bool isPreferredTargetSkill = false;
            EventPtrList s;
            // judge whether this skill in this event is a preferred-target skill, make a invoke list as s
            foreach (const EventPtr &detail, (detailsList + triggered).toSet()) {
                if (detail->eh == r.first()->eh) {
                    s << detail;
                    if (detail->preferredTarget != nullptr)
                        isPreferredTargetSkill = true;
                }
            }
            if (!isPreferredTargetSkill) {
                std::sort(s.begin(), s.end(), [](const EventPtr &a1, const EventPtr &a2) { return a1->triggered && !a2->triggered; });
                // because these are of one single skill, so we can pick the invoke list using a trick like this
                s.append(r);
                r = s.mid(0, r.length());
            } else {
                // do a stable sort to r and s since we should judge the trigger order
                static std::function<bool(const EventPtr &, const EventPtr &)> preferredTargetLess =
                    [](const EventPtr &a1, const EventPtr &a2) {
                        return a1->preferredTargetLess(*a2);
                    };

                std::stable_sort(r.begin(), r.end(), preferredTargetLess);
                std::stable_sort(s.begin(), s.end(), preferredTargetLess);

                {
                    QListIterator<EventPtr> r_it(r);
                    QMutableListIterator<EventPtr> s_it(s);
                    while (r_it.hasNext() && s_it.hasNext()) {
                        QSharedPointer<Event> r_now = r_it.next();
                        QSharedPointer<Event> s_now = s_it.next();

                        if (r_now->preferredTarget == s_now->preferredTarget)
                            continue;

                        // e.g. let r =  a b c d e f   h     k
                        //      let s =  a b   d e f g h i j
                        // it pos:      *

                        // the position of Qt's Java style iterator is between 2 items, we can use next() to get the next item and use previous() to get the previous item, and move the iterator according to the direction.

                        if (ServerPlayer::sortByActionOrder(r_now->preferredTarget, s_now->preferredTarget)) {
                            // 1.the case that ServerPlayer::compareByActionOrder(r_now, s_now) == true, i.e. seat(r_now) < seat(s_now)
                            // because the r is triggerable list, s is the invoke list, so we should move s_it to the front of the just passed item add the r_now into s

                            // the list is now like this: r = a b c d e f   h
                            //                            s = a b   d e f g h i j
                            // it pos:                             r s

                            s_it.previous();
                            s_it.insert(r_now);

                            // so the list becomes like:  r = a b c d e f   h
                            //                            s = a b c d e f g h i j
                            // it pos:                             *
                        } else {
                            // 2. the case that ServerPlayer::compareByActionOrder(r_now, s_now) == true, i.e. seat(r_now) > seat(s_now)
                            // because the r is triggerable list, s is the invoke list, so we should remove the s_now and move r_it to the position just before the deleted item

                            // the list is now like this: r = a b c d e f   h
                            //                            s = a b c d e f g h i j
                            // it pos:                                     s r

                            s_it.remove();
                            r_it.previous();
                            // so the list becomes like:  r = a b c d e f   h
                            //                            s = a b c d e f   h i j
                            // it pos:                                   r s
                        }
                    }

                    // the whole loop will be over when one of r_it or s_it has no next item, but there are situations that another one has more items. Let's deal with this situation.
                    // let's take some other examples.

                    // e.g. let r = a b c d e
                    //      let s = a b c
                    // it pos:           *

                    // now s_it has no next item, but r_it has some next items.
                    // since r is the trigger list, we should add the more items to s.

                    while (r_it.hasNext())
                        s_it.insert(r_it.next());

                    // another example.

                    // e.g. let r = a b c
                    //          s = a b c d e
                    // it pos:           *

                    // now s_it has more items.
                    // since r is the triggerable list, we should remove the more items from s.
                    while (s_it.hasNext())
                        s_it.remove();
                }

                // let the r become the invoke list.
                r = s;

                // we should mark the ones who passed the trigger order as triggered.
                QListIterator<EventPtr> r_it(r);
                r_it.toBack();
                bool over_trigger = true;
                while (r_it.hasPrevious()) {
                    const EventPtr &p = r_it.previous();
                    if (p->triggered)
                        over_trigger = true;
                    else if (over_trigger)
                        p->triggered = true;
                }

            }

        }

        details << r;
    }

    // do a stable sort to details use the operator < of Event in which judge the priority, the seat of invoker, and whether it is a skill of an equip.
    std::stable_sort(details.begin(), details.end(), [](const EventPtr &a1, const EventPtr &a2) { return *a1 < *a2; });

    // mark the skills which missed the trigger timing as it has triggered
    EventPtr over_trigger;
    QListIterator<EventPtr> it(details);
    it.toBack();
    while (it.hasPrevious()) {
        // search the last skill which triggered times isn't 0 from back to front. if found, save it to over_trigger. 
        // if over_trigger is valid, then mark the skills which missed the trigger timing as it has triggered.
        const EventPtr &detail = it.previous();
        if (over_trigger.isNull() || !over_trigger->isValid()) {
            if (detail->triggered)
                over_trigger = detail;
        } else if (*detail < *over_trigger)
            detail->triggered = true;
    }

    detailsList = details;
}

bool GameLogic::trigger(EventType event, ServerPlayer *target, QObject *data)
{
//     EventTriplet triplet(triggerEvent, room);
//     event_stack.push_back(triplet);

    QList<const EventHandler *> skillList = m_handlers[event]; // find all the skills, do the record first. it do the things only for record. it should not and must not interfere the procedure of other skills.
//     foreach(const EventHandler *skill, skillList)
//         skill->record(triggerEvent, room, data);

    EventPtrList details;
    EventPtrList triggered;
    bool interrupt = false;
    try {
        forever {
            getEventHandlersAndSort(event, details, triggered, data, target);

            EventPtrList sameTiming;
            // search for the first skills which can trigger
            foreach (const EventPtr &ptr, details) {
                if (ptr->triggered)
                    continue;
                if (sameTiming.isEmpty())
                    sameTiming << ptr;
                else if (ptr->sameTimingWith(*sameTiming.first()))
                    sameTiming << ptr;
            }

            // if not found, it means that all the skills is triggered done, we can exit the loop now.
            if (sameTiming.isEmpty())
                break;

            EventPtr invoke = sameTiming.first();

            // treat the invoker is NULL, if the triggered skill is some kind of gamerule
            if (invoke->invoker != nullptr && (invoke->eh->priority() >= -5 && invoke->eh->priority() <= 5) && invoke->eh->priority() != 0) {
                // select the triggerorder of same timing
                // if there is a compulsory skill or compulsory effect, it shouldn't be able to cancel
                // if the priority is bigger than 5 or smaller than -5, that means it could be some kind of record skill, notify-client skill or fakemove skill,
                // if the priority is 0, that means it is a gamerule
                // then no need to select the trigger order at these conditions
                bool has_compulsory = false;
                foreach (const EventPtr &detail, sameTiming) {
                    if (detail->isCompulsory) { // judge the compulsory effect/skill in the detail struct
                        has_compulsory = true;
                        break;
                    }
                }
                if (sameTiming.length() > 1 || !has_compulsory) {
                    // since the invoker of the sametiming list is the same, we can use sameTiming.first()->invoker to judge the invoker of this time
                    EventPtr detailSelected = sameTiming.first()->invoker->askForTriggerOrder(sameTiming, !has_compulsory);
                    if (detailSelected.isNull() || !detailSelected->isValid()) {
                        // if cancel is pushed when it is cancelable, we set all the sametiming as triggered, and add all the skills to triggeredList, continue the next loop
                        foreach (const EventPtr &ptr, sameTiming) {
                            ptr->triggered = true;
                            triggered << ptr;
                        }
                        continue;
                    } else
                        invoke = detailSelected;
                }
            }

            // if not cancelled, then we add the selected skill to triggeredList, and add the triggered times of the skill. then we process with the skill's cost and effect.

            invoke->triggered = true;
            triggered << invoke;

            // if cost returned false, we don't process with the skill's left trigger times(use the trick of set it as triggered)
            // if effect returned true, exit the whole loop.

            if (invoke->eh->onCost(this, event, invoke, data, target)) {
                // if we don't insert the target in the cost and there is a preferred target, we set the preferred target as the only target of the skill
                if (invoke->preferredTarget != nullptr && invoke->targets.isEmpty())
                    invoke->targets << invoke->preferredTarget;
                // the show general of hegemony mode can be inserted here
                if (invoke->eh->effect(this, event, invoke, data, target)) {
                    interrupt = true;
                    break;
                }
            } else
                invoke->triggered = true;
        }


//         event_stack.pop_back();

    }
    catch (EventType) {
//         event_stack.pop_back();
        throw;
    }
    return interrupt;
}

QList<ServerPlayer *> GameLogic::players() const
{
    QList<ServerPlayer *> players;
    auto abstractPlayers = this->abstractPlayers();
    foreach (CAbstractPlayer *p, abstractPlayers)
        players << qobject_cast<ServerPlayer *>(p);
    return players;
}

ServerPlayer *GameLogic::findPlayer(uint id) const
{
    return qobject_cast<ServerPlayer *>(findAbstractPlayer(id));
}

ServerPlayer *GameLogic::findPlayer(CServerAgent *agent) const
{
    return qobject_cast<ServerPlayer *>(findAbstractPlayer(agent));
}

QList<ServerPlayer *> GameLogic::allPlayers(bool includeDead) const
{
    QList<ServerPlayer *> players = this->players();
    ServerPlayer *current = currentPlayer();
    if (current == nullptr)
        return players;

    std::sort(players.begin(), players.end(), [](const ServerPlayer *a, const ServerPlayer *b){
        return a->seat() < b->seat();
    });

    int currentIndex = players.indexOf(current);
    if (currentIndex == -1)
        return players;

    QList<ServerPlayer *> allPlayers;
    for (int i = currentIndex; i < players.length(); i++) {
        if (includeDead || players.at(i)->isAlive())
            allPlayers << players.at(i);
    }
    for (int i = 0; i < currentIndex; i++) {
        if (includeDead || players.at(i)->isAlive())
            allPlayers << players.at(i);
    }

    if (current->phase() == Player::Inactive && allPlayers.contains(current)) {
        allPlayers.removeOne(current);
        allPlayers.append(current);
    }

    return allPlayers;
}

QList<ServerPlayer *> GameLogic::otherPlayers(ServerPlayer *except, bool includeDead) const
{
    QList<ServerPlayer *> players = allPlayers(includeDead);
    if (except && (except->isAlive() || includeDead))
        players.removeOne(except);
    return players;
}

void GameLogic::sortByActionOrder(QList<ServerPlayer *> &players) const
{
    QList<ServerPlayer *> allPlayers = this->allPlayers(true);

    QMap<ServerPlayer *, int> actionOrder;
    foreach (ServerPlayer *player, players)
        actionOrder[player] = allPlayers.indexOf(player);

    std::sort(allPlayers.begin(), allPlayers.end(), [&actionOrder](ServerPlayer *a, ServerPlayer *b){
        return actionOrder.value(a) < actionOrder.value(b);
    });
}

Card *GameLogic::getDrawPileCard()
{
    if (m_drawPile->length() < 1)
        reshuffleDrawPile();
    return m_drawPile->first();
}

QList<Card *> GameLogic::getDrawPileCards(int n)
{
    if (m_drawPile->length() < n)
        reshuffleDrawPile();
    return m_drawPile->first(n);
}

void GameLogic::reshuffleDrawPile()
{
    if (m_discardPile->length() <= 0)
        gameOver(QList<ServerPlayer *>());

    m_reshufflingCount++;

    //@to-do: check reshuffling count limit
    /*if (limit > 0 && times == limit)
        gameOver(".");*/

    QList<Card *> cards = m_discardPile->cards();
    m_discardPile->clear();
    qShuffle(cards);
    foreach (Card *card, cards)
        m_cardPosition[card] = m_drawPile;
    m_drawPile->add(cards, CardArea::Bottom);
}

void GameLogic::moveCards(CardsMoveValue &moves)
{
    filterCardsMove(moves);
    QList<ServerPlayer *> allPlayers = this->allPlayers();
    foreach (ServerPlayer *player, allPlayers)
        trigger(BeforeCardsMove, player, &moves);

    filterCardsMove(moves);

    for (int i = 0; i < moves.moves.length(); ++i) {
        const CardMove &move = moves.moves.at(i);
        if (move.fromArea->contains(move.card, true)) {
            if (move.fromArea->remove(move.card)) {
                move.toArea->add(move.card, move.toDirection);
                m_cardPosition[move.card] = move.toArea;
            }
        }
    }

    for (int i = 0; i < moves.virtualMoves.length(); ++i) {
        const CardMove &move = moves.virtualMoves.at(i);
        // there are cases each of which fromArea can be nullptr sometimes, so we shouldn't relay on the move.fromArea
        // but if the move.fromArea != nullptr, the move.fromArea must be virtual card area
        bool flag = true;
        if (move.fromArea != nullptr) {
            // we should check the virtual card is actually in that area
            if (move.fromArea->contains(move.card))
                flag = move.fromArea->remove(move.card);
        }
        // if move.fromArea has succeeded in removing the old card, we should judge whether the toPosition is virtual card area
        if (flag && move.toArea->isVirtualCardArea()) {
            move.toArea->add(move.card);
            m_cardPosition[move.card] = move.toArea;
        } else
            m_cardPosition.remove(move.card);
    }

    QList<ServerPlayer *> viewers = players();
    foreach (ServerPlayer *viewer, viewers) {
        QVariant data = moves.toVariant(viewer);
        CServerAgent *agent = viewer->agent();
        agent->notify(S_COMMAND_MOVE_CARDS, data);
    }

    allPlayers = this->allPlayers();
    foreach (ServerPlayer *player, allPlayers)
        trigger(AfterCardsMove, player, &moves);
}

bool GameLogic::useCard(CardUseValue &use)
{
    if (use.card == nullptr || use.from == nullptr)
        return false;

    //Initialize isHandcard
    use.isHandcard = true;
    QList<Card *> realCards = use.card->realCards();
    foreach (Card *card, realCards) {
        CardArea *area = m_cardPosition[card];
        if (area == nullptr || area->owner() != use.from || area->type() != CardArea::Hand) {
            use.isHandcard = false;
            break;
        }
    }

    if (!use.card->isVirtual()) // make a virtual card of it since all the card in the card using process is virtual card
        use.card = use.card->makeVirtual();

    if (use.from->phase() == Player::Play && use.addHistory)
        use.from->addCardHistory(use.card->objectName());

    try {
        use.card->onUse(this, use);
        trigger(CardUsed, use.from, &use);

        if (use.from) {
            trigger(TargetChoosing, use.from, &use);

            QVariantMap args;
            args["from"] = use.from->id();
            QVariantList tos;
            foreach (ServerPlayer *to, use.to)
                tos << to->id();
            args["to"] = tos;

            /*
            since the use card is all virtual card, we can only use the args["card"]
            if (!use.card->isVirtual())
                args["cardId"] = use.card->id();
            else
            */

            args["card"] = use.card->toVariant();

            room()->broadcastNotification(S_COMMAND_USE_CARD, args);

            if (use.from) {
                if (!use.to.isEmpty()) {
                    foreach (ServerPlayer *to, use.to) {
                        if (!use.to.contains(to))
                            continue;
                        trigger(TargetConfirming, to, &use);
                    }

                    if (use.from && !use.to.isEmpty()) {
                        trigger(TargetChosen, use.from, &use);

                        if (use.from && !use.to.isEmpty()) {
                            foreach (ServerPlayer *to, use.to) {
                                if (!use.to.contains(to))
                                    continue;
                                trigger(TargetConfirmed, to, &use);
                            }

                            use.card->use(this, use);
                        }
                    }
                } else if (use.target) {
                    use.card->use(this, use);
                }
            }
        }

        trigger(CardFinished, use.from, &use);

    } catch (EventType e) {
        //@to-do: handle TurnBroken and StageChange
        throw e;
    }

    return true;
}

bool GameLogic::takeCardEffect(CardEffectValue &effect)
{
    bool canceled = false;
    if (effect.to) {
        if (effect.to->isAlive()) {
            canceled = trigger(CardEffect, effect.to, &effect);
            if (!canceled) {
                canceled = trigger(CardEffected, effect.to, &effect);
                if (!canceled) {
                    effect.use.card->onEffect(this, effect);
                    if (effect.to->isAlive() && !effect.isNullified())
                        effect.use.card->effect(this, effect);
                }
            }
        }
    } else if (effect.use.target) {
        effect.use.card->onEffect(this, effect);
        if (!effect.isNullified())
            effect.use.card->effect(this, effect);
    }
    trigger(PostCardEffected, effect.to, &effect);
    return !canceled;
}

bool GameLogic::invokeProactiveSkill(SkillInvokeValue &invoke)
{
    const ProactiveSkill *proactiveSkill = dynamic_cast<const ProactiveSkill *>(invoke.skill);
    if (proactiveSkill != nullptr) {
        if (proactiveSkill->cost(this, invoke.player, invoke.targets, invoke.cards)) {
            proactiveSkill->effect(this, invoke.player, invoke.targets, invoke.cards);
            return true;
        }
    } else
        QT_THROW(std::bad_cast()); // we only throw it in debug mode @todo_Fs: find all the places which can cause an abnormal processing

    return false;
}

bool GameLogic::respondCard(CardResponseValue &response)
{
    // todo_Fs: make the responsed card a virtual card

    CardsMoveValue moves;
    CardMove move;
    move.card = response.card;
    move.toArea = m_table;
    move.isOpen = true;
    moves.moves << move;
    moveCards(moves);

    bool broken = false;

    broken = trigger(CardResponded, response.from, &response);

    if (response.card && m_table->contains(response.card)) {
        CardsMoveValue moves;
        CardMove move;
        move.card = response.card;
        move.toArea = m_discardPile;
        move.isOpen = true;
        moves.moves << move;
        moveCards(moves);
    }

    return !broken;
}

void GameLogic::judge(JudgeValue &judge)
{
    if (trigger(StartJudge, judge.who, &judge))
        return;

    judge.card = getDrawPileCard();
    judge.updateResult();

    CardsMoveValue moves;
    CardMove move;
    move.card = judge.card;
    move.toArea = judge.who->judgeCards();
    move.isOpen = true;
    moves.moves << move;
    moveCards(moves);

    trigger(AskForRetrial, nullptr, &judge);

    trigger(FinishRetrial, judge.who, &judge);
    trigger(FinishJudge, judge.who, &judge);

    const CardArea *judgeCards = judge.who->judgeCards();
    if (judgeCards->contains(judge.card)) {
        CardsMoveValue moves;
        CardMove move;
        move.card = judge.card;
        move.toArea = m_discardPile;
        move.isOpen = true;
        moves.moves << move;
        moveCards(moves);
    }
}

void GameLogic::retrialCost(JudgeValue &judge, Card *card, bool isReplace)
{
    CardArea *cardArea = m_cardPosition.value(card, nullptr);
    Q_ASSERT(cardArea != nullptr);

    bool trigger_responded = cardArea->type() == CardArea::Hand || cardArea->type() == CardArea::Equip;
    // todo_Fs: make the responsed card a virtual card

    CardsMoveValue moves;
    CardMove move;
    move.card = card;
    move.toArea = judge.who->judgeCards();
    move.isOpen = true;
    moves.moves << move;

    CardMove move2;
    move2.card = judge.card;
    if (isReplace && cardArea->owner() != nullptr)
        move2.toArea = cardArea;
    else
        move2.toArea = m_discardPile;
    move2.isOpen = true;
    moves.moves << move2;
    moveCards(moves);

    if (trigger_responded) {
        CardResponseValue response;
        response.card = card;
        response.from = qobject_cast<ServerPlayer *>(cardArea->owner());
        response.target = judge.card;
        response.isRetrial = true;
        response.to = nullptr;

        trigger(CardResponded, response.from, &response);
    }
}

void GameLogic::retrialEffect(JudgeValue &judge, Card *card)
{
    judge.card = card;
    judge.updateResult();
}

QList<Card *> GameLogic::findCards(const QVariant &data)
{
    QList<Card *> cards;
    QVariantList dataList = data.toList();
    foreach (const QVariant &cardId, dataList) {
        Card *card = findCard(cardId.toUInt());
        if (card)
            cards << card;
    }
    return cards;
}

void GameLogic::damage(DamageValue &damage)
{
    if (damage.to == nullptr || damage.to->isDead())
        return;

    if (!damage.chain && !damage.transfer) {
        trigger(ConfirmDamage, damage.from, &damage);
    }

    if (trigger(BeforeDamage, damage.from, &damage))
        return;

    try {
        do {
            if (trigger(DamageStart, damage.to, &damage))
                break;

            if (damage.from && trigger(Damaging, damage.from, &damage))
                break;

            if (damage.to && trigger(Damaged, damage.to, &damage))
                break;

            if (damage.to)
                trigger(BeforeHpReduced, damage.to, &damage);

            if (damage.to) {
                QVariantMap arg;
                arg["from"] = damage.from ? damage.from->id() : 0;
                arg["to"] = damage.to->id();
                arg["nature"] = damage.nature;
                arg["damage"] = damage.damage;
                room()->broadcastNotification(S_COMMAND_DAMAGE, arg);

                int newHp = damage.to->hp() - damage.damage;
                damage.to->setHp(newHp);
                damage.to->broadcastProperty("hp");

                trigger(AfterHpReduced, damage.to, &damage);
            }

            if (damage.from)
                trigger(AfterDamaging, damage.from, &damage);

            if (damage.to)
                trigger(AfterDamaged, damage.to, &damage);

        } while (false);

        if (damage.to)
            trigger(DamageComplete, damage.to, &damage);

    } catch (EventType e) {
        //@to-do: handle TurnBroken and StageChange
        throw e;
    }
}

void GameLogic::loseHp(ServerPlayer *victim, int lose)
{
    if (lose <= 0 || victim->isDead())
        return;
    
    IntValue data(lose);
    if (trigger(HpLost, victim, &data))
        return;

    lose = data.value;
    if (lose <= 0)
        return;

    victim->setHp(victim->hp() - lose);
    victim->broadcastProperty("hp");

    QVariantMap arg;
    arg["victimId"] = victim->id();
    arg["loseHp"] = lose;
    room()->broadcastNotification(S_COMMAND_LOSE_HP, arg);

    trigger(AfterHpReduced, victim, &data);
    trigger(AfterHpLost, victim, &data);
}

void GameLogic::recover(RecoverValue &recover)
{
    if (recover.to == nullptr || recover.to->lostHp() == 0 || recover.to->isDead())
        return;

    if (trigger(BeforeRecover, recover.to, &recover))
        return;
    if (recover.to == nullptr)
        return;

    int newHp = qMin(recover.to->hp() + recover.recover, recover.to->maxHp());
    recover.to->setHp(newHp);
    recover.to->broadcastProperty("hp");

    QVariantMap arg;
    arg["from"] = recover.from ? recover.from->id() : 0;
    arg["to"] = recover.to->id();
    arg["num"] = recover.recover;
    room()->broadcastNotification(S_COMMAND_RECOVER, arg);

    trigger(AfterRecover, recover.to, &recover);
}

void GameLogic::killPlayer(ServerPlayer *victim, DamageValue *damage)
{
    victim->setAlive(false);
    victim->broadcastProperty("alive");
    victim->broadcastProperty("role");

    DeathValue death;
    death.who = victim;
    death.damage = damage;

    trigger(BeforeGameOverJudge, victim, damage);
    trigger(GameOverJudge, victim, damage);

    trigger(Died, victim, damage);
    trigger(BuryVictim, victim, damage);
}

void GameLogic::gameOver(const QList<ServerPlayer *> &winners)
{
    QVariantList data;
    foreach (ServerPlayer *winner, winners)
        data << winner->id();
    room()->broadcastNotification(S_COMMAND_GAME_OVER, data);
    throw GameFinish;
}

QMap<uint, QList<const General *>> GameLogic::broadcastRequestForGenerals(const QList<ServerPlayer *> &players, int num, int limit)
{
    GeneralList generals;
    QList<const Package *> packages = this->packages();
    foreach(const Package *package, packages)
        generals << package->generals();
    qShuffle(generals);

    int minCandidateNum = limit * players.length();
    while (minCandidateNum > generals.length())
        generals << generals.mid(0, minCandidateNum - generals.length());

    QMap<ServerPlayer *, GeneralList> playerCandidates;

    foreach (ServerPlayer *player, players) {
        GeneralList candidates = generals.mid((player->seat() - 1) * limit, limit);
        playerCandidates[player] = candidates;

        QVariantList candidateData;
        foreach (const General *general, candidates)
            candidateData << general->id();

        QVariantList bannedPairData;
        //@todo: load banned pairs

        QVariantMap data;
        data["num"] = num;
        data["candidates"] = candidateData;
        data["banned"] = bannedPairData;

        CServerAgent *agent = findAgent(player);
        agent->prepareRequest(S_COMMAND_CHOOSE_GENERAL, data);
    }

    //@to-do: timeout should be loaded from config
    CRoom *room = this->room();
    QList<CServerAgent *> agents;
    foreach (ServerPlayer *player, players)
        agents << player->agent();
    room->broadcastRequest(agents, settings()->timeout * 1000);

    QMap<uint, GeneralList> result;
    foreach (ServerPlayer *player, players) {
        const GeneralList &candidates = playerCandidates[player];

        GeneralList generals;
        CServerAgent *agent = findAgent(player);
        if (agent) {
            QVariantList reply = agent->waitForReply(0).toList();
            foreach (const QVariant &choice, reply) {
                uint id = choice.toUInt();
                foreach (const General *general, candidates) {
                    if (general->id() == id) {
                        generals << general;
                        break;
                    }
                }
            }
        }

        //@to-do: handle banned pairs
        if (generals.length() < num)
            generals = candidates.mid(0, num);

        result[player->id()] = generals;
    }

    return result;
}

CAbstractPlayer *GameLogic::createPlayer(CServerAgent *agent)
{
    return new ServerPlayer(this, agent);
}

void GameLogic::loadMode(const GameMode *mode)
{
    setGameRule(mode->rule());

    QList<const EventHandler *> rules = mode->extraRules();
    foreach (const EventHandler *rule, rules)
        addEventHandler(rule);

    setPackages(Sanguosha.getPackages(mode));
}

const RoomSettings *GameLogic::settings() const
{
    return room()->settings<RoomSettings>();
}

void GameLogic::prepareToStart()
{
    CRoom *room = this->room();

    //Load game mode
    const GameMode *mode = Sanguosha.mode(settings()->mode);
    loadMode(mode);

    //Arrange seats for all the players
    QList<ServerPlayer *> players = this->players();
    qShuffle(players);
    for (int i = 1; i < players.length(); i++) {
        players[i - 1]->setSeat(i);
        players[i - 1]->setNext(players.at(i));
    }
    ServerPlayer *lastPlayer = players.last();
    lastPlayer->setSeat(players.length());
    lastPlayer->setNext(players.first());
    setCurrentPlayer(players.first());

    QVariantList playerList;
    foreach (ServerPlayer *player, players) {
        CServerAgent *agent = findAgent(player);
        QVariantMap info;
        info["agentId"] = agent->id();
        info["playerId"] = player->id();
        playerList << info;
    }
    room->broadcastNotification(S_COMMAND_ARRANGE_SEAT, playerList);

    //Import packages
    GeneralList generals;
    foreach (const Package *package, m_packages) {
        generals << package->generals();

        QList<const Card *> cards = package->cards();
        foreach (const Card *card, cards)
            m_cards.insert(card->id(), card->clone());
    }

    //Prepare cards
    QVariantList cardData;
    foreach (const Card *card, m_cards)
        cardData << card->id();
    room->broadcastNotification(S_COMMAND_PREPARE_CARDS, cardData);

    foreach (Card *card, m_cards) {
        m_drawPile->add(card);
        m_cardPosition[card] = m_drawPile;
    }
    qShuffle(m_drawPile->cards());

    m_gameRule->prepareToStart(this);
}

void GameLogic::filterCardsMove(CardsMoveValue &moves)
{
    // thanks to implicit sharing!!
    QList<CardMove> move = moves.moves;
    moves.moves.clear();


    if (move.isEmpty())
        return;

    for (int i = 0; i < move.length(); ++i) {
        // for each card, we should fill the source information into it
        // for virtual cards, we should find the real cards of it to move
        const CardMove &singleMove = move.value(i);
        if (singleMove.card == nullptr)
            continue;
        QList<Card *> cards;
        if (singleMove.card->isVirtual())
            cards << singleMove.card->realCards();
        else
            cards << singleMove.card;

        // for real cards
        if (singleMove.fromArea == nullptr) {
            CardMove modifiedMove = singleMove;
            foreach (Card *card, cards) {
                CardArea *area = m_cardPosition.value(card);
                modifiedMove.fromArea = area;
                modifiedMove.card = card;
                moves.moves << modifiedMove;
            }
        } else {
            foreach (Card *card, cards) {
                // check the card is actually in the position!!! if not, we don't move it
                if (singleMove.fromArea->contains(card, true)) {
                    CardMove modifiedMove = singleMove;
                    modifiedMove.card = card;
                    moves.moves << modifiedMove;
                }
            }
        }

        // for virtual cards, we should add it to virtual move
        if (singleMove.card->isVirtual()) {
            CardMove modifiedMove = singleMove;
            if (singleMove.fromArea == nullptr)
                modifiedMove.fromArea = m_cardPosition.value(singleMove.card);
            else if (!singleMove.fromArea->contains(singleMove.card))
                modifiedMove.fromArea = nullptr;
            // we should still move the cards even if there is no fromArea of virtual card
            moves.virtualMoves << modifiedMove;
        }
    }
}

void GameLogic::run()
{
    qsrand((uint) QDateTime::currentMSecsSinceEpoch());

    prepareToStart();

    //@to-do: Turn broken event
    QList<ServerPlayer *> allPlayers = this->allPlayers();
    foreach (ServerPlayer *player, allPlayers)
        trigger(GameStart, player);

    forever {
        try {
            ServerPlayer *current = currentPlayer();
            forever {
                if (current->seat() == 1)
                    m_round++;
                if (current->isDead()) {
                    current = current->next();
                    continue;
                }

                setCurrentPlayer(current);
                trigger(TurnStart, current);
                current = current->next();

                while (!m_extraTurns.isEmpty()) {
                    ServerPlayer *extra = m_extraTurns.takeFirst();
                    setCurrentPlayer(extra);
                    trigger(TurnStart, extra);
                }
            }
        } catch (EventType event) {
            if (event == GameFinish) {
                return;
            } else if (event == TurnBroken) {
                ServerPlayer *current = currentPlayer();
                trigger(TurnBroken, current);
                ServerPlayer *next = current->nextAlive(1, false);
                if (current->phase() != Player::Inactive) {
                    m_gameRule->effect(this, PhaseEnd, EventPtr(), nullptr, current);
                    //@todo:
                    current->setPhase(Player::Inactive);
                    current->broadcastProperty("phase");
                }
                setCurrentPlayer(next);
            }
        }
    }
}

ServerPlayer *GameLogic::getFront(ServerPlayer *a, ServerPlayer *b) const
{
    QList<ServerPlayer *> pls = allPlayers();
    return pls.indexOf(a) < pls.indexOf(b) ? a : b;
}
