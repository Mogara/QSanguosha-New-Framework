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

#include "card.h"
#include "cardpattern.h"
#include "player.h"

CardPattern::CardPattern(const QString &pattern)
{
    QStringList exps = pattern.split('#');
    foreach (const QString &subexp, exps) {
        QStringList factors = subexp.split('|');

        Exp exp;
        exp.types = factors.at(0).split(',');
        if (factors.length() > 1) {
            exp.suits = factors.at(1).split(',');
            if (factors.length() > 2) {
                exp.numbers = factors.at(2).split(',');
                if (factors.length() > 3) {
                    exp.places = factors.at(3).split(',');
                }
            }
        }
        m_exps << exp;
    }
}

bool CardPattern::match(const Player *player, const Card *card) const
{
    foreach (const Exp &exp, m_exps)
        if (matchOne(player, card, exp))
            return true;
    return false;
}

// '|' means 'and', '#' means 'or'.
// the expression splited by '|' has 3 parts,
// 1st part means the card name, and ',' means more than one options.
// 2nd patt means the card suit, and ',' means more than one options.
// 3rd part means the card number, and ',' means more than one options,
// the number uses '~' to make a scale for valid expressions
bool CardPattern::matchOne(const Player *player, const Card *card, const Exp &exp) const
{
    bool checkPoint = false;
    foreach (const QString &or_name, exp.types) {
        checkPoint = false;
        foreach (const QString &_name, or_name.split('+')) {
            QString name = _name;
            if (name == ".") {
                checkPoint = true;
            } else {
                bool isInt = false;
                bool positive = true;
                if (name.startsWith('^')) {
                    positive = false;
                    name = name.mid(1);
                }
                if (card->inherits(name.toLatin1().data())
                    || (name.startsWith('%') && name.midRef(1) == name)
                    || (card->effectiveId() == name.toUInt(&isInt) && isInt))
                    checkPoint = positive;
                else
                    checkPoint = !positive;
            }
            if (!checkPoint)
                break;
        }
        if (checkPoint)
            break;
    }

    if (!checkPoint)
        return false;
    if (exp.suits.isEmpty())
        return true;

    checkPoint = false;
    foreach (const QString &_suit, exp.suits) {
        QString suit = _suit;
        if (suit == ".") {
            checkPoint = true; break;
        }
        bool positive = true;
        if (suit.startsWith('^')) {
            positive = false;
            suit = suit.mid(1);
        }
        if (card->suitString() == suit
            || (card->color() == Card::Black && suit == "black")
            || (card->color() == Card::Red && suit == "red"))
            checkPoint = positive;
        else
            checkPoint = !positive;
        if (checkPoint) break;
    }

    if (!checkPoint)
        return false;
    if (exp.numbers.isEmpty())
        return true;

    checkPoint = false;
    int cardNumber = card->number();

    foreach (const QString &number, exp.numbers) {
        if (number == ".") {
            checkPoint = true;
            break;
        }

        bool isInt = false;
        if (number.contains('~')) {
            QStringList params = number.split('~');
            int from, to;
            if (!params.at(0).size())
                from = 1;
            else
                from = params.at(0).toInt();
            if (!params.at(1).size())
                to = 13;
            else
                to = params.at(1).toInt();

            if (from <= cardNumber && cardNumber <= to)
                checkPoint = true;

        } else if (number.toInt(&isInt) == cardNumber && isInt) {
            checkPoint = true;
        } else if ((number == "A" && cardNumber == 1)
            || (number == "J" && cardNumber == 11)
            || (number == "Q" && cardNumber == 12)
            || (number == "K" && cardNumber == 13)) {
            checkPoint = true;
        }
        if (checkPoint)
            break;
    }

    if (!checkPoint)
        return false;
    if (exp.places.isEmpty())
        return true;

    checkPoint = false;
    if (!player || (exp.places.length() == 1 && exp.places.first() == "."))
        checkPoint = true;

    if (!checkPoint) {
        QList<const Card *> cards = card->realCards();
        if (!cards.isEmpty()) {
            foreach (const Card *card, cards) {
                checkPoint = false;
                foreach (const QString &_p, exp.places) {
                    QString p = _p;
                    if (p == "equipped" && player->equips()->contains(card)) {
                        checkPoint = true;
                    } else if (p == "hand") {
                        if (player->handcards()->contains(card))
                            checkPoint = true;
                    } else {
                        //@to-do: pile cards checking
                    }
                    if (checkPoint)
                        break;
                }
                if (!checkPoint)
                    break;
            }
        }
    }

    return checkPoint;
}
