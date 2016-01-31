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
#include "client.h"
#include "clientplayer.h"
#include "engine.h"
#include "protocol.h"
#include "skill.h"

#include <cclientuser.h>

#include <QtQml>

static Client *ClientInstance = nullptr;

Client::Client(QObject *parent)
    : CClient(parent)
    , m_wugu(new CardArea(CardArea::Wugu))
{
    ClientInstance = this;
    connect(this, &CClient::gameStarted, this, &Client::restart);
}

Client *Client::instance()
{
    static Client *client = new Client(qApp);
    return client;
}

Client::~Client()
{
    delete m_wugu;

    foreach (Card *card, m_cards)
        card->deleteLater();

    if (ClientInstance == this)
        ClientInstance = nullptr;
}

const ClientPlayer *Client::findPlayer(CClientUser *user) const
{
    return m_user2player.value(user);
}

QList<const ClientPlayer *> Client::players() const
{
    QList<const ClientPlayer *> players;
    foreach (const ClientPlayer *player, m_players)
        players << player;
    return players;
}

int Client::playerNum() const
{
    return m_players.size();
}

void Client::useCard(const QList<const Card *> &cards, const QList<const Player *> &targets, const Skill *skill)
{
    QVariantMap data;

    QVariantList cardData;
    foreach (const Card *card, cards)
        cardData << card->id();
    data["cards"] = cardData;

    QVariantList to;
    foreach (const Player *target, targets)
        to << target->id();
    data["to"] = to;

    data["skillId"] = skill ? skill->id() : 0;

    replyToServer(S_COMMAND_USE_CARD, data);
}

void Client::respondCard(const QList<const Card *> &cards, const Skill *skill)
{
    QVariantMap data;

    QVariantList cardData;
    foreach (const Card *card, cards)
        cardData << card->id();
    data["cards"] = cardData;

    data["skillId"] = skill ? skill->id() : 0;

    replyToServer(S_COMMAND_ASK_FOR_CARD, data);
}

void Client::AddInteraction(int command, Client::Callback callback)
{
    CClient::AddInteraction(command, reinterpret_cast<CClient::Callback>(callback));
}

void Client::AddCallback(int command, Client::Callback callback)
{
    CClient::AddCallback(command, reinterpret_cast<CClient::Callback>(callback));
}

void Client::restart()
{
    foreach (ClientPlayer *player, m_players)
        player->deleteLater();
    m_players.clear();

    foreach (Card *card, m_cards)
        delete card;
    m_cards.clear();
}

CardArea *Client::findArea(const CardsMoveStruct::Area &area)
{
    if (area.owner) {
        switch (area.type) {
        case CardArea::Hand:
            return area.owner->handcardArea();
        case CardArea::Equip:
            return area.owner->equipArea();
        case CardArea::DelayedTrick:
            return area.owner->delayedTrickArea();
        case CardArea::Judge:
            return area.owner->judgeCards();
        default:
            return nullptr;
        }
    } else {
        switch (area.type) {
        case CardArea::Wugu:
            return m_wugu;
        default:
            return nullptr;
        }
    }
    return nullptr;
}

QList<Card *> Client::findCards(const QVariant &data)
{
    QList<Card *> result;
    const QVariantList cardList = data.toList();
    if (cardList.isEmpty()) {
        int cardNum = data.toInt();
        result.reserve(cardNum);
        for (int i = 0; i < cardNum; i++)
            result << nullptr;
    } else {
        foreach (const QVariant &cardData, cardList) {
            Card *card = m_cards.value(cardData.toUInt());
            if (card)
                result << card;
            else
                qWarning("Unknown card id received: %u", cardData.toUInt());
        }
    }
    return result;
}

QList<const ClientPlayer *> Client::findPlayers(const QVariant &data)
{
    QList<const ClientPlayer *> players;
    const QVariantList playerList = data.toList();
    foreach (const QVariant &id, playerList) {
        const ClientPlayer *player = findPlayer(id.toUInt());
        if (player)
            players << player;
    }
    return players;
}

