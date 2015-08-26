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

DEFINES += MCD_STATIC
#DEFINES += MCD_BUILD
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
    !contains(DEFINES, MCD_STATIC){
        CONFIG(debug, debug|release): copy(Cardirector/bin/Cardirectord.dll, $$_PRO_FILE_PWD_/)
        else:copy(Cardirector/bin/Cardirector.dll, $$_PRO_FILE_PWD_/)
    }

    RC_FILE = src/resource/icons/icon.rc

    QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
}

macx {
    ICON = src/resource/icons/icon.icns
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
    script/Client/ClientSettings.qml \
    script/Gui/Splash.qml \
    script/Gui/StartScene.qml \
    script/Gui/Lobby.qml \
    script/Gui/Dialog/StartGameDialog.qml \
    script/Gui/Dialog/StartServerDialog.qml \
    script/Gui/RoomScene.qml \
    script/Gui/RoomElement/CardItem.qml \
    script/Gui/RoomElement/ChatBox.qml \
    script/Gui/RoomElement/ChooseGeneralBox.qml \
    script/Gui/RoomElement/Dashboard.qml \
    script/Gui/RoomElement/GeneralAvatar.qml \
    script/Gui/RoomElement/GeneralCardItem.qml \
    script/Gui/RoomElement/GlowText.qml \
    script/Gui/RoomElement/GraphicsBox.qml \
    script/Gui/RoomElement/HandcardNumber.qml \
    script/Gui/RoomElement/HpBar.qml \
    script/Gui/RoomElement/IrregularButton.qml \
    script/Gui/RoomElement/LogBox.qml \
    script/Gui/RoomElement/Magatama.qml \
    script/Gui/RoomElement/PixmapAnimation.qml \
    script/Gui/RoomElement/RoleComboBox.qml \
    script/Gui/RoomElement/SimpleRoleIcon.qml \
    script/Gui/RoomElement/Photo.qml \
    script/engine.js \
    script/utility.js \
    script/main.qml

# Create the resource file
CONFIG(embeded_resource){
    !build_pass{
        defineTest(generate_qrc){
            qrc_path = $$1
            qrc_contents = $$2

            resource_content = \
                "<RCC>" \
                "    <qresource prefix=\"/\">"

            for(resource_file, qrc_contents) {
                resource_content += "        <file>$$relative_path($$resource_file)</file>"
            }

            resource_content += \
                "    </qresource>" \
                "</RCC>"

            write_file($$qrc_path, resource_content) | error("Aborting.")
        }

        generate_qrc("qml.qrc", $$QML_FILES)

        equals(QMAKE_HOST.os, Windows): MCD_LS = "dir /A:-D /B /S image"
        equals(QMAKE_HOST.os, Linux): MCD_LS = "find image -type f"
        defined(MCD_LS, var): generate_qrc("image.qrc", $$system($$MCD_LS))
    }

    RESOURCES += \
        image.qrc \
        qml.qrc
    DEFINES += EMBEDED_RESOURCE
}

lupdate_only {
    SOURCES += $$QML_FILES
}

TRANSLATIONS += \
    translations/zh_CN.ts \
    translations/en_US.ts

OTHER_FILES += src/resource/android/AndroidManifest.xml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/src/resource/android

# Default rules for deployment.
include(deployment.pri)
