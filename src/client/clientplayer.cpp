#include "clientplayer.h"
#include "clientskill.h"
#include "skill.h"
#include "util.h"

ClientPlayer::ClientPlayer(CClientUser *user, QObject *parent)
    : Player(parent)
    , m_user(user)
{
}

QString ClientPlayer::fullGeneralName() const
{
    if (!hasShownGeneral())
        return tr("Seat %1").arg(seat());

    if (deputyGeneral()) {
        QString headName = tr(headGeneralName().toLatin1().constData());
        QString deputyName = tr(deputyGeneralName().toLatin1().constData());
        return QString("%1/%2").arg(headName).arg(deputyName);
    } else {
        return tr(generalName().toLatin1().constData());
    }
}

void ClientPlayer::addSkill(const Skill *skill, const QString &position)
{
    if (position == "head") {
        addHeadSkill(skill);
        m_headSkills.insert(skill->name(), new ClientSkill(skill, this));
        emit headSkillChanged();
    } else if (position == "deputy") {
        addDeputySkill(skill);
        m_deputySkills.insert(skill->name(), new ClientSkill(skill, this));
        emit deputySkillChanged();
    } else {
        addAcquiredSkill(skill);
        m_acquiredSkills.insert(skill->name(), new ClientSkill(skill, this));
        emit acquiredSkillChanged();
    }
}

QVariant ClientPlayer::headSkillModel() const
{
    return qConvertToModel(m_headSkills.values());
}

QVariant ClientPlayer::deputySkillModel() const
{
    return qConvertToModel(m_deputySkills.values());
}

QVariant ClientPlayer::acquiredSkillModel() const
{
    return qConvertToModel(m_acquiredSkills.values());
}