void Client::ShowPromptCommand(Client *client, const QVariant &data)
{
    const QVariantList args = data.toList();
    if (args.isEmpty())
        return;

    QString message = args.first().toString();
    if (message.isEmpty())
        return;

    message = tr(message.toLatin1().constData());
    for (int i = 1; i < args.length(); i++) {
        QString arg = args.at(i).toString();
        if (arg == "player") {
            i++;
            if (i >= args.length())
                break;
            uint playerId = args.at(i).toUInt();
            const ClientPlayer *player = client->findPlayer(playerId);
            if (player)
                message = message.arg(player->fullGeneralName());
            else
                message = message.arg(tr("Unknown"));
        } else if (arg == "card") {
            i++;
            if (i >= args.length())
                break;
            uint cardId = args.at(i).toUInt();
            const Card *card = client->findCard(cardId);
            if (card) {
                message = message.arg(QString("%1<img src=\"image://root/card/suit/%2.png\" />%3")
                                      .arg(tr(card->objectName()))
                                      .arg(card->suitString())
                                      .arg(card->number()));
            }
        } else {
            message = message.arg(arg);
        }
    }

    emit client->promptReceived(message);
}

void Client::ArrangeSeatCommand(Client *client, const QVariant &data)
{
    QVariantList infos = data.toList();
    if (infos.isEmpty())
        return;

    QList<ClientPlayer *> players;
    players.reserve(infos.length());

    foreach (const QVariant &rawInfo, infos) {
        const QVariantMap info = rawInfo.toMap();

        CClientUser *agent = nullptr;
        if (info.contains("userId")) {
            uint userId = info["userId"].toUInt();
            agent = client->findUser(userId);
        } else if (info.contains("robotId")) {
            uint robotId = info["robotId"].toUInt();
            agent = client->findRobot(robotId);
        }

        if (agent) {
            ClientPlayer *player = new ClientPlayer(agent, client);
            player->setId(info["playerId"].toUInt());
            client->m_players[player->id()] = player;
            client->m_user2player[agent] = player;
            player->setScreenName(agent->screenName());

            players << player;
        }
    }

    if (!players.isEmpty()) {
        for (int i = 1; i < infos.length(); i++) {
            Player *prev = players.at(i - 1);
            prev->setSeat(i);
            prev->setNext(players.at(i));
        }

        Player *last = players.last();
        last->setSeat(players.length());
        last->setNext(players.first());
    }


    emit client->seatArranged();
}

void Client::PrepareCardsCommand(Client *client, const QVariant &data)
{
    foreach (Card *card, client->m_cards)
        card->deleteLater();
    client->m_cards.clear();

    Engine *engine = Engine::instance();
    QVariantList cardData = data.toList();
    foreach (const QVariant &cardId, cardData) {
        const Card *card = engine->getCard(cardId.toUInt());
        if (card)
            client->m_cards[card->id()] = card->clone();
    }
}

void Client::UpdatePlayerPropertyCommand(Client *client, const QVariant &data)
{
    QVariantList dataList = data.toList();
    if (dataList.length() != 3)
        return;

    uint playerId = dataList.at(0).toUInt();
    ClientPlayer *player = client->m_players.value(playerId);
    if (player) {
        QString name = dataList.at(1).toString();
        QVariant value = dataList.at(2);
        player->setProperty(name.toLatin1().constData(), value);
    }
}

void Client::ChooseGeneralRequestCommand(Client *client, const QVariant &data)
{
    QVariantList dataList = data.toList();
    if (dataList.length() < 2)
        return;

    QVariantList candidateData = dataList.at(0).toList();

    Engine *engine = Engine::instance();
    QList<const General *> generals;
    foreach (const QVariant &generalId, candidateData) {
        uint id = generalId.toUInt();
        const General *general = engine->getGeneral(id);
        if (general)
            generals << general;
    }

    //@to-do: parse banned pairs

    emit client->chooseGeneralRequested(generals);
}

