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
#include <QCoreApplication>

static Client *ClientInstance = NULL;

static QObject *ClientInstanceCallback(QQmlEngine *, QJSEngine *)
{
    return Client::instance();
}

static void initClientInstance()
{
    qmlRegisterSingletonType<Client>("Sanguosha", 1, 0, "Client", ClientInstanceCallback);
}

Q_COREAPP_STARTUP_FUNCTION(initClientInstance)

Client::Client(QObject *parent)
    : CClient(parent)
{
    ClientInstance = this;
}

Client *Client::instance()
{
    static Client *client = new Client(qApp);
    return client;
}

Client::~Client()
{
    if (ClientInstance == this)
        ClientInstance = NULL;
}
