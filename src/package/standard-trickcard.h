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
    void onEffect(GameLogic *logic, CardEffectStruct &effect) override;

private:
    void clearRestCards(GameLogic *logic) const;
};

class GodSalvation : public GlobalEffect
{
    Q_OBJECT

public:
    Q_INVOKABLE GodSalvation(Suit suit, int number);

    void onEffect(GameLogic *logic, CardEffectStruct &effect) override;
    bool isNullifiable(const CardEffectStruct &effect) const override;
};

#endif // STANDARDTRICKCARD_H