void Client::MoveCardsCommand(Client *client, const QVariant &data)
{
    QList<CardsMoveStruct> moves;

    QVariantList movesData = data.toList();
    foreach (const QVariant &moveVar, movesData) {
        const QVariantMap moveData = moveVar.toMap();
        const QVariantMap from = moveData["from"].toMap();
        const QVariantMap to = moveData["to"].toMap();

        CardsMoveStruct move;
        move.from.type = static_cast<CardArea::Type>(from["type"].toInt());
        move.from.direction = static_cast<CardArea::Direction>(from["direction"].toInt());
        move.from.name = from["name"].toString();
        move.from.owner = client->findPlayer(from["ownerId"].toUInt());
        move.to.type = static_cast<CardArea::Type>(to["type"].toInt());
        move.to.direction = static_cast<CardArea::Direction>(from["direction"].toInt());
        move.to.name = to["name"].toString();
        move.to.owner = client->findPlayer(to["ownerId"].toInt());
        move.isOpen = to["isOpen"].toBool();
        move.isLastHandCard = to["isLastHandCard"].toBool();
        move.cards = client->findCards(moveData["cards"]);

        CardArea *source = client->findArea(move.from);
        CardArea *destination = client->findArea(move.to);
        if (source)
            source->remove(move.cards);
        if (destination)
            destination->add(move.cards);

        moves << move;
    }

    emit client->cardsMoved(moves);
}

void Client::UseCardRequestCommand(Client *client, const QVariant &data)
{
    const QVariantMap arg = data.toMap();
    if (arg.size() != 3) {
        emit client->usingCard(QString(), QList<const Player *>());
    } else {
        QString pattern = arg["pattern"].toString();
        QList<const Player *> targets;
        QVariantList dataList = arg["assignedTargets"].toList();
        foreach (const QVariant &data, dataList) {
            const ClientPlayer *target = client->findPlayer(data.toUInt());
            if (target)
                targets << target;
        }
        emit client->usingCard(pattern, targets);
    }
}

void Client::UseCardCommand(Client *client, const QVariant &data)
{
    const QVariantMap args = data.toMap();
    const ClientPlayer *from = client->findPlayer(args["from"].toUInt());
    QList<const ClientPlayer *> targets = client->findPlayers(args["to"]);

    //@to-do: set card footnote
    emit client->cardUsed(from, targets);
}

void Client::AddCardHistoryCommand(Client *client, const QVariant &data)
{
    ClientPlayer *self = client->m_user2player.value(client->self());

    if (data.isNull()) {
        self->clearCardHistory();
    } else {
        QVariantList dataList = data.toList();
        if (dataList.size() != 2)
            return;

        QString name = dataList.at(0).toString();
        int times = dataList.at(1).toInt();
        self->addCardHistory(name, times);
    }
}

void Client::DamageCommand(Client *client, const QVariant &data)
{
    QVariantList dataList = data.toList();
    if (dataList.length() != 3)
        return;

    ClientPlayer *victim = client->findPlayer(dataList.at(0).toUInt());
    DamageStruct::Nature nature = static_cast<DamageStruct::Nature>(dataList.at(1).toInt());
    int damage = dataList.at(2).toInt();
    emit client->damageDone(victim, nature, damage);
}

void Client::LoseHpCommand(Client *client, const QVariant &data)
{
    const QVariantMap arg = data.toMap();
    if (!arg.contains("victimId") || !arg.contains("loseHp"))
        return;

    uint id = arg["victimId"].toUInt();
    const ClientPlayer *victim = client->findPlayer(id);
    if (victim == nullptr)
        return;

    int loseHp = arg["loseHp"].toInt();
    emit client->loseHpDone(victim, loseHp);
}

void Client::RecoverCommand(Client *client, const QVariant &data)
{
    QVariantMap arg = data.toMap();
    if (!arg.contains("from") || !arg.contains("to") || !arg.contains("num"))
        return;

    const ClientPlayer *from = client->findPlayer(arg["from"].toUInt());
    const ClientPlayer *to = client->findPlayer(arg["to"].toUInt());
    int num = arg["num"].toInt();
    emit client->recoverDone(from, to, num);
}

void Client::AskForCardRequestCommand(Client *client, const QVariant &data)
{
    const QVariantMap arg = data.toMap();
    if (!arg.contains("pattern"))
        return;

    QString pattern = arg["pattern"].toString();

    if (arg.contains("minNum") && arg.contains("maxNum")) {
        int minNum = arg["minNum"].toInt();
        int maxNum = arg["maxNum"].toInt();
        int optional = arg["optional"].toBool();
        emit client->cardsAsked(pattern, minNum, maxNum, optional);
    } else {
        emit client->cardAsked(pattern);
    }
}

void Client::ShowAmazingGraceCommand(Client *client, const QVariant &)
{
    emit client->amazingGraceStarted();
}

void Client::ClearAmazingGraceCommand(Client *client, const QVariant &)
{
    emit client->amazingGraceFinished();
}

