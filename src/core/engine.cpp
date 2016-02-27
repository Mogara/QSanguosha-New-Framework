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

#include "card.h"
#include "engine.h"
#include "gamemode.h"
#include "package.h"
#include "general.h"
#include "skill.h"

#include <QJsonDocument>
#include <QFile>
#include <QJsonArray>
#include <QCoreApplication>

#include <QThreadStorage>
#include <CJSEngine>

namespace
{
    QThreadStorage<CJSEngine> jsEngineStorage;
}

namespace
{
    void loadGameModes()
    {
        // dummy for now because the modes are now in CPP
    }

    void loadPackages(bool addToEngine = false)
    {
        CJSEngine *engine = Engine::JsEngineInstance(true);
        bool packageLoaded = engine->globalObject().hasProperty("packageLoaded") && engine->globalObject().property("packageLoaded").isBool() && engine->globalObject().property("packageLoaded").toBool();
        bool packageAdded = engine->globalObject().hasProperty("packageAdded") && engine->globalObject().property("packageAdded").isBool() && engine->globalObject().property("packageAdded").toBool();

        if (packageAdded || (packageLoaded && !addToEngine))
            return;

        QJSValueList packageInitializers;
        QStringList packages;

        QFile packageFile("script/Package/package.json");
        if (packageFile.open(QIODevice::ReadOnly)) {
            QJsonParseError error;
            QJsonDocument packageJson = QJsonDocument::fromJson(packageFile.readAll(), &error);
            packageFile.close();
            if (error.error != QJsonParseError::NoError) {
                // @todo: Takashiro: report error to the UI system!!! there is no way calling QMessageBox::critical
                return;
            } else {
                QVariantList packages = packageJson.array().toVariantList();
                if (packages.isEmpty()) {
                    // @todo: Takashiro: report error to the UI system!!! there is no way calling QMessageBox::critical
                    return;
                } 
                foreach (const QVariant &packageNameV, packages) {
                    if (!packageNameV.canConvert(QVariant::String)) {
                        // @todo: Takashiro: report warning to the UI system!!! there is no way calling QMessageBox::warning
                        continue;
                    }
                    QString packageName = packageNameV.toString();
                    packages << packageName;
                }
            }
        } else {
            // @todo: Takashiro: report error to the UI system!!! there is no way calling QMessageBox::critical
            return;
        }

        foreach (const QString &packageName, packages) {
            if (!packageLoaded) {
                QFile singlePackageFile("script/Package/" + packageName + "/" + packageName + ".js");
                if (!singlePackageFile.open(QIODevice::ReadOnly)) {
                    // @todo: Takashiro: report warning to the UI system!!! there is no way calling QMessageBox::warning
                    continue;
                }
                QJSValue evaluateResult = engine->evaluate(singlePackageFile.readAll(), "script/Package/" + packageName + "/" + packageName + ".js");
                singlePackageFile.close();
                if (evaluateResult.isError()) {
                    // @todo: Takashiro: report warning to the UI system!!! there is no way calling QMessageBox::warning
                    continue;
                }
            }
            if (addToEngine) {
                QJSValue initPackageFunction = engine->globalObject().property("newPackage" + packageName);
                if (!initPackageFunction.isCallable()) {
                    // @todo: Takashiro: report warning to the UI system!!! there is no way calling QMessageBox::warning
                    continue;
                }
                packageInitializers << initPackageFunction;
            }
        }
        engine->globalObject().setProperty("packageLoaded", true);
        if (addToEngine) {
            foreach (QJSValue packageInitializer, packageInitializers) {
                QJSValue packageValue = packageInitializer.call();
                if (packageValue.isQObject()) {
                    Package *package = qobject_cast<Package *>(packageValue.toQObject());
                    if (package == nullptr) {
                        // @todo: Takashiro: report warning to the UI system!!! there is no way calling QMessageBox::warning
                        continue;
                    }
                    package->setIsCreatedByJs(true);
                    Sanguosha.addPackage(package);
                } else {
                    // @todo: Takashiro: report warning to the UI system!!! there is no way calling QMessageBox::warning
                    continue;
                }
            }
            engine->globalObject().setProperty("packageAdded", true);
        }
    }
}

CJSEngine *Engine::JsEngineInstance(bool create /* = false */)
{
    if (jsEngineStorage.hasLocalData())
        return &jsEngineStorage.localData();

    if (create) {
        CJSEngine &jsEngine = jsEngineStorage.localData(); // create a default initialized JS engine in the internal code of QThreadStorage
        // @todo_Fs: Shall we load a start script in this period? if so, we can put the initializations in JS file
        return &jsEngine;
    }

    return nullptr;
}

Engine::Engine()
{
}

Engine *Engine::instance()
{
    static Engine engine;
    return &engine;
}

Engine::~Engine()
{
    foreach (const GameMode *mode, m_modes)
        delete mode;

    foreach (Package *package, m_packages) {
        if (!package->isCreatedByJs()) // JS packages can be deleted by the Garbage Collector
            delete package;
    }
}

const GameMode *Engine::mode(const QString &name) const
{
    foreach (const GameMode *mode, m_modes) {
        if (mode->name() == name)
            return mode;
    }

    return nullptr;
}

void Engine::addPackage(Package *package)
{
    m_packages.insert(package->name(), package);

    QList<const General *> generals = package->generals(true);
    foreach (const General *general, generals) {
        m_generals.insert(general->id(), general);

        QList<const Skill *> skills = general->skills();
        foreach (const Skill *skill, skills)
            m_skills.insert(skill->id(), skill);
    }

    QList<const Card *> cards = package->cards();
    foreach (const Card *card, cards)
        m_cards.insert(card->id(), card);
}

QList<const Package *> Engine::packages() const
{
    QList<const Package *> packages;
    packages.reserve(m_packages.size());
    foreach (Package *package, m_packages)
        packages << package;
    return packages;
}

QList<const General *> Engine::getGenerals(bool includeHidden) const
{
    QList<const General *> generals;
    foreach (const Package *package, m_packages)
        generals << package->generals(includeHidden);
    return generals;
}

void Engine::init()
{
    loadGameModes();
    loadPackages(true);
}