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
#include "skill.h"

Player::Player(QObject *parent)
    : CAbstractPlayer(parent)
    , m_hp(0)
    , m_maxHp(0)
    , m_alive(true)
    , m_removed(false)
    , m_seat(0)
    , m_next(nullptr)
    , m_phase(Inactive)
    , m_turnCount(0)
    , m_faceUp(false)
    , m_handcardNum(0)
    , m_drunk(false)
    , m_kingdom("hidden")
    , m_role("hidden")
    , m_attackRange(1)
    , m_chained(false)
    , m_dying(false)
    , m_headGeneral(nullptr)
    , m_deputyGeneral(nullptr)
    , m_headGeneralShown(false)
    , m_deputyGeneralShown(false)
    , m_extraOutDistance(0)
    , m_extraInDistance(0)
{
    m_handcardArea = new CardArea(CardArea::Hand, this);
    m_handcardArea->setSignal([this](){
        m_handcardNum = m_handcardArea->length();
        emit handcardNumChanged();
    });
    m_equipArea = new CardArea(CardArea::Equip, this);
    m_equipArea->setSignal([this](){
        emit equipNumChanged();
    });
    m_delayedTrickArea = new CardArea(CardArea::DelayedTrick, this);
    m_delayedTrickArea->setSignal([this](){
        emit delayedTrickNumChanged();
    });
    m_judgeCards = new CardArea(CardArea::Judge, this);

    connect(this, &Player::headGeneralChanged, this, &Player::generalChanged);
}

Player::~Player()
{
    delete m_handcardArea;
    delete m_equipArea;
    delete m_delayedTrickArea;
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
        m_phase = Inactive;
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
        return "inactive";
    }
}

QString Player::headGeneralName() const
{
    return m_headGeneral ? m_headGeneral->name() : "";
}

uint Player::headGeneralId() const
{
    return m_headGeneral ? m_headGeneral->id() : 0;
}

void Player::setHeadGeneralId(uint id)
{
    Engine *engine = Engine::instance();
    const General *general = engine->getGeneral(id);
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

uint Player::deputyGeneralId() const
{
    return m_deputyGeneral ? m_deputyGeneral->id() : 0;
}

void Player::setDeputyGeneralId(uint id)
{
    Engine *engine = Engine::instance();
    const General *general = engine->getGeneral(id);
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
    return m_equipArea->length();
}

int Player::delayedTrickNum() const
{
    return m_delayedTrickArea->length();
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

void Player::setDrunk(bool drunk)
{
    m_drunk = drunk;
    emit drunkChanged();
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

void Player::setChained(bool chained)
{
    m_chained = chained;
    emit chainedChanged();
}

void Player::setDying(bool dying)
{
    m_dying = dying;
    emit dyingChanged();
}


bool Player::hasSkill(const Skill *skill) const
{
    skill = skill->topSkill();
    if (m_headSkills.contains(skill))
        return true;
    if (m_deputySkills.contains(skill))
        return true;
    return m_acquiredSkills.contains(skill);
}

bool Player::hasShownSkill(const Skill *skill) const
{
    if (hasShownHeadGeneral()) {
        if (m_headSkills.contains(skill))
            return true;
    }
    if (hasShownDeputyGeneral()) {
        if (m_deputySkills.contains(skill))
            return true;
    }
    return m_acquiredSkills.contains(skill);
}

const Skill *Player::getSkill(uint id) const
{
    foreach(const Skill *skill, m_headSkills) {
        if (skill->id() == id)
            return skill;
    }

    foreach(const Skill *skill, m_deputySkills) {
        if (skill->id() == id)
            return skill;
    }

    foreach(const Skill *skill, m_acquiredSkills) {
        if (skill->id() == id)
            return skill;
    }

    return nullptr;
}

const Skill *Player::getSkill(const QString &name) const
{
    foreach(const Skill *skill, m_headSkills) {
        if (skill->name() == name)
            return skill;
    }

    foreach(const Skill *skill, m_deputySkills) {
        if (skill->name() == name)
            return skill;
    }

    foreach(const Skill *skill, m_acquiredSkills) {
        if (skill->name() == name)
            return skill;
    }

    return nullptr;
}

QList<const Skill *> Player::getGlobalSkills() const
{
    QList<const Skill *> skills = this->skills();
    const Player *current = this->nextAlive(1, false);
    while (current != this) {
        skills << current->skills();
        current = current->nextAlive(1, false);
    }
    return skills;
}

void Player::addSkill(const Skill *skill, SkillArea type)
{
    if (type == HeadSkillArea)
        addHeadSkill(skill);
    else if (type == DeputySkillArea)
        addDeputySkill(skill);
    else
        addAcquiredSkill(skill);
}

void Player::removeSkill(const Skill *skill, SkillArea type)
{
    if (type == UnknownSkillArea) {
        if (m_acquiredSkills.removeOne(skill))
            return;
        if (m_headSkills.removeOne(skill))
            return;
        m_deputySkills.removeOne(skill);
        return;
    }

    if (type == HeadSkillArea)
        m_headSkills.removeOne(skill);
    else if (type == DeputySkillArea)
        m_deputySkills.removeOne(skill);
    else
        m_acquiredSkills.removeOne(skill);
}
