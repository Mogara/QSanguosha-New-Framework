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

#ifndef GENERAL_H
#define GENERAL_H

#include <QObject>

#include <QSet>
#include <QString>

class Skill;
class Package;

class General : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName MEMBER m_name)
    Q_PROPERTY(QString kingdom READ kingdom WRITE setKingdom MEMBER m_kingdom)
    Q_PROPERTY(int maxHp READ maxHp WRITE setMaxHp MEMBER m_maxHp)
    Q_PROPERTY(Gender gender READ gender WRITE setGender MEMBER m_gender)
    Q_PROPERTY(bool isLord READ isLord WRITE setLord MEMBER m_lord)
    Q_PROPERTY(bool isHidden READ isHidden WRITE setHidden MEMBER m_hidden)
    Q_PROPERTY(bool isNeverShown READ isNeverShown WRITE setNeverShown MEMBER m_neverShown)
    Q_PROPERTY(int headExtraMaxHp READ headExtraMaxHp WRITE setHeadExtraMaxHp MEMBER m_headExtraMaxHp)
    Q_PROPERTY(int deputyExtraMaxHp READ deputyExtraMaxHp WRITE setDeputyExtraMaxHp MEMBER m_deputyExtraMaxHp)
        

private:
    friend class Package;

public:
    enum Gender
    {
        Sexless,
        Male,
        Female,
        Neuter
    };
    Q_ENUM(Gender)

    Q_INVOKABLE General(const QString &name, const QString &kingdom, int maxHp, Gender gender = Male);
    ~General();

    uint id() const { return m_id; }

    void setName(const QString &name) { m_name = name; }
    const QString &name() const { return m_name; }

    void setKingdom(const QString &kingdom) { m_kingdom = kingdom; }
    const QString &kingdom() const { return m_kingdom; }

    void setMaxHp(int maxHp) { m_maxHp = maxHp; }
    int maxHp() const { return m_maxHp; }

    void setGender(Gender gender) { m_gender = gender; }
    Gender gender() const { return m_gender; }

    void setLord(bool lord) { m_lord = lord; }
    bool isLord() const { return m_lord; }

    void setHidden(bool hidden) { m_hidden = hidden; }
    bool isHidden() const { return m_hidden; }

    void setNeverShown(bool neverShown) { m_neverShown = neverShown; }
    bool isNeverShown() const { return m_neverShown; }

    void setHeadExtraMaxHp(int maxHp) { m_headExtraMaxHp = maxHp; }
    int headExtraMaxHp() const { return m_headExtraMaxHp; }
    int headMaxHp() const { return maxHp() + headExtraMaxHp(); }

    void setDeputyExtraMaxHp(int maxHp) { m_deputyExtraMaxHp = maxHp; }
    int deputyExtraMaxHp() const { return m_deputyExtraMaxHp; }
    int deputyMaxHp() const { return maxHp() + deputyExtraMaxHp(); }

    Q_INVOKABLE void addCompanion(const QString &companion) { m_companions.insert(companion); }
    QSet<QString> companions() const { return m_companions; }
    Q_INVOKABLE bool isCompanionWith(const General *general) const;
    Q_INVOKABLE QStringList companionList() const { return m_companions.toList(); }

    Q_INVOKABLE void addSkill(Skill *skill);
    Q_INVOKABLE bool hasSkill(const Skill *skill) const;
    Q_INVOKABLE const QList<const Skill *> &skills() const { return m_skills; }

private:
    int m_id;
    QString m_name;
    QString m_kingdom;
    int m_maxHp;
    Gender m_gender;
    bool m_lord;
    bool m_hidden;
    bool m_neverShown;

    int m_headExtraMaxHp;
    int m_deputyExtraMaxHp;

    QSet<QString> m_companions;

    QList<const Skill *> m_skills;
};

typedef QList<const General *> GeneralList;
Q_DECLARE_METATYPE(General::Gender)

#endif // GENERAL_H
