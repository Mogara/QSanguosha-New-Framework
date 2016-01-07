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
#include "cglobal.h"
#include "client.h"
#include "clientplayer.h"
#include "clientskill.h"
#include "cardpattern.h"
#include "engine.h"
#include "general.h"
#include "protocol.h"
#include "roomscene.h"
#include "skill.h"
#include "util.h"

static QString AreaTypeToString(CardArea::Type type)
{
    switch (type) {
    case CardArea::Judge:
    case CardArea::Table:
    case CardArea::DiscardPile:
        return "table";
    case CardArea::Wugu:
        return "wugu";
    case CardArea::DrawPile:
        return "drawPile";
    case CardArea::Hand:
        return "hand";
    case CardArea::Equip:
        return "equip";
    case CardArea::DelayedTrick:
        return "delayedTrick";
    default:
        return "unknown";
    }
}

static QVariant AreaToVariant(const CardsMoveStruct::Area area)
{
    QVariantMap data;
    data["seat"] = area.owner ? area.owner->seat() : 0;
    data["type"] = AreaTypeToString(area.type);
    data["name"] = area.name;
    return data;
}

RoomScene::RoomScene(QQuickItem *parent)
    : QQuickItem(parent)
    , m_client(Client::instance())
    , m_respondingState(InactiveState)
    , m_minRespondingCardNum(1)
    , m_maxRespondingCardNum(1)
    , m_respondingOptional(true)
    , m_viewAsSkill(nullptr)
{
    connect(m_client, &Client::promptReceived, this, &RoomScene::showPrompt);
    connect(m_client, &Client::chooseGeneralRequested, this, &RoomScene::onChooseGeneralRequested);
    connect(m_client, &Client::seatArranged, this, &RoomScene::onSeatArranged);
    connect(m_client, &Client::cardsMoved, this, &RoomScene::onCardsMoved);
    connect(m_client, &Client::usingCard, this, &RoomScene::onUsingCard);
    connect(m_client, &Client::damageDone, this, &RoomScene::onDamageDone);
    connect(m_client, &Client::recoverDone, this, &RoomScene::onRecoverDone);
    connect(m_client, &Client::cardUsed, this, &RoomScene::onCardUsed);
    connect(m_client, &Client::cardAsked, this, &RoomScene::onCardAsked);
    connect(m_client, &Client::cardsAsked, this, &RoomScene::onCardsAsked);
    connect(m_client, &Client::amazingGraceStarted, this, &RoomScene::onAmazingGraceStarted);
    connect(m_client, &Client::amazingGraceFinished, this, &RoomScene::clearPopupBox);
    connect(m_client, &Client::choosePlayerCardRequested, this, &RoomScene::onChoosePlayerCardRequested);
    connect(m_client, &Client::cardShown, this, &RoomScene::onCardShown);
    connect(m_client, &Client::optionRequested, this, &RoomScene::showOptions);
}

void RoomScene::onCardsMoved(const QList<CardsMoveStruct> &moves)
{
    QVariantList paths;
    foreach (const CardsMoveStruct &move, moves) {
        QVariantMap path;
        path["from"] = AreaToVariant(move.from);
        path["to"] = AreaToVariant(move.to);

        QVariantList cards;
        foreach (const Card *card, move.cards) {
            QVariantMap cardData = convertToMap(card);
            cardData["subtype"] = card ? card->subtype() : 0;
            cards << cardData;
        }
        path["cards"] = cards;
        paths << path;
    }

    moveCards(paths);
}

