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
#include "eventtype.h"
#include "gamelogic.h"
#include "serverplayer.h"
#include "skill.h"

Card::Card(Suit suit, int number)
    : m_id(0)
    , m_suit(suit)
    , m_number(number)
    , m_color(UndeterminedColor)
    , m_type(InvalidType)
    , m_subtype(0)
    , m_transferable(false)
    , m_canRecast(false)
    , m_useLimit(InfinityNum)
    , m_maxTargetNum(1)
    , m_minTargetNum(1)
    , m_distanceLimit(InfinityNum)
    , m_targetFixed(false)
    , m_skill(nullptr)
{
}

Card *Card::clone() const
{
    const QMetaObject *metaObject = this->metaObject();
    Card *card = qobject_cast<Card *>(metaObject->newInstance(Q_ARG(Suit, suit()), Q_ARG(int, number())));
    card->m_id = m_id;
    return card;
}

uint Card::effectiveId() const
{
    if (!isVirtual())
        return m_id;

    if (m_subcards.length() == 1)
        return m_subcards.first()->effectiveId();

    return 0;
}

Card::Suit Card::suit() const
{
    if (m_suit == UndeterminedSuit) {
        if (m_subcards.length() == 1)
            return m_subcards.first()->suit();
        else
            return NoSuit;
    } else
        return m_suit;
}

void Card::setSuitString(const QString &suit)
{
    if (suit == "spade")
        setSuit(Spade);
    else if (suit == "heart")
        setSuit(Heart);
    else if (suit == "club")
        setSuit(Club);
    else if (suit == "diamond")
        setSuit(Diamond);
    else
        setSuit(NoSuit);
}

QString Card::suitString() const
{
    Suit s = suit();
    if (s == Spade)
        return "spade";
    else if (s == Heart)
        return "heart";
    else if (s == Club)
        return "club";
    else if (s == Diamond)
        return "diamond";
    else
        return "no_suit";
}

int Card::number() const
{
    if (m_number >= 0)
        return m_number;

    int number = 0;
    foreach (const Card *card, m_subcards)
        number += card->number();
    return number >= 13 ? 13 : number;
}

Card::Color Card::color() const
{
    if (m_color == UndeterminedColor) {
        Suit s = suit();
        if (s != NoSuit)
            return (s == Spade || s == Club) ? Black : Red;
        else
            return NoColor;
    } else
        return m_color;
}

void Card::setColorString(const QString &color)
{
    if (color == "black")
        setColor(Black);
    else if (color == "red")
        setColor(Red);
    else
        setColor(NoColor);
}

QString Card::colorString() const
{
    Color c = color();
    if (c == Black)
        return "black";
    else if (c == Red)
        return "red";
    else
        return "no_color";
}

QString Card::typeString() const
{
    if (m_type == BasicType)
        return "basic";
    else if (m_type == TrickType)
        return "trick";
    else if (m_type == EquipType)
        return "equip";
    else
        return "skill";
}

void Card::addSubcard(Card *card)
{
    m_subcards << card;
}

Card *Card::realCard()
{
    if (id() > 0)
        return this;

    if (m_subcards.length() == 1)
        return m_subcards.first()->realCard();

    return nullptr;
}

const Card *Card::realCard() const
{
    if (id() > 0)
        return this;

    if (m_subcards.length() == 1)
        return m_subcards.first()->realCard();

    return nullptr;
}

QList<Card *> Card::realCards()
{
    QList<Card *> cards;
    if (id() > 0) {
        cards << this;
    } else {
        foreach (Card *card, m_subcards)
            cards << card->realCards();
    }
    return cards;
}

QList<const Card *> Card::realCards() const
{
    QList<const Card *> cards;
    if (id() > 0) {
        cards << this;
    } else {
        foreach (const Card *card, m_subcards)
            cards << card->realCards();
    }
    return cards;
}

