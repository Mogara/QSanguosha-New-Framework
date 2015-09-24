/********************************************************************
    Copyright (c) 2013-2015 - Mogara

    This file is part of Cardirector.

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

#include "cardarea.h"
#include "player.h"
#include "general.h"
#include "engine.h"

Player::Player(QObject *parent)
    : CAbstractPlayer(parent)
    , m_hp(0)
    , m_maxHp(0)
    , m_alive(true)
    , m_removed(false)
    , m_seat(0)
    , m_next(nullptr)
    , m_phase(NotActive)
    , m_turnCount(0)
    , m_faceUp(false)
    , m_handcardNum(0)
    , m_drank(false)
    , m_headGeneral(nullptr)
    , m_deputyGeneral(nullptr)
    , m_headGeneralShown(false)
    , m_deputyGeneralShown(false)
    , m_extraOutDistance(0)
    , m_extraInDistance(0)
{
    m_handcards = new CardArea(CardArea::Hand, this);
    m_handcards->setSignal([this](){
        m_handcardNum = m_handcards->length();
        emit handcardNumChanged();
    });
    m_equips = new CardArea(CardArea::Equip, this);
    m_equips->setSignal([this](){
        emit equipNumChanged();
    });
    m_delayedTricks = new CardArea(CardArea::DelayedTrick, this);
    m_delayedTricks->setSignal([this](){
        emit delayedTrickNumChanged();
    });
    m_judgeCards = new CardArea(CardArea::Judge, this);

    connect(this, &Player::headGeneralChanged, this, &Player::generalChanged);
}

Player::~Player()
{
    delete m_handcards;
    delete m_equips;
    delete m_delayedTricks;
    delete m_judgeCards;
}

void Player::setScreenName(const QString &name)
{
    m_screenName = name;
    emit screenNameChanged();
}

void Player::setHp(int hp)
{
    m_hp = hp;
    emit hpChanged();
}

void Player::setMaxHp(int maxHp)
{
    m_maxHp = maxHp;
    emit maxHpChanged();
}

void Player::setAlive(bool alive)
{
    m_alive = alive;
    emit aliveChanged();
}

bool Player::hasSkill(const EventHandler *skill) const
{
    Q_UNUSED(skill)
    return false;
}

bool Player::hasShownSkill(const EventHandler *skill) const
{
    //@todo:
    C_UNUSED(skill);
    return false;
}

void Player::setRemoved(bool removed)
{
    m_removed = removed;
    emit removedChanged();
}

void Player::setSeat(int seat)
{
    m_seat = seat;
    emit seatChanged();
}

Player *Player::next(bool ignoreRemoved) const
{
    Player *next = this->next();
    if (ignoreRemoved) {
        while (next->isRemoved())
            next = next->next();
    }
    return next;
}

Player *Player::nextAlive(int step, bool ignoreRemoved) const
{
    Player *cur = next();
    forever {
        if (cur->isAlive() && !(ignoreRemoved && cur->isRemoved())) {
            step--;
            if (step <= 0)
                break;
        }

        cur = cur->next();
        if (cur == this)
            break;
    }
    return cur;
}

void Player::setPhase(Phase phase)
{
    m_phase = phase;
    emit phaseChanged();
}

void Player::setPhaseString(const QString &phase)
{
    if (phase == "round_start")
        m_phase = RoundStart;
    else if (phase == "start")
        m_phase = Start;
    else if (phase == "judge")
        m_phase = Judge;
    else if (phase == "draw")
        m_phase = Draw;
    else if (phase == "play")
        m_phase = Play;
    else if (phase == "discard")
        m_phase = Discard;
    else if (phase == "finish")
        m_phase = Finish;
    else
        m_phase = NotActive;
    emit phaseChanged();
}

QString Player::phaseString() const
{
    switch (m_phase) {
    case RoundStart:
        return "round_start";
    case Start:
        return "start";
    case Judge:
        return "judge";
    case Draw:
        return "draw";
    case Play:
        return "play";
    case Discard:
        return "discard";
    case Finish:
        return "finish";
    default:
        return "not_active";
    }
}

QString Player::headGeneralName() const
{
    return m_headGeneral ? m_headGeneral->name() : "";
}

void Player::setHeadGeneralName(const QString &name)
{
    Engine *engine = Engine::instance();
    const General *general = engine->getGeneral(name);
    if (general)
        setHeadGeneral(general);
}

void Player::setHeadGeneral(const General *general)
{
    m_headGeneral = general;
    emit headGeneralChanged();
}

QString Player::deputyGeneralName() const
{
    return m_deputyGeneral ? m_deputyGeneral->name() : "";
}

void Player::setDeputyGeneralName(const QString &name)
{
    Engine *engine = Engine::instance();
    const General *general = engine->getGeneral(name);
    if (general)
        setDeputyGeneral(general);
}

void Player::setDeputyGeneral(const General *general)
{
    m_deputyGeneral = general;
    emit deputyGeneralChanged();
}

void Player::setFaceUp(bool faceUp)
{
    m_faceUp = faceUp;
    emit faceUpChanged();
}

int Player::equipNum() const
{
    return m_equips->length();
}

int Player::delayedTrickNum() const
{
    return m_delayedTricks->length();
}

int Player::distanceTo(const Player *other) const
{
    if (this == other)
        return 0;

    if (isRemoved() || other->isRemoved() || isDead() || other->isDead())
        return -1;

    if (m_fixedDistance.contains(other))
        return m_fixedDistance.value(other);

    const Player *next = this;
    int right = 0;
    while (next != other) {
        next = next->nextAlive();
        if (next == this)
            return -1;
        right++;
    }
    int left = 0;
    while (next != this) {
        next = next->nextAlive();
        left++;
    }

    int distance = qMin(left, right) + extraOutDistance() + other->extraInDistance();

    // keep the distance >=1
    if (distance < 1)
        distance = 1;

    return distance;
}

void Player::addCardHistory(const QString &name, int times)
{
    if (m_cardHistory.contains(name))
        m_cardHistory[name] += times;
    else
        m_cardHistory[name] = times;
}

void Player::setDrank(bool drank)
{
    m_drank = drank;
    emit drankChanged();
}

void Player::setKingdom(const QString &kingdom)
{
    m_kingdom = kingdom;
    emit kingdomChanged();
}

void Player::setRole(const QString &role)
{
    m_role = role;
    emit roleChanged();
}

void Player::setAttackRange(int range)
{
    m_attackRange = range;
    emit attackRangeChanged();
}
