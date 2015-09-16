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

Player::Player(QObject *parent)
    : CAbstractPlayer(parent)
    , m_hp(0)
    , m_maxHp(0)
    , m_alive(true)
    , m_headGeneral(nullptr)
    , m_deputyGeneral(nullptr)
    , m_turnCount(0)
{
    m_handCards = new CardArea(CardArea::Hand, this);
    m_handCards->setSignal([this](){
        emit handCardNumChanged();
    });
    m_equips = new CardArea(CardArea::Equip, this);
    m_handCards->setSignal([this](){
        emit equipNumChanged();
    });
    m_delayedTricks = new CardArea(CardArea::DelayedTrick, this);
    m_handCards->setSignal([this](){
        emit delayedTrickNumChanged();
    });
    m_judgeCards = new CardArea(CardArea::Judge, this);
}

Player::~Player()
{
    delete m_handCards;
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

QString Player::headGeneralName() const
{
    return m_headGeneral ? m_headGeneral->name() : "";
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

int Player::handcardNum() const
{
    return m_handCards->length();
}

int Player::equipNum() const
{
    return m_equips->length();
}

int Player::delayedTrickNum() const
{
    return m_delayedTricks->length();
}

void Player::addCardHistory(const QString &name, int times)
{
    if (m_cardHistory.contains(name))
        m_cardHistory[name] += times;
    else
        m_cardHistory[name] = times;
}

void Player::setDrank(int drank)
{
    m_drank = drank;
    emit drankChanged();
}
