#include "clientplayer.h"
#include "skill.h"

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
        emit headSkillChanged();
    } else if (position == "deputy") {
        addDeputySkill(skill);
        emit deputySkillChanged();
    } else {
        addAcquiredSkill(skill);
        emit acquiredSkillChanged();
    }
}

static QVariant convertToModel(const QList<const Skill *> &skills)
{
    QVariantList data;
    foreach (const Skill *skill, skills) {
        QVariantMap e;
        e["name"] = skill->name();
        switch (skill->frequency()) {
        case Skill::NotFrequent:
        case Skill::Frequent:
            e["type"] = "proactive";
            break;
        case Skill::Wake:
        case Skill::Limited:
            e["type"] = "limited";
            break;
        case Skill::Compulsory:
        default:
            //@to-do: battle array skill
            e["type"] = "compulsory";
        }
        data << e;
    }
    return data;
}

QVariant ClientPlayer::headSkillModel() const
{
    return convertToModel(headSkills());
}

QVariant ClientPlayer::deputySkillModel() const
{
    return convertToModel(deputySkills());
}

QVariant ClientPlayer::acquiredSkillModel() const
{
    return convertToModel(acquiredSkills());
}