void Client::TakeAmazingGraceRequestCommand(Client *client, const QVariant &)
{
    emit client->amazingGraceRequested();
}

void Client::ChoosePlayerCardRequestCommand(Client *client, const QVariant &data)
{
    const QVariantMap args = data.toMap();
    QList<Card *> handcards = client->findCards(args["handcards"]);
    QList<Card *> equips = client->findCards(args["equips"]);
    QList<Card *> delayedTricks = client->findCards(args["delayedTricks"]);
    emit client->choosePlayerCardRequested(handcards, equips, delayedTricks);
}

void Client::ShowCardCommand(Client *client, const QVariant &data)
{
    const QVariantMap arg = data.toMap();
    if (arg.isEmpty())
        return;

    const ClientPlayer *from = client->findPlayer(arg["from"].toUInt());
    if (from == nullptr)
        return;

    QVariantList dataList = arg["cards"].toList();
    QList<const Card *> cards;
    foreach (const QVariant &var, dataList) {
        const Card *card = client->findCard(var.toUInt());
        if (card)
            cards << card;
    }

    emit client->cardShown(from, cards);
}

void Client::AddSkillCommand(Client *client, const QVariant &data)
{
    const QVariantMap arg = data.toMap();
    if (arg.isEmpty())
        return;

    ClientPlayer *player = client->findPlayer(arg["playerId"].toUInt());
    if (player == nullptr)
        return;

    Engine *engine = Engine::instance();
    const Skill *skill = engine->getSkill(arg["skillId"].toUInt());
    if (skill == nullptr)
        return;

    Player::SkillArea type = static_cast<Player::SkillArea>(arg["skillArea"].toInt());
    player->addSkill(skill, type);

    bool log = arg["log"].toBool();
    if (log) {
        //@to-do: add log message
    }
}

void Client::RemoveSkillCommand(Client *client, const QVariant &data)
{
    const QVariantMap arg = data.toMap();
    if (arg.isEmpty())
        return;

    ClientPlayer *player = client->findPlayer(arg["playerId"].toUInt());
    if (player == nullptr)
        return;

    Engine *engine = Engine::instance();
    const Skill *skill = engine->getSkill(arg["skillId"].toUInt());
    if (skill == nullptr)
        return;

    Player::SkillArea type = static_cast<Player::SkillArea>(arg["skillArea"].toInt());
    player->removeSkill(skill, type);

    bool log = arg["log"].toBool();
    if (log) {
        //@to-do: add log message
    }
}

void Client::TriggerOrderCommand(Client *client, const QVariant &data)
{
    const QVariantMap arg = data.toMap();
    if (arg.isEmpty())
        return;

    QStringList options;
    QVariantList optionData = arg["options"].toList();
    foreach (const QVariant &option, optionData) {
        const QVariantMap &optionMap = option.toMap();
        options << optionMap["name"].toString();
        //@to-do: check optionMap["to"] and display the corresponding avatar
    }

    if (arg["cancelable"].toBool())
        options << "cancel";

    //@to-do: customize a new signal for trigger order
    emit client->optionRequested(options);
}

void Client::ArrangeCardCommand(Client *client, const QVariant &data)
{
    const QVariantMap args = data.toMap();

    const QVariantList capacityData = args.value("capacities").toList();
    QList<int> capacities;
    foreach (const QVariant &capacity, capacityData)
        capacities << capacity.toInt();

    QVariantList areaNameData = args.value("areaNames").toList();
    QStringList areaNames;
    foreach (const QVariant &name, areaNameData)
        areaNames << name.toString();

    QList<Card *> cards = client->findCards(args.value("cards"));

    emit client->arrangeCardRequested(cards, capacities, areaNames);
}

void Client::ArrangeCardStartCommand(Client *client, const QVariant &data)
{
    C_UNUSED(client);
    C_UNUSED(data);
}

void Client::ArrangeCardMoveCommand(Client *client, const QVariant &data)
{
    C_UNUSED(client);
    C_UNUSED(data);
}

void Client::ArrangeCardEndCommand(Client *client, const QVariant &data)
{
    C_UNUSED(client);
    C_UNUSED(data);
}

