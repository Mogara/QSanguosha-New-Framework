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
#include "cardpattern.h"
#include "engine.h"
#include "general.h"
#include "protocol.h"
#include "roomscene.h"
#include "util.h"

static QString AreaTypeToString(CardArea::Type type)
{
    switch (type) {
    case CardArea::Judge:
    case CardArea::Table:
    case CardArea::DiscardPile:
        return "table";
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
{
    connect(m_client, &Client::chooseGeneralRequested, this, &RoomScene::onChooseGeneralRequested);
    connect(m_client, &Client::seatArranged, this, &RoomScene::onSeatArranged);
    connect(m_client, &Client::cardsMoved, this, &RoomScene::onCardsMoved);
    connect(m_client, &Client::usingCard, this, &RoomScene::onUsingCard);
    connect(m_client, &Client::damageDone, this, &RoomScene::onDamageDone);
    connect(m_client, &Client::recoverDone, this, &RoomScene::onRecoverDone);
    connect(m_client, &Client::cardUsed, this, &RoomScene::onCardUsed);
    connect(m_client, &Client::cardAsked, this, &RoomScene::onCardAsked);
    connect(m_client, &Client::cardsAsked, this, &RoomScene::onCardsAsked);
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
            QVariantMap cardData;
            if (card) {
                cardData["cid"] = card->id();
                cardData["name"] = card->objectName();
                cardData["number"] = card->number();
                cardData["suit"] = card->suitString();
            } else {
                cardData["cid"] = 0;
                cardData["name"] = "hegback";
                cardData["number"] = 0;
                cardData["suit"] = "";
            }
            cards << cardData;
        }
        path["cards"] = cards;
        paths << path;
    }

    moveCards(paths);
}

void RoomScene::updateButtonState()
{
    if (m_selectedCard.length() == 1) {
        const Card *card = m_selectedCard.first();
        const ClientPlayer *self = m_client->selfPlayer();
        setAcceptEnabled(card->targetFeasible(m_selectedPlayer, self));
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
}

void RoomScene::enableCards(const QString &pattern)
{
    QVariantList cardIds;
    const ClientPlayer *self = m_client->selfPlayer();
    CardPattern exp(pattern);
    QList<Card *> handcards = self->handcards()->cards();
    foreach (const Card *card, handcards) {
        if (exp.match(self, card))
            cardIds << card->id();
    }
    QList<Card *> equips = self->equips()->cards();
    foreach (const Card *card, equips) {
        if (exp.match(self, card))
            cardIds << card->id();
    }
    enableCards(cardIds);
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

void RoomScene::onUsingCard(const QString &pattern)
{
    m_respondingState = UsingCardState;

    if (!pattern.isEmpty()) {
        //@todo: load CardPattern
    } else {
        //@todo: filter usable cards
        const ClientPlayer *self = m_client->selfPlayer();
        QList<Card *> cards = self->handcards()->cards();
        QVariantList cardIds;
        foreach (Card *card, cards) {
            if (card->isAvailable(self))
                cardIds << card->id();
        }
        enableCards(cardIds);
    }

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
            onUsingCard(QString());
        } else {
            const Card *card = m_selectedCard.first();
            uint id = card->id();
            enableCards(QVariantList() << id);
            m_selectedPlayer.clear();

            QVariantList seats;
            const ClientPlayer *self = m_client->selfPlayer();
            QList<const ClientPlayer *> players = m_client->players();
            foreach (const ClientPlayer *player, players) {
                if (card->targetFilter(m_selectedPlayer, player, self))
                    seats << player->seat();
            }
            enablePhotos(seats);
        }
        updateButtonState();
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

    updateButtonState();
}

void RoomScene::onAccepted()
{
    switch (m_respondingState) {
    case UsingCardState:{
        if (m_selectedCard.length() == 1) {
            m_client->useCard(m_selectedCard.first(), m_selectedPlayer);
        }
        break;
    }
    case RespondingCardState:{
        m_client->respondCard(m_selectedCard);
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

void RoomScene::onCardAsked(const QString &pattern, const QString &prompt)
{
    m_respondingState = RespondingCardState;
    m_respondingPattern = pattern;
    m_minRespondingCardNum = m_maxRespondingCardNum = 1;
    m_respondingOptional = true;
    showPrompt(prompt);
    enableCards(pattern);
    setRejectEnabled(true);
}

void RoomScene::onCardsAsked(const QString &pattern, const QString &prompt, int minNum, int maxNum, bool optional)
{
    m_respondingState = RespondingCardState;
    m_respondingPattern = pattern;
    m_minRespondingCardNum = minNum;
    m_maxRespondingCardNum = maxNum;
    m_respondingOptional = optional;
    showPrompt(prompt);
    enableCards(pattern);
    setRejectEnabled(optional);
}

C_REGISTER_QMLTYPE("Sanguosha", 1, 0, RoomScene)