int Card::useLimit() const
{
    return m_useLimit;
}

int Card::useLimit(const Player *source) const
{
    int limit = useLimit();
    QList<const Skill *> skills = source->getGlobalSkills();
    foreach (const Skill *skill, skills) {
        if (skill->type() == Skill::CardModType) {
            const CardModSkill *modSkill = static_cast<const CardModSkill *>(skill);
            limit += modSkill->extraUseNum(this, source);
        }
    }
    return limit;
}

int Card::maxTargetNum() const
{
    return m_maxTargetNum;
}

int Card::minTargetNum() const
{
    return m_minTargetNum;
}

int Card::distanceLimit() const
{
    return m_distanceLimit;
}

bool Card::targetFeasible(const QList<const Player *> &selected, const Player *source) const
{
    C_UNUSED(source);
    return minTargetNum() <= selected.length() && selected.length() <= maxTargetNum();
}

bool Card::targetFilter(const QList<const Player *> &selected, const Player *toSelect, const Player *source) const
{
    int distanceLimit = this->distanceLimit();
    int maxTargetNum = this->maxTargetNum();
    bool isValid = toSelect->isAlive();

    QList<const Skill *> skills = toSelect->getGlobalSkills();
    foreach (const Skill *skill, skills) {
        if (skill->type() == Skill::CardModType) {
            const CardModSkill *modSkill = static_cast<const CardModSkill *>(skill);
            isValid = modSkill->targetFilter(this, selected, toSelect, source);
            if (!isValid)
                return false;

            distanceLimit += modSkill->extraDistanceLimit(this, selected, toSelect, source);
            maxTargetNum += modSkill->extraMaxTargetNum(this, selected, toSelect, source);
        }
    }

    return isValid && selected.length() < maxTargetNum && (source == nullptr || source->distanceTo(toSelect) <= distanceLimit);
}

bool Card::isAvailable(const Player *source) const
{
    int limit = useLimit(source);
    return source->cardHistory(objectName()) < limit;
}

bool Card::isValid(const QList<ServerPlayer *> &targets, ServerPlayer *source) const
{
    if (isTargetFixed())
        return targets.isEmpty();

    QList<const Player *> selected;
    foreach (const Player *toSelect, targets) {
        if (targetFilter(selected, toSelect, source))
            selected << toSelect;
        else
            return false;
    }
    return targetFeasible(selected, source);
}

bool Card::isValid(const QList<const Player *> &targets, const Player *source) const
{
    if (isTargetFixed())
        return targets.isEmpty();

    QList<const Player *> selected;
    foreach (const Player *toSelect, targets) {
        if (targetFilter(selected, toSelect, source))
            selected << toSelect;
        else
            return false;
    }
    return targetFeasible(selected, source);
}

void Card::onUse(GameLogic *logic, CardUseValue &use)
{
    logic->sortByActionOrder(use.to);

    logic->trigger(PreCardUsed, use.from, &use);

    CardsMoveValue moves;
    CardMove move;
    move.toArea = logic->table();
    move.card = use.card->makeVirtual();
    move.isOpen = true;
    moves.moves << move;
    logic->moveCards(moves);
}

void Card::use(GameLogic *logic, CardUseValue &use)
{
    foreach (ServerPlayer *target, use.to) {
        CardEffectValue effect(use);
        effect.to = target;
        logic->takeCardEffect(effect);
    }

    if (use.target) {
        CardEffectValue effect(use);
        logic->takeCardEffect(effect);
    }

    complete(logic);
}

void Card::onEffect(GameLogic *, CardEffectValue &)
{
}

void Card::effect(GameLogic *, CardEffectValue &)
{
}

void Card::complete(GameLogic *logic)
{
    const CardArea *table = logic->table();
    if (table->contains(realCard())) {
        CardsMoveValue moves;
        CardMove move;
        move.card = makeVirtual();
        move.toArea = logic->discardPile();
        move.isOpen = true;
        moves.moves << move;
        logic->moveCards(moves);
    }
}

