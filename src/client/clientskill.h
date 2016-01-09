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

#ifndef SKILLMODEL_H
#define SKILLMODEL_H

#include <QObject>

class Skill;

class ClientSkill : public QObject
{
    Q_OBJECT

    Q_PROPERTY(uint sid READ id NOTIFY idChanged)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString type READ type NOTIFY typeChanged)
    Q_PROPERTY(bool enabled READ isEnabled NOTIFY enabledChanged)

public:
    ClientSkill(const Skill *skill, QObject *parent = 0);

    uint id() const;
    QString name() const;
    QString type() const;

    void setEnabled(bool enabled);
    bool isEnabled() const { return m_enabled; }

signals:
    void idChanged();
    void nameChanged();
    void typeChanged();
    void enabledChanged();

private:
    const Skill *m_skill;
    bool m_enabled;
};

#endif // SKILLMODEL_H
