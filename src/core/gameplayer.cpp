#include "gameplayer.h"

GamePlayer::GamePlayer(QObject *parent)
    : CAbstractGamePlayer(parent)
    , m_hp(0)
    , m_maxHp(0)
{

}
