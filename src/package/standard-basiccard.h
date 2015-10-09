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

#ifndef STANDARDBASICCARD_H
#define STANDARDBASICCARD_H

#include "card.h"

struct SlashEffectStruct
{
    ServerPlayer *from;
    ServerPlayer *to;

    Card *slash;
    QList<Card *> jink;

    DamageStruct::Nature nature;
    bool drunk;
    int jinkNum;

    SlashEffectStruct();
};

Q_DECLARE_METATYPE(SlashEffectStruct *)

class Slash : public BasicCard
{
    Q_OBJECT

public:
    Q_INVOKABLE Slash(Suit suit, int number);

    bool targetFeasible(const QList<const Player *> &targets, const Player *) const override;
    bool targetFilter(const QList<const Player *> &targets, const Player *toSelect, const Player *self) const override;
    void effect(GameLogic *logic, CardEffectStruct &cardEffect) override;
    bool isAvailable(const Player *player) const override;

protected:
    DamageStruct::Nature m_nature;
};

class Jink : public BasicCard
{
    Q_OBJECT

public:
    Q_INVOKABLE Jink(Suit suit, int number);

    void onUse(GameLogic *logic, CardUseStruct &use) override;
    void effect(GameLogic *, CardEffectStruct &effect) override;
    bool isAvailable(const Player *) const override;
};

class Peach : public BasicCard
{
    Q_OBJECT

public:
    Q_INVOKABLE Peach(Suit suit, int number);

    void onUse(GameLogic *logic, CardUseStruct &use) override;
    void effect(GameLogic *logic, CardEffectStruct &effect) override;
    bool isAvailable(const Player *player) const override;
};

#endif // STANDARDBASICCARD_H
