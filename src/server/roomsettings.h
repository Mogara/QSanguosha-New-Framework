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

#ifndef ROOMSETTINGS_H
#define ROOMSETTINGS_H

/* The original header file must be included here, for it's perhaps a bug of moc that no properties of the
 * parent class will be generated in QMetaObject otherwise.
*/
#include <croomsettings.h>

class RoomSettings : public CRoomSettings
{
    Q_GADGET
    const QMetaObject *metaObject() const override;

    Q_PROPERTY(QString mode MEMBER mode)

public:
    RoomSettings();

    QString mode;
};

#endif // ROOMSETTINGS_H
