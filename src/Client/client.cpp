/********************************************************************
    Copyright (c) 2013-2015 - Mogara

    This file is part of Cardirector.

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

#include "client.h"
#include <QtQml>

static Client *ClientInstance = NULL;

Client::Client(QObject *parent)
    : CClient(parent)
{
}

static QObject *ClientInstanceCallback(QQmlEngine *, QJSEngine *)
{
    return ClientInstance;
}

void Client::Init()
{
    ClientInstance = new Client(qApp);
    qmlRegisterSingletonType<Client>("Sanguosha", 1, 0, "Client", ClientInstanceCallback);
}
C_INITIALIZE_CLASS(Client)

Client *Client::instance()
{
    return ClientInstance;
}

Client::~Client()
{
    if (ClientInstance == this)
        ClientInstance = NULL;
}
