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

void StandardPackage::addWuGenerals()
{
    General *sunquan = new General("sunquan", "wu", 4);
    sunquan->setLord(true);
    addGeneral(sunquan);

    General *ganning = new General("ganning", "wu", 4);
    addGeneral(ganning);

    General *lvmeng = new General("lvmeng", "wu", 4);
    addGeneral(lvmeng);

    General *huanggai = new General("huanggai", "wu", 4);
    addGeneral(huanggai);

    General *zhouyu = new General("zhouyu", "wu", 3);
    addGeneral(zhouyu);

    General *daqiao = new General("daqiao", "wu", 3, General::Female);
    addGeneral(daqiao);

    General *luxun = new General("luxun", "wu", 3);
    addGeneral(luxun);

    General *sunshangxiang = new General("sunshangxiang", "wu", 3, General::Female);
    addGeneral(sunshangxiang);
}
