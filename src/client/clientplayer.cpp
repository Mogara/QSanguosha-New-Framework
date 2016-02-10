#include "clientplayer.h"
#include "clientskill.h"
#include "skill.h"
#include "util.h"

ClientPlayer::ClientPlayer(CClientUser *user, QObject *parent)
    : Player(parent)
    , m_user(user)
{
    connect(this, &ClientPlayer::headGeneralChanged, [this](){
        setHeadGeneralShown(headGeneral() != nullptr);
    });
    connect(this, &ClientPlayer::deputyGeneralChanged, [this](){
        setDeputyGeneralShown(deputyGeneral() != nullptr);
    });
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

void ClientPlayer::addSkill(const Skill *skill, SkillArea type)
{
    if (type == HeadSkillArea) {
        addHeadSkill(skill);
        m_headSkills.insert(skill, new ClientSkill(skill, this));
        emit headSkillChanged();
    } else if (type == DeputySkillArea) {
        addDeputySkill(skill);
        m_deputySkills.insert(skill, new ClientSkill(skill, this));
        emit deputySkillChanged();
    } else {
        addAcquiredSkill(skill);
        m_acquiredSkills.insert(skill, new ClientSkill(skill, this));
        emit acquiredSkillChanged();
    }
}

ClientSkill *ClientPlayer::getSkill(const Skill *skill) const
{
    ClientSkill *model = m_headSkills.value(skill);
    if (model == nullptr) {
        model = m_deputySkills.value(skill);
        if (model == nullptr)
            model = m_acquiredSkills.value(skill);
    }
    return model;
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