Card *Card::Find(const QList<Card *> &cards, uint id)
{
    foreach (Card *card, cards) {
        if (card->id() == id)
            return card;
    }
    return nullptr;
}

QList<Card *> Card::Find(const QList<Card *> &cards, const QVariant &data)
{
    QList<Card *> result;
    QVariantList targets = data.toList();
    foreach(const QVariant &target, targets) {
        foreach (Card *card, cards) {
            if (card->id() == target.toUInt())
                result << card;
        }
    }
    return result;
}

QVariantMap Card::toVariant() const
{
    QVariantMap data;
    data["name"] = objectName();
    data["suit"] = suitString();
    data["number"] = number();
    data["color"] = color();
    return data;
}

Card *Card::makeVirtual()
{
    if (isVirtual())
        return this;

    const QMetaObject *meta = metaObject();
    Card *card = qobject_cast<Card *>(meta->newInstance(Q_ARG(Card::Suit, suit()), Q_ARG(int, number())));
    if (card == nullptr)
        return nullptr;

    card->addSubcard(this);
    card->deleteLater(); // do not delete from other process of the game logic
    return card;
}

BasicCard::BasicCard(Card::Suit suit, int number)
    : Card(suit, number)
{
    m_type = BasicType;
}


TrickCard::TrickCard(Card::Suit suit, int number)
    : Card(suit, number)
{
    m_type = TrickType;
}

void TrickCard::onEffect(GameLogic *logic, CardEffectValue &effect)
{
    if (isNullifiable(effect)) {
        QList<ServerPlayer *> players = logic->allPlayers();
        //@to-do: do not ask if no player can use nullification
        foreach (ServerPlayer *player, players) {
            if (effect.from) {
                if (effect.to)
                    player->showPrompt("trick-nullification-1", effect.from, effect.to, effect.use.card);
                else
                    player->showPrompt("trick-nullification-2", effect.from, effect.use.card);
            } else if (effect.to) {
                player->showPrompt("trick-nullification-3", effect.to, effect.use.card);
            } else {
                player->showPrompt("trick-nullification-4", effect.use.card);
            }
            Card *card = player->askForCard("Nullification"); // @to-do: Takashiro: the ask of Nullification is actually a race request(according to the old framework)
            if (card) {
                CardUseValue use;
                use.from = player;
                use.card = card;
                use.target = effect.use.card;
                use.extra = QVariant::fromValue(&effect);
                logic->useCard(use);
                break;
            }
        }
    }
}

bool TrickCard::isNullifiable(const CardEffectValue &) const
{
    return true;
}

EquipCard::EquipCard(Card::Suit suit, int number)
    : Card(suit, number)
    , m_skill(nullptr)
{
    m_type = EquipType;
    m_targetFixed = true;
}

void EquipCard::onUse(GameLogic *logic, CardUseValue &use)
{
    ServerPlayer *player = use.from;
    if (use.to.isEmpty())
        use.to << player;

    logic->trigger(PreCardUsed, player, &use);
}

void EquipCard::use(GameLogic *logic, CardUseValue &use)
{
    if (use.to.isEmpty()) {
        CardsMoveValue moves;
        CardMove move;
        move.card = this;
        move.toArea = logic->discardPile();
        move.isOpen = true;
        moves.moves << move;
        logic->moveCards(moves);
        return;
    }

    ServerPlayer *target = use.to.first();

    //Find the existing equip
    Card *equippedCard = nullptr;
    QList<Card *> equips = target->equipArea()->cards();
    foreach (Card *card, equips) {
        if (card->subtype() == subtype()) {
            equippedCard = card;
            break;
        }
    }

    CardsMoveValue moves;

    CardMove install;
    install.card = makeVirtual();
    install.toArea = target->equipArea(); // we should make a virtual card of the equip card
    install.isOpen = true;
    moves.moves << install;

    if (equippedCard != nullptr) {
        CardMove uninstall;
        uninstall.card = equippedCard;
        uninstall.toArea = logic->table();
        uninstall.isOpen = true;
        moves.moves << uninstall;
    }
    logic->moveCards(moves);

    if (equippedCard != nullptr) {
        const CardArea *table = logic->table();
        if (table->contains(equippedCard)) {
            CardsMoveValue discards;
            CardMove discard;
            discard.card = equippedCard;
            discard.toArea = logic->discardPile();
            discard.isOpen = true;
            discards.moves << discard;
            logic->moveCards(discards);
        }
    }
}