void RoomScene::checkTargetFeasibility()
{
    const Card *card = nullptr;
    const ClientPlayer *self = m_client->selfPlayer();

    if (m_viewAsSkill) {
        const ClientPlayer *self = m_client->selfPlayer();
        QList<Card *> copy;
        foreach (const Card *card, m_selectedCard)
            copy << card->clone();
        Card *result = m_viewAsSkill->viewAs(copy, self);
        foreach (Card *card, copy)
            delete card;
        card = result;
    } else if (m_selectedCard.length() == 1) {
        card = m_selectedCard.first();
    }

    if (card) {
        bool acceptable = card->isTargetFixed() || card->targetFeasible(m_selectedPlayer, self) || (m_selectedPlayer.isEmpty() && card->canRecast());
        if (acceptable && !m_assignedTargets.isEmpty()) {
            foreach (const Player *target, m_assignedTargets) {
                if (!m_selectedPlayer.contains(target)) {
                    acceptable = false;
                    break;
                }
            }
        }
        setAcceptEnabled(acceptable);

        QVariantList seats;
        if (!card->isTargetFixed()) {
            const ClientPlayer *self = m_client->selfPlayer();
            QList<const ClientPlayer *> players = m_client->players();
            foreach (const ClientPlayer *player, players) {
                if (card->targetFilter(m_selectedPlayer, player, self))
                    seats << player->seat();
            }
        }
        enablePhotos(seats);

        if (card->isVirtual())
            delete card;
    } else {
        setAcceptEnabled(false);
    }
}

void RoomScene::resetDashboard()
{
    m_respondingState = InactiveState;

    m_selectedCard.clear();
    enableCards(QVariantList());
    m_selectedPlayer.clear();
    enablePhotos(QVariantList());

    setAcceptEnabled(false);
    setRejectEnabled(false);
    setFinishEnabled(false);

    hidePrompt();

    m_viewAsSkill = nullptr;
    const ClientPlayer *self = m_client->selfPlayer();
    QList<const Skill *> skills = self->skills();
    foreach (const Skill *skill, skills) {
        ClientSkill *model = self->getSkill(skill);
        model->setEnabled(false);
    }
}

void RoomScene::enableCards(const QString &pattern)
{
    QVariantList cardIds;
    const ClientPlayer *self = m_client->selfPlayer();
    QList<Card *> cards = self->handcardArea()->cards();
    cards << self->equipArea()->cards();
    if (pattern.isEmpty()) {
        foreach (Card *card, cards) {
            if (card->isAvailable(self))
                cardIds << card->id();
        }
    } else {
        CardPattern exp(pattern);
        foreach (const Card *card, cards) {
            if (exp.match(self, card))
                cardIds << card->id();
        }
    }
    enableCards(cardIds);

    QList<const Skill *> skills = self->skills();
    foreach (const Skill *skill, skills) {
        if (skill->type() != Skill::ViewAsType)
            continue;
        const ViewAsSkill *viewAsSkill = static_cast<const ViewAsSkill *>(skill);
        ClientSkill *model = self->getSkill(skill);
        model->setEnabled(viewAsSkill->isAvailable(self, pattern));
    }
}

void RoomScene::enableCards(const QList<const Card *> &cards)
{
    QVariantList cardIds;
    foreach (const Card *card, cards)
        cardIds << card->id();
    enableCards(cardIds);
}

void RoomScene::onSeatArranged()
{
    QList<const ClientPlayer *> players = m_client->players();
    qSort(players.begin(), players.end(), [](const ClientPlayer *p1, const ClientPlayer *p2){
        return p1->seat() < p2->seat();
    });

    const ClientPlayer *self = m_client->selfPlayer();
    int selfIndex = players.indexOf(self);
    players = players.mid(selfIndex + 1) + players.mid(0, selfIndex);
    setProperty("dashboardModel", qConvertToModel(self));
    setProperty("photoModel", qConvertToModel(players));
    setProperty("playerNum", players.length() + 1);
}

void RoomScene::onChooseGeneralRequested(const QStringList &candidates)
{
    Engine *engine = Engine::instance();

    QVariantList generals;
    foreach (const QString &candidate, candidates) {
        const General *general = engine->getGeneral(candidate);
        if (general == nullptr)
            continue;

        QVariantMap generalData;
        generalData["name"] = candidate;
        generalData["kingdom"] = general->kingdom();
        generals << generalData;
    }
    chooseGeneral(generals);
}

void RoomScene::onChooseGeneralFinished(const QString &head, const QString &deputy)
{
    QVariantList data;
    data << head;
    data << deputy;
    m_client->replyToServer(S_COMMAND_CHOOSE_GENERAL, data);
}

