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

    const Card *slash;
    const Card *jink;

    DamageStruct::Nature nature;
    int drank;
    int jinkNum;
    bool nullified;

    SlashEffectStruct();
};

Q_DECLARE_METATYPE(SlashEffectStruct)

class Slash : public BasicCard
{
    Q_OBJECT

public:
    Q_INVOKABLE Slash(Suit suit, int number);

    void onEffect(GameLogic *logic, CardEffectStruct &cardEffect) override;

protected:
    DamageStruct::Nature m_nature;
    int m_drank;
};

#endif // STANDARDBASICCARD_H
