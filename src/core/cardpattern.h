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

#ifndef CARDPATTERN_H
#define CARDPATTERN_H

#include <QStringList>

class Player;
class Card;

class CardPattern
{
public:
    CardPattern(const QString &pattern);

    bool match(const Player *player, const Card *card) const;

private:
    struct Exp
    {
        QStringList types;
        QStringList suits;
        QStringList numbers;
        QStringList places;
    };

    bool matchOne(const Player *player, const Card *card, const Exp &exp) const;

    QList<Exp> m_exps;
};

#endif // CARDPATTERN_H