void RoomScene::onUsingCard(const QString &pattern, const QList<const Player *> &assignedTargets)
{
    m_respondingState = UsingCardState;
    m_assignedTargets = assignedTargets;
    m_respondingPattern.clear();

    enableCards(pattern);

    setAcceptEnabled(false);
    setRejectEnabled(false);
    setFinishEnabled(true);
}

void RoomScene::onCardSelected(const QVariantList &cardIds)
{
    m_selectedCard.clear();
    foreach (const QVariant &cardId, cardIds) {
        const Card *card = m_client->findCard(cardId.toUInt());
        if (card)
            m_selectedCard << card;
    }

    switch (m_respondingState) {
    case UsingCardState:{
        if (m_selectedCard.isEmpty()) {
            if (m_viewAsSkill)
                onSkillActivated(m_viewAsSkill->name(), true);
            else
                onUsingCard();
        } else {
            if (m_viewAsSkill) {
                const ClientPlayer *self = m_client->selfPlayer();;
                QList<Card *> cards = self->handcardArea()->cards();
                cards << self->equipArea()->cards();

                QVariantList enabled;
                foreach (const Card *card, m_selectedCard)
                    enabled << card->id();

                foreach (const Card *card, cards) {
                    if (m_selectedCard.contains(card))
                        continue;
                    if (m_viewAsSkill->viewFilter(m_selectedCard, card, self, m_respondingPattern))
                        enabled << card->id();
                }
                enableCards(enabled);
            } else {
                const Card *card = m_selectedCard.first();
                uint id = card->id();
                enableCards(QVariantList() << id);
            }
            m_selectedPlayer.clear();
            checkTargetFeasibility();
        }
        break;
    }
    case RespondingCardState:{
        int selectedNum = m_selectedCard.length();
        setAcceptEnabled(m_minRespondingCardNum <= selectedNum && selectedNum <= m_maxRespondingCardNum);
        if (selectedNum >= m_maxRespondingCardNum) {
            enableCards(m_selectedCard);
        } else {
            enableCards(m_respondingPattern);
        }
        break;
    }
    default:;
    }
}

void RoomScene::onPhotoSelected(const QVariantList &seats)
{
    QSet<int> selectedSeats;
    foreach (const QVariant &seat, seats)
        selectedSeats << seat.toInt();

    m_selectedPlayer.clear();
    QList<const ClientPlayer *> players = m_client->players();
    foreach (const ClientPlayer *player, players) {
        if (selectedSeats.contains(player->seat()))
            m_selectedPlayer << player;
    }

    checkTargetFeasibility();
}

void RoomScene::onAccepted()
{
    switch (m_respondingState) {
    case UsingCardState:{
        m_client->useCard(m_selectedCard, m_selectedPlayer, m_viewAsSkill);
        break;
    }
    case RespondingCardState:{
        m_client->respondCard(m_selectedCard, m_viewAsSkill);
        break;
    }
    default:;
    }

    resetDashboard();
}

void RoomScene::onRejected()
{
    if (m_respondingState == RespondingCardState)
        m_client->replyToServer(S_COMMAND_ASK_FOR_CARD);

    resetDashboard();
}

void RoomScene::onFinished()
{
    if (m_respondingState == UsingCardState)
        m_client->replyToServer(S_COMMAND_USE_CARD);

    resetDashboard();
}

void RoomScene::onAmazingGraceTaken(uint cid)
{
    m_client->replyToServer(S_COMMAND_TAKE_AMAZING_GRACE, cid);
}

void RoomScene::onPlayerCardSelected(uint cid)
{
    m_client->replyToServer(S_COMMAND_CHOOSE_PLAYER_CARD, cid);
}

