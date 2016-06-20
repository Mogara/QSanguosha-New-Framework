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

#ifndef PACKAGE_H
#define PACKAGE_H

#include <QString>
#include <QList>
#include <QObject>

class Card;
class General;
class GameMode;

class Package : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)

public:
    Q_INVOKABLE explicit Package(const QString &name = QString());
    virtual ~Package();

    QString name() const { return m_name; }
    QList<const General *> generals(bool includeHidden = false) const;
    Q_INVOKABLE const General *getGeneral(const QString &name) const;
    QList<const Card *> cards() const;

    Q_INVOKABLE void addGeneral(General *general);
    void addGenerals(const QList<General *> &generals);

    Q_INVOKABLE void addCard(Card *card);
    void addCards(const QList<Card *> &cards);

private:
    QString m_name;
    QList<General *> m_generals;
    QList<Card *> m_cards;
    bool m_isCreatedByJs;
};

#endif // PACKAGE_H
