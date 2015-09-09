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
    General *liubei = new General("liubei", "shu", 4);
    addGeneral(liubei);

    General *huangyueying = new General("huangyueying", "shu", 3, General::Female);
    addGeneral(huangyueying);

    General *zhugeliang = new General("zhugeliang", "shu", 3);
    addGeneral(zhugeliang);

    General *guanyu = new General("guanyu", "shu", 5);
    addGeneral(guanyu);

    General *zhangfei = new General("zhangfei", "shu", 4);
    addGeneral(zhangfei);

    General *zhaoyun = new General("zhaoyun", "shu", 4);
    addGeneral(zhaoyun);

    General *huangzhong = new General("huangzhong", "shu", 4);
    addGeneral(huangzhong);

    General *weiyan = new General("weiyan", "shu", 4);
    addGeneral(weiyan);

    General *pangtong = new General("pangtong", "shu", 3);
    addGeneral(pangtong);

    General *wolong = new General("wolong", "shu", 3);
    addGeneral(wolong);

    General *liushan = new General("liushan", "shu", 3);
    addGeneral(liushan);

    General *menghuo = new General("menghuo", "shu", 4);
    addGeneral(menghuo);

    General *zhurong = new General("zhurong", "shu", 4, General::Female);
    addGeneral(zhurong);

    General *ganfuren = new General("ganfuren", "shu", 3, General::Female);
    addGeneral(ganfuren);
}
