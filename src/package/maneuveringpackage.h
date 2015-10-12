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


#ifndef MANEUVERINGPACKAGE_H
#define MANEUVERINGPACKAGE_H

#include "standard-basiccard.h"
#include "package.h"

class FireSlash : public Slash
{
    Q_OBJECT

public:
    Q_INVOKABLE FireSlash(Suit suit, int number);
};

class ThunderSlash : public Slash
{
    Q_OBJECT

public:
    Q_INVOKABLE ThunderSlash(Suit suit, int number);
};

class Analeptic : public BasicCard
{
    Q_OBJECT

public:
    Q_INVOKABLE Analeptic(Suit suit, int number);

    void onUse(GameLogic *logic, CardUseStruct &use) override;
    void effect(GameLogic *logic, CardEffectStruct &effect) override;
    bool isAvailable(const Player *player) const override;
};

class Fan : public Weapon
{
    Q_OBJECT

public:
    Q_INVOKABLE Fan(Suit suit = Diamond, int number = 1);
};

class GudingBlade : public Weapon
{
    Q_OBJECT

public:
    Q_INVOKABLE GudingBlade(Suit = Spade, int number = 1);
};

class Vine : public Armor
{
    Q_OBJECT

public:
    Q_INVOKABLE Vine(Suit suit = Club, int number = 2);
};

class SilverLion : public Armor
{
    Q_OBJECT

public:
    Q_INVOKABLE SilverLion(Suit suit = Club, int number = 1);
};

class SupplyShortage : public DelayedTrick
{
    Q_OBJECT

public:
    Q_INVOKABLE SupplyShortage(Suit suit, int number);

    void takeEffect(GameLogic *, CardEffectStruct &effect) override;
};

class IronChain : public TrickCard
{
    Q_OBJECT

public:
    Q_INVOKABLE IronChain(Suit suit, int number);

    void effect(GameLogic *, CardEffectStruct &effect) override;
};

class FireAttack : public SingleTargetTrick
{
    Q_OBJECT

public:
    Q_INVOKABLE FireAttack(Suit suit, int number);

    bool targetFilter(const QList<const Player *> &targets, const Player *toSelect, const Player *self) const override;
    void effect(GameLogic *logic, CardEffectStruct &effect) override;
};

class ManeuveringPackage : public Package
{
public:
    ManeuveringPackage();
};

#endif // MANEUVERINGPACKAGE_H
