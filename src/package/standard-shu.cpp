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

void StandardPackage::addShuGenerals()
{
    // SHU 001
    General *liubei = new General("liubei", "shu", 4);
    addGeneral(liubei);

    // SHU 002
    General *guanyu = new General("guanyu", "shu", 5);
    addGeneral(guanyu);

    // SHU 003
    General *zhangfei = new General("zhangfei", "shu", 4);
    addGeneral(zhangfei);

    // SHU 004
    General *zhugeliang = new General("zhugeliang", "shu", 3);
    addGeneral(zhugeliang);

    // SHU 005
    General *zhaoyun = new General("zhaoyun", "shu", 4);
    addGeneral(zhaoyun);

    // SHU 006
    General *machao = new General("machao", "shu", 4);
    addGeneral(machao);

    // SHU 007
    General *huangyueying = new General("huangyueying", "shu", 3, General::Female);
    addGeneral(huangyueying);
}
