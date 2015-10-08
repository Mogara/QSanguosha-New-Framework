#include "clientplayer.h"

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
