#include "clientplayer.h"

ClientPlayer::ClientPlayer(CClientUser *user, QObject *parent)
    : Player(parent)
    , m_user(user)
{
}