void RoomScene::onSkillActivated(const QString &skillName, bool activated)
{
    Engine *engine = Engine::instance();
    const Skill *originalSkill = engine->getSkill(skillName);
    if (originalSkill->type() != Skill::ViewAsType)
        return;

    if (activated) {
        const ViewAsSkill *skill = static_cast<const ViewAsSkill *>(originalSkill);
        m_viewAsSkill = skill;

        const ClientPlayer *self = m_client->selfPlayer();

        QList<Card *> cards = self->handcardArea()->cards() + self->equipArea()->cards();
        QVariantList enabled;
        foreach (Card *card, cards) {
            if (skill->viewFilter(m_selectedCard, card, self, m_respondingPattern))
                enabled << card->id();
        }
        enableCards(enabled);
    } else {
        m_viewAsSkill = nullptr;

        if (m_respondingState == UsingCardState)
            onUsingCard();
        else if (m_respondingState == RespondingCardState)
            enableCards(m_respondingPattern);
        else if (m_respondingState == InactiveState)
            enableCards(QVariantList());
    }
}

void RoomScene::onOptionSelected(int selected)
{
    m_client->replyToServer(S_COMMAND_TRIGGER_ORDER, selected);
}

void RoomScene::onDamageDone(const ClientPlayer *victim, DamageStruct::Nature nature, int damage)
{
    if (damage <= 0)
        return;

    int seat = victim->seat();
    startEmotion("damage", seat);

    //@to-do:
    if (nature == DamageStruct::Fire) {

    } else if (nature == DamageStruct::Thunder) {

    }

    playAudio(QString("system/injure%1.ogg").arg(qMin(damage, 3)));
}

void RoomScene::onRecoverDone(const ClientPlayer *from, const ClientPlayer *to, int num)
{
    C_UNUSED(from);
    C_UNUSED(to);
    C_UNUSED(num);
    playAudio("card/common/peach.ogg");
}

void RoomScene::onCardUsed(const ClientPlayer *from, const QList<const ClientPlayer *> &tos)
{
    QVariantList toSeats;
    foreach (const ClientPlayer *to, tos)
        toSeats << to->seat();
    showIndicatorLine(from->seat(), toSeats);
}

void RoomScene::onCardAsked(const QString &pattern)
{
    m_respondingState = RespondingCardState;
    m_respondingPattern = pattern;
    m_minRespondingCardNum = m_maxRespondingCardNum = 1;
    m_respondingOptional = true;
    enableCards(pattern);
    setRejectEnabled(true);
}

void RoomScene::onCardsAsked(const QString &pattern, int minNum, int maxNum, bool optional)
{
    m_respondingState = RespondingCardState;
    m_respondingPattern = pattern;
    m_minRespondingCardNum = minNum;
    m_maxRespondingCardNum = maxNum;
    m_respondingOptional = optional;
    enableCards(pattern);
    setRejectEnabled(optional);
}

void RoomScene::onAmazingGraceStarted()
{
    const CardArea *wugu = m_client->wugu();
    QList<Card *> cards = wugu->cards();
    QVariantList cardList;
    foreach (Card *card, cards) {
        QVariantMap data = convertToMap(card);
        data["selectable"] = true;
        cardList << data;
    }
    askToChooseCards(cardList);
}

void RoomScene::onChoosePlayerCardRequested(const QList<Card *> &handcards, const QList<Card *> &equips, const QList<Card *> &delayedTricks)
{
    QVariantList h;
    foreach (Card *card, handcards)
        h << convertToMap(card);
    QVariantList e;
    foreach (Card *card, equips)
        e << convertToMap(card);
    QVariantList j;
    foreach (Card *card, delayedTricks)
        j << convertToMap(card);
    askToChoosePlayerCard(h, e, j);
}

void RoomScene::onCardShown(const ClientPlayer *from, const QList<const Card *> &cards)
{
    QVariantList data;
    foreach (const Card *card, cards)
        data << convertToMap(card);
    showCard(from->seat(), data);
}

QVariantMap RoomScene::convertToMap(const Card *card) const
{
    QVariantMap data;
    if (card) {
        data["cid"] = card->id();
        data["name"] = card->objectName();
        data["suit"] = card->suitString();
        data["number"] = card->number();
    } else {
        data["cid"] = 0;
        data["name"] = "hegback";
        data["number"] = 0;
        data["suit"] = "";
    }
    return data;
}

C_REGISTER_QMLTYPE("Sanguosha", 1, 0, RoomScene)
