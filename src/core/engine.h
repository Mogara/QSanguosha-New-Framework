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

#ifndef ENGINE_H
#define ENGINE_H

#include <QString>
#include <QList>
#include <QMap>

class Card;
class General;
class Package;
class Skill;

class Engine
{
public:
    static Engine *instance();
    ~Engine();

    void addPackage(Package *package);
    const Package *package(const QString &name) const;
    QList<const Package *> packages() const;

    QList<const General *> getGenerals() const;
    const General *getGeneral(const QString &name) const { return m_generals.value(name); }

    QList<const Card *> getCards() const;
    const Card *getCard(uint id) const { return m_cards.value(id); }

private:
    Engine();

    QList<Package *> m_packages;
    QMap<QString, const General *> m_generals;
    QMap<uint, const Card *> m_cards;
};

#define ADD_PACKAGE(name) struct name##PackageAdder\
{\
    name##PackageAdder()\
    {\
        Engine::instance()->addPackage(new name##Package);\
    }\
};\
static name##PackageAdder __packageAdder__;

#endif // ENGINE_H