void EquipCard::complete(GameLogic *)
{
}

GlobalEffect::GlobalEffect(Card::Suit suit, int number)
    : TrickCard(suit, number)
{
    m_targetFixed = true;
    m_subtype = GlobalEffectType;
    m_maxTargetNum = InfinityNum;
}

void GlobalEffect::onUse(GameLogic *logic, CardUseValue &use)
{
    if (use.to.isEmpty()) {
        QList<const Player *> selected;
        QList<ServerPlayer *> targets = logic->allPlayers();
        foreach (ServerPlayer *toSelect, targets) {
            if (targetFilter(selected, toSelect, use.from)) {
                selected << toSelect;
                use.to << toSelect;
            }
        }
    }
    TrickCard::onUse(logic, use);
}

AreaOfEffect::AreaOfEffect(Card::Suit suit, int number)
    : TrickCard(suit, number)
{
    m_targetFixed = true;
    m_subtype = AreaOfEffectType;
    m_maxTargetNum = InfinityNum;
}

void AreaOfEffect::onUse(GameLogic *logic, CardUseValue &use)
{
    if (use.to.isEmpty()) {
        QList<const Player *> selected;
        QList<ServerPlayer *> targets = logic->otherPlayers(use.from);
        foreach (ServerPlayer *toSelect, targets) {
            if (targetFilter(selected, toSelect, use.from)) {
                selected << toSelect;
                use.to << toSelect;
            }
        }
    }
    TrickCard::onUse(logic, use);
}

SingleTargetTrick::SingleTargetTrick(Card::Suit suit, int number)
    : TrickCard(suit, number)
{
    m_subtype = SingleTargetType;
}

DelayedTrick::DelayedTrick(Card::Suit suit, int number)
    : TrickCard(suit, number)
{
    m_subtype = DelayedType;
}

bool DelayedTrick::targetFeasible(const QList<const Player *> &selected, const Player *) const
{
    return selected.length() == 1;
}

bool DelayedTrick::targetFilter(const QList<const Player *> &selected, const Player *toSelect, const Player *source) const
{
    if (!selected.isEmpty() || toSelect == source)
        return false;

    if (!TrickCard::targetFilter(selected, toSelect, source))
        return false;

    const CardArea *area = toSelect->delayedTrickArea();
    return area->length() <= 0 || !area->contains(metaObject()->className());
}

void DelayedTrick::onUse(GameLogic *logic, CardUseValue &use)
{
    logic->sortByActionOrder(use.to);

    logic->trigger(PreCardUsed, use.from, &use);
}

void DelayedTrick::use(GameLogic *logic, CardUseValue &use)
{
    CardsMoveValue moves;
    CardMove move;

    move.card = use.card->makeVirtual();
    move.isOpen = true;
    if (use.to.isEmpty())
        move.toArea = logic->discardPile();
    else
        move.toArea = use.to.first()->delayedTrickArea();

    moves.moves << move;
    logic->moveCards(moves);
}

void DelayedTrick::onEffect(GameLogic *logic, CardEffectValue &effect)
{
    CardsMoveValue moves;
    CardMove move;

    move.card = effect.use.card->makeVirtual();
    move.isOpen = true;
    move.toArea = logic->table();

    moves.moves << move;
    logic->moveCards(moves);

    TrickCard::onEffect(logic, effect);
}

