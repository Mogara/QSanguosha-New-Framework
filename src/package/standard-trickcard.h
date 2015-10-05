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

class SavageAssault :public AreaOfEffect
{
    Q_OBJECT

public:
    Q_INVOKABLE SavageAssault(Suit suit, int number);

    void onEffect(GameLogic *logic, CardEffectStruct &effect) override;
};

class ArcheryAttack : public AreaOfEffect
{
    Q_OBJECT

public:
    Q_INVOKABLE ArcheryAttack(Suit suit, int number);

    void onEffect(GameLogic *logic, CardEffectStruct &effect) override;
};

class ExNihilo : public SingleTargetTrick
{
    Q_OBJECT

public:
    Q_INVOKABLE ExNihilo(Suit suit, int number);

    void onUse(GameLogic *logic, CardUseStruct &use) override;
    void onEffect(GameLogic *, CardEffectStruct &effect) override;
};

class Duel : public SingleTargetTrick
{
    Q_OBJECT

public:
    Q_INVOKABLE Duel(Suit suit, int number);

    bool targetFilter(const QList<const Player *> &targets, const Player *toSelect, const Player *self) const override;
    void onEffect(GameLogic *logic, CardEffectStruct &effect) override;
};

#endif // STANDARDTRICKCARD_H
