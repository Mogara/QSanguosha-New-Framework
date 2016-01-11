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

#ifndef GAMEMODE_H
#define GAMEMODE_H

#include <QString>
#include <QList>

class EventHandler;
class GameRule;

class GameMode
{
public:
    GameMode();
    virtual ~GameMode();

    const QString &name() const { return m_name; }
    int minPlayerNum() const { return m_minPlayerNum; }
    int maxPlayerNum() const { return m_maxPlayerNum; }

    const GameRule *rule() const { return m_rule; }
    QList<const EventHandler *> extraRules() const { return m_extraRules; }

protected:
    QString m_name;
    int m_minPlayerNum;
    int m_maxPlayerNum;
    const GameRule *m_rule;
    QList<const EventHandler *> m_extraRules;
};

#endif // GAMEMODE_H