void Client::InvokeSkillCommand(Client *client, const QVariant &data)
{
    const QVariantMap args = data.toMap();
    if (!args.contains("invokerId") || !args.contains("skillId"))
        return;

    uint invokerId = args.value("invokerId").toUInt();
    ClientPlayer *invoker = client->findPlayer(invokerId);
    if (invoker == nullptr)
        return;

    uint skillId = args.value("skillId").toUInt();
    const Skill *skill = Engine::instance()->getSkill(skillId);
    if (skill == nullptr)
        return;

    invoker->addSkillHistory(skill);

    QList<Card *> cards;
    if (args.contains("cards"))
        cards = client->findCards(args.value("cards"));

    QList<const ClientPlayer *> targets;
    if (args.contains("targets"))
        targets = client->findPlayers(args.value("targets"));

    emit client->skillInvoked(invoker, skill, cards, targets);
}

void Client::ClearSkillHistoryCommand(Client *client, const QVariant &data)
{
    uint id = data.toUInt();
    ClientPlayer *player = client->findPlayer(id);
    if (player)
        player->clearSkillHistory();
}

void Client::AskForOptionCommand(Client *client, const QVariant &data)
{
    QStringList options = data.toStringList();
    emit client->optionRequested(options);
}

static QObject *ClientInstanceCallback(QQmlEngine *, QJSEngine *)
{
    return Client::instance();
}

void Client::Init()
{
    qmlRegisterSingletonType<Client>("Sanguosha", 1, 0, "Client", ClientInstanceCallback);

    AddCallback(S_COMMAND_SHOW_PROMPT, ShowPromptCommand);
    AddCallback(S_COMMAND_ARRANGE_SEAT, ArrangeSeatCommand);
    AddCallback(S_COMMAND_PREPARE_CARDS, PrepareCardsCommand);
    AddCallback(S_COMMAND_UPDATE_PLAYER_PROPERTY, UpdatePlayerPropertyCommand);
    AddCallback(S_COMMAND_MOVE_CARDS, MoveCardsCommand);
    AddCallback(S_COMMAND_ADD_CARD_HISTORY, AddCardHistoryCommand);
    AddCallback(S_COMMAND_DAMAGE, DamageCommand);
    AddCallback(S_COMMAND_LOSE_HP, LoseHpCommand);
    AddCallback(S_COMMAND_RECOVER, RecoverCommand);
    AddCallback(S_COMMAND_USE_CARD, UseCardCommand);
    AddCallback(S_COMMAND_SHOW_AMAZING_GRACE, ShowAmazingGraceCommand);
    AddCallback(S_COMMAND_CLEAR_AMAZING_GRACE, ClearAmazingGraceCommand);
    AddCallback(S_COMMAND_SHOW_CARD, ShowCardCommand);
    AddCallback(S_COMMAND_ADD_SKILL, AddSkillCommand);
    AddCallback(S_COMMAND_REMOVE_SKILL, RemoveSkillCommand);
    AddCallback(S_COMMAND_ARRANGE_CARD_START, ArrangeCardStartCommand);
    AddCallback(S_COMMAND_ARRANGE_CARD_MOVE, ArrangeCardMoveCommand);
    AddCallback(S_COMMAND_ARRANGE_CARD_END, ArrangeCardEndCommand);
    AddCallback(S_COMMAND_INVOKE_SKILL, InvokeSkillCommand);
    AddCallback(S_COMMAND_CLEAR_SKILL_HISTORY, ClearSkillHistoryCommand);

    AddInteraction(S_COMMAND_CHOOSE_GENERAL, ChooseGeneralRequestCommand);
    AddInteraction(S_COMMAND_USE_CARD, UseCardRequestCommand);
    AddInteraction(S_COMMAND_ASK_FOR_CARD, AskForCardRequestCommand);
    AddInteraction(S_COMMAND_TAKE_AMAZING_GRACE, TakeAmazingGraceRequestCommand);
    AddInteraction(S_COMMAND_CHOOSE_PLAYER_CARD, ChoosePlayerCardRequestCommand);
    AddInteraction(S_COMMAND_TRIGGER_ORDER, TriggerOrderCommand);
    AddInteraction(S_COMMAND_ARRANGE_CARD, ArrangeCardCommand);
    AddInteraction(S_COMMAND_ASK_FOR_OPTION, AskForOptionCommand);
}
C_INITIALIZE_CLASS(Client)