void DelayedTrick::effect(GameLogic *logic, CardEffectValue &effect)
{
    JudgeValue judge(m_judgePattern);
    judge.who = effect.to;
    logic->judge(judge);

    if (judge.matched)
        takeEffect(logic, effect);
}

MovableDelayedTrick::MovableDelayedTrick(Card::Suit suit, int number)
    : DelayedTrick(suit, number)
{
    m_targetFixed = true;
}

void MovableDelayedTrick::onUse(GameLogic *logic, CardUseValue &use)
{
    if (use.to.isEmpty())
        use.to << use.from;
    DelayedTrick::onUse(logic, use);
}

void MovableDelayedTrick::effect(GameLogic *logic, CardEffectValue &effect)
{
    JudgeValue judge(m_judgePattern);
    judge.who = effect.to;
    logic->judge(judge);

    if (judge.matched) {
        takeEffect(logic, effect);
        const CardArea *table = logic->table();
        if (table->contains(this)) {
            CardsMoveValue moves;
            CardMove move;
            move.card = this;
            move.toArea = logic->discardPile();
            move.isOpen = true;
            moves.moves << move;
            logic->moveCards(moves);
        }
    }
}

void MovableDelayedTrick::complete(GameLogic *logic)
{
    const CardArea *table = logic->table();
    if (!table->contains(this))
        return;

    ServerPlayer *current = logic->currentPlayer();
    ServerPlayer *target = logic->currentPlayer();
    forever {
        target = target->nextAlive();
        if (!targetFilter(QList<const Player *>(), target, nullptr) && target != current)
            continue;

        CardsMoveValue moves;
        CardMove move;
        move.card = makeVirtual();
        move.toArea = target->delayedTrickArea();
        move.isOpen = true;
        moves.moves << move;
        logic->moveCards(moves);

        CardUseValue use;
        use.card = this;
        use.to << target;

        foreach (ServerPlayer *to, use.to)
            logic->trigger(TargetConfirming, to, &use);
        if (use.to.isEmpty())
            continue;
        foreach (ServerPlayer *to, use.to)
            logic->trigger(TargetConfirmed, to, &use);
        if (!use.to.isEmpty())
            break;
    }
}

bool MovableDelayedTrick::isAvailable(const Player *player) const
{
    const char *className = metaObject()->className();
    QList<Card *> cards = player->delayedTrickArea()->cards();
    foreach (const Card *card, cards) {
        if (card->inherits(className))
            return false;
    }
    return DelayedTrick::isAvailable(player);
}

Weapon::Weapon(Card::Suit suit, int number)
    : EquipCard(suit, number)
    , m_attackRange(0)
{
    m_subtype = WeaponType;
}


Armor::Armor(Card::Suit suit, int number)
    : EquipCard(suit, number)
{
    m_subtype = ArmorType;
}


Horse::Horse(Card::Suit suit, int number)
    : EquipCard(suit, number)
{
}

Card *Horse::clone() const
{
    Card *card = Card::clone();
    card->setObjectName(objectName());
    return card;
}

Card *Horse::makeVirtual()
{
    Card *card = Card::makeVirtual();
    if (card == this)
        return card;

    card->setObjectName(objectName());
    return card;
}

OffensiveHorse::OffensiveHorse(Card::Suit suit, int number)
    : Horse(suit, number)
    , m_extraOutDistance(-1)
{
    m_subtype = OffensiveHorseType;
}

DefensiveHorse::DefensiveHorse(Card::Suit suit, int number)
    : Horse(suit, number)
    , m_extraInDistance(+1)
{
    m_subtype = DefensiveHorseType;
}

Treasure::Treasure(Card::Suit suit, int number)
    : EquipCard(suit, number)
{
    m_subtype = TreasureType;
}
