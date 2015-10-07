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

#ifndef STANDARDTRICKCARD_H
#define STANDARDTRICKCARD_H

#include "card.h"

class GameLogic;

class AmazingGrace : public GlobalEffect
{
    Q_OBJECT

public:
    Q_INVOKABLE AmazingGrace(Suit suit, int number);

    void onUse(GameLogic *logic, CardUseStruct &use) override;
    void use(GameLogic *logic, CardUseStruct &use) override;
    void effect(GameLogic *logic, CardEffectStruct &effect) override;

private:
    void clearRestCards(GameLogic *logic) const;
};

class GodSalvation : public GlobalEffect
{
    Q_OBJECT

public:
    Q_INVOKABLE GodSalvation(Suit suit, int number);

    void effect(GameLogic *logic, CardEffectStruct &effect) override;
    bool isNullifiable(const CardEffectStruct &effect) const override;
};

class SavageAssault :public AreaOfEffect
{
    Q_OBJECT

public:
    Q_INVOKABLE SavageAssault(Suit suit, int number);

    void effect(GameLogic *logic, CardEffectStruct &effect) override;
};

class ArcheryAttack : public AreaOfEffect
{
    Q_OBJECT

public:
    Q_INVOKABLE ArcheryAttack(Suit suit, int number);

    void effect(GameLogic *logic, CardEffectStruct &effect) override;
};

class ExNihilo : public SingleTargetTrick
{
    Q_OBJECT

public:
    Q_INVOKABLE ExNihilo(Suit suit, int number);

    void onUse(GameLogic *logic, CardUseStruct &use) override;
    void effect(GameLogic *, CardEffectStruct &effect) override;
};

class Duel : public SingleTargetTrick
{
    Q_OBJECT

public:
    Q_INVOKABLE Duel(Suit suit, int number);

    bool targetFilter(const QList<const Player *> &targets, const Player *toSelect, const Player *self) const override;
    void effect(GameLogic *logic, CardEffectStruct &effect) override;
};

class Snatch : public SingleTargetTrick
{
    Q_OBJECT

public:
    Q_INVOKABLE Snatch(Suit suit, int number);

    bool targetFilter(const QList<const Player *> &targets, const Player *toSelect, const Player *self) const override;
    void effect(GameLogic *logic, CardEffectStruct &effect) override;
};

class Dismantlement : public SingleTargetTrick
{
    Q_OBJECT

public:
    Q_INVOKABLE Dismantlement(Suit suit, int number);

    bool targetFilter(const QList<const Player *> &targets, const Player *toSelect, const Player *self) const override;
    void effect(GameLogic *logic, CardEffectStruct &effect) override;
};

class Collateral : public SingleTargetTrick
{
    Q_OBJECT

public:
    Q_INVOKABLE Collateral(Suit suit, int number);
    bool isAvailable(const Player *player) const override;
    bool targetFeasible(const QList<const Player *> &targets, const Player *self) const override;
    bool targetFilter(const QList<const Player *> &targets, const Player *toSelect, const Player *self) const override;
    void onUse(GameLogic *logic, CardUseStruct &use) override;
    void effect(GameLogic *logic, CardEffectStruct &effect) override;

private:
    bool doCollateral(CardEffectStruct &effect) const;

    ServerPlayer *m_victim;
};

class Nullification : public SingleTargetTrick
{
    Q_OBJECT

public:
    Q_INVOKABLE Nullification(Suit suit, int number);

    bool isAvailable(const Player *) const override;
    bool targetFeasible(const QList<const Player *> &targets, const Player *) const override;
    bool targetFilter(const QList<const Player *> &, const Player *, const Player *) const override;
    void onUse(GameLogic *logic, CardUseStruct &use);
    void effect(GameLogic *, CardEffectStruct &effect) override;
};

class Indulgence : public DelayedTrick
{
    Q_OBJECT

public:
    Q_INVOKABLE Indulgence(Suit suit, int number);

    void takeEffect(GameLogic *, CardEffectStruct &effect) override;
};

class Lightning : public MovableDelayedTrick
{
    Q_OBJECT

public:
    Q_INVOKABLE Lightning(Suit suit, int number);

    void takeEffect(GameLogic *logic, CardEffectStruct &effect) override;
};

#endif // STANDARDTRICKCARD_H
