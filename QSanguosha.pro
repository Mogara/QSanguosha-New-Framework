TEMPLATE = app

QT += qml quick

SOURCES += src/main.cpp \
    src/client/client.cpp \
    src/core/gamelogic.cpp \
    src/core/player.cpp \
    src/gui/dialog/startserverdialog.cpp \
    src/gui/dialog/startgamedialog.cpp \
    src/gui/lobby.cpp

HEADERS += \
    src/client/client.h \
    src/core/gamelogic.h \
    src/core/player.h \
    src/gui/dialog/startserverdialog.h \
    src/gui/dialog/startgamedialog.h \
    src/gui/lobby.h

INCLUDEPATH += src

INCLUDEPATH += Cardirector/include
LIBS += -L"$$_PRO_FILE_PWD_/Cardirector/lib"

defineTest(copy) {
    file = $$1
    path = $$2
    !exists($$file): return(false)
    system("$$QMAKE_COPY $$system_path($$file) $$system_path($$path)")

    return(true)
}

win32 {
    CONFIG(debug, debug|release): copy(Cardirector/bin/Cardirectord.dll, $$_PRO_FILE_PWD_/)
    else:copy(Cardirector/bin/Cardirector.dll, $$_PRO_FILE_PWD_/)

    RC_FILE = icons/icon.rc

    QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
}

macx {
    ICON = icons/icon.icns
}

android {
    CONFIG += embeded_resource
    QT += androidextras
    ANDROID_EXTRA_LIBS += $$PWD/Cardirector/lib/libCardirector.so
}

LIBS += -l$$qtLibraryTarget(Cardirector)

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = $$PWD

QML_FILES = \
    ui-script/Client/ClientSettings.qml \
    ui-script/Gui/Splash.qml \
    ui-script/Gui/StartScene.qml \
    ui-script/Gui/Lobby.qml \
    ui-script/Gui/Dialog/StartGameDialog.qml \
    ui-script/Gui/Dialog/StartServerDialog.qml \
    ui-script/Gui/RoomScene.qml \
    ui-script/Gui/RoomElement/CardItem.qml \
    ui-script/Gui/RoomElement/ChatBox.qml \
    ui-script/Gui/RoomElement/ChooseGeneralBox.qml \
    ui-script/Gui/RoomElement/Dashboard.qml \
    ui-script/Gui/RoomElement/GeneralAvatar.qml \
    ui-script/Gui/RoomElement/GeneralCardItem.qml \
    ui-script/Gui/RoomElement/GlowText.qml \
    ui-script/Gui/RoomElement/GraphicsBox.qml \
    ui-script/Gui/RoomElement/HandcardNumber.qml \
    ui-script/Gui/RoomElement/HpBar.qml \
    ui-script/Gui/RoomElement/IrregularButton.qml \
    ui-script/Gui/RoomElement/LogBox.qml \
    ui-script/Gui/RoomElement/Magatama.qml \
    ui-script/Gui/RoomElement/RoleComboBox.qml \
    ui-script/Gui/RoomElement/SimpleRoleIcon.qml \
    ui-script/Gui/RoomElement/Photo.qml \
    ui-script/utility.js \
    ui-script/main.qml

# Create the resource file
GENERATED_RESOURCE_FILE = qml.qrc

INCLUDED_RESOURCE_FILES = $$QML_FILES

RESOURCE_CONTENT = \
    "<RCC>" \
    "    <qresource prefix=\"/\">"

for(resourcefile, INCLUDED_RESOURCE_FILES) {
    RESOURCE_CONTENT += "        <file>$$resourcefile</file>"
}

RESOURCE_CONTENT += \
    "    </qresource>" \
    "</RCC>"

write_file($$GENERATED_RESOURCE_FILE, RESOURCE_CONTENT)|error("Aborting.")

CONFIG(embeded_resource){
    RESOURCES += \
        image.qrc \
        $$GENERATED_RESOURCE_FILE
    DEFINES += EMBEDED_RESOURCE
}

lupdate_only {
    SOURCES += $$QML_FILES
}

TRANSLATIONS += translations/zh_CN.ts

OTHER_FILES += android/AndroidManifest.xml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

# Default rules for deployment.
include(deployment.pri)
