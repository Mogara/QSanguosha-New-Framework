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

#ifndef STANDARDEQUIPCARD_H
#define STANDARDEQUIPCARD_H

#include "card.h"

class Crossbow : public Weapon
{
    Q_OBJECT

public:
    Q_INVOKABLE Crossbow(Suit suit, int number);
};

class DoubleSword : public Weapon
{
    Q_OBJECT

public:
    Q_INVOKABLE DoubleSword(Suit suit = Spade, int number = 2);
};

class QinggangSword : public Weapon
{
    Q_OBJECT

public:
    Q_INVOKABLE QinggangSword(Suit suit = Spade, int number = 6);
};

class IceSword : public Weapon
{
    Q_OBJECT

public:
    Q_INVOKABLE IceSword(Suit suit = Spade, int number = 2);
};

class SixSwords : public Weapon
{
    Q_OBJECT

public:
    Q_INVOKABLE SixSwords(Suit suit = Diamond, int number = 6);
};

class Spear : public Weapon
{
    Q_OBJECT

public:
    Q_INVOKABLE Spear(Suit suit = Spade, int number = 12);
};

class Axe : public Weapon
{
    Q_OBJECT

public:
    Q_INVOKABLE Axe(Suit suit = Diamond, int number = 5);
};

class Triblade : public Weapon
{
    Q_OBJECT

public:
    Q_INVOKABLE Triblade(Suit suit = Diamond, int number = 12);
};

class KylinBow : public Weapon
{
    Q_OBJECT

public:
    Q_INVOKABLE KylinBow(Suit suit = Heart, int number = 5);
};

class EightDiagram : public Armor
{
    Q_OBJECT

public:
    Q_INVOKABLE EightDiagram(Suit suit, int number);
};

class RenwangShield : public Armor
{
    Q_OBJECT

public:
    Q_INVOKABLE RenwangShield(Suit suit = Club, int number = 2);
};

#endif // STANDARDEQUIPCARD_H
