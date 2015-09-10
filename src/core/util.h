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

#ifndef UTIL_H
#define UTIL_H

#include <QList>
#include <QObject>
#include <QVariant>

template<class T>
void qShuffle(QList<T> &list)
{
    int i, n = list.length();
    for (i = 0; i < n; i++) {
        int r = qrand() % (n - i) + i;
        list.swap(i, r);
    }
}

template<class T>
QVariant qConvertToModel(const QList<T *> &list)
{
    QObjectList objects;
    foreach (T *o, list)
        objects << o;
    return QVariant::fromValue(objects);
}

template<class T>
QVariant qConvertToModel(const QList<const T *> &list)
{
    QObjectList objects;
    foreach (const T *o, list)
        objects << const_cast<T *>(o);
    return QVariant::fromValue(objects);
}

#endif // UTIL_H

