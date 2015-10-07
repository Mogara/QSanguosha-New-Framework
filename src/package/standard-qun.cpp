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

#include "standardpackage.h"
#include "general.h"
#include "skill.h"

void StandardPackage::addQunGenerals()
{
    // QUN 001
    General *huatuo = new General("huatuo", "qun", 3);
    addGeneral(huatuo);

    // QUN 002
    General *lvbu = new General("lvbu", "qun", 4);
    addGeneral(lvbu);

    // QUN 003
    General *diaochan = new General("diaochan", "qun", 3, General::Female);
    addGeneral(diaochan);
}
