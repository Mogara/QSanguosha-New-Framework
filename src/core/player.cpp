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

#include "player.h"

Player::Player(QObject *parent)
    : CAbstractPlayer(parent)
    , m_hp(0)
    , m_maxHp(0)
    , m_alive(true)
{

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

bool Player::hasSkill(const EventHandler *skill) const
{
    Q_UNUSED(skill)
    return false;
}

bool Player::hasShownSkill(const EventHandler *skill) const
{
    //@todo:
    return false;
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
