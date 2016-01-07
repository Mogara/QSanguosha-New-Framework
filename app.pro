TEMPLATE = app
TARGET = QSanguosha
QT += qml quick multimedia
CONFIG += c++11

SOURCES += src/main.cpp \
    src/client/client.cpp \
    src/client/clientplayer.cpp \
    src/client/clientskill.cpp \
    src/core/card.cpp \
    src/core/cardarea.cpp \
    src/core/cardpattern.cpp \
    src/core/engine.cpp \
    src/core/gamemode.cpp \
    src/core/general.cpp \
    src/core/package.cpp \
    src/core/player.cpp \
    src/core/protocol.cpp \
    src/core/skill.cpp \
    src/core/structs.cpp \
    src/core/util.cpp \
    src/gamelogic/event.cpp \
    src/gamelogic/eventhandler.cpp \
    src/gamelogic/gamelogic.cpp \
    src/gamelogic/gamerule.cpp \
    src/gamelogic/serverplayer.cpp \
    src/gui/dialog/pcconsolestartdialog.cpp \
    src/gui/dialog/startserverdialog.cpp \
    src/gui/dialog/startgamedialog.cpp \
    src/gui/lobby.cpp \
    src/gui/roomscene.cpp \
    src/package/hegstandardpackage.cpp \
    src/package/hegstandard-qun.cpp \
    src/package/hegstandard-shu.cpp \
    src/package/hegstandard-wei.cpp \
    src/package/hegstandard-wu.cpp \
    src/package/standardpackage.cpp \
    src/package/standard-basiccard.cpp \
    src/package/standard-equipcard.cpp \
    src/package/standard-qun.cpp \
    src/package/standard-shu.cpp \
    src/package/standard-trickcard.cpp \
    src/package/standard-wei.cpp \
    src/package/standard-wu.cpp \
    src/package/systempackage.cpp \
    src/package/maneuveringpackage.cpp

HEADERS += \
    src/client/client.h \
    src/client/clientplayer.h \
    src/client/clientskill.h \
    src/core/card.h \
    src/core/cardarea.h \
    src/core/cardpattern.h \
    src/core/engine.h \
    src/core/gamemode.h \
    src/core/general.h \
    src/core/package.h \
    src/core/player.h \
    src/core/protocol.h \
    src/core/skill.h \
    src/core/structs.h \
    src/core/util.h \
    src/gamelogic/event.h \
    src/gamelogic/eventhandler.h \
    src/gamelogic/eventtype.h \
    src/gamelogic/gamelogic.h \
    src/gamelogic/gamerule.h \
    src/gamelogic/serverplayer.h \
    src/gui/dialog/pcconsolestartdialog.h \
    src/gui/dialog/startserverdialog.h \
    src/gui/dialog/startgamedialog.h \
    src/gui/lobby.h \
    src/gui/roomscene.h \
    src/package/hegstandardpackage.h \
    src/package/standardpackage.h \
    src/package/standard-basiccard.h \
    src/package/standard-equipcard.h \
    src/package/standard-trickcard.h \
    src/package/systempackage.h \
    src/package/maneuveringpackage.h

INCLUDEPATH += src \
    src/client \
    src/core \
    src/gamelogic \
    src/gui \
    src/package

defineTest(copy) {
    file = $$1
    path = $$2
    !exists($$file): return(false)
    system("$$QMAKE_COPY $$system_path($$file) $$system_path($$path)")

    return(true)
}

win32 {
    RC_FILE = src/resource/icons/icon.rc
    QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
}

macx {
    ICON = src/resource/icons/icon.icns
}

android{
    QT += androidextras
    CONFIG += embedresource
}

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = $$PWD

QML_FILES = \
    script/Client/ClientSettings.qml \
    script/Gui/Splash.qml \
    script/Gui/StartScene.qml \
    script/Gui/Lobby.qml \
    script/Gui/Dialog/PcConsoleStartDialog.qml \
    script/Gui/Dialog/StartGameDialog.qml \
    script/Gui/Dialog/StartServerDialog.qml \
    script/Gui/RoomScene.qml \
    script/Gui/RoomElement/CardArea.qml \
    script/Gui/RoomElement/CardItem.qml \
    script/Gui/RoomElement/ChatBox.qml \
    script/Gui/RoomElement/ChooseCardBox.qml \
    script/Gui/RoomElement/ChooseGeneralBox.qml \
    script/Gui/RoomElement/ChooseOptionBox.qml \
    script/Gui/RoomElement/Dashboard.qml \
    script/Gui/RoomElement/DelayedTrickArea.qml \
    script/Gui/RoomElement/EquipArea.qml \
    script/Gui/RoomElement/EquipItem.qml \
    script/Gui/RoomElement/GeneralAvatar.qml \
    script/Gui/RoomElement/GeneralCardItem.qml \
    script/Gui/RoomElement/GlowText.qml \
    script/Gui/RoomElement/GraphicsBox.qml \
    script/Gui/RoomElement/GuanxingBox.qml \
    script/Gui/RoomElement/HandcardArea.qml \
    script/Gui/RoomElement/HandcardNumber.qml \
    script/Gui/RoomElement/HpBar.qml \
    script/Gui/RoomElement/IndicatorLine.qml \
    script/Gui/RoomElement/InvisibleCardArea.qml \
    script/Gui/RoomElement/IrregularButton.qml \
    script/Gui/RoomElement/LogBox.qml \
    script/Gui/RoomElement/Magatama.qml \
    script/Gui/RoomElement/Photo.qml \
    script/Gui/RoomElement/PixmapAnimation.qml \
    script/Gui/RoomElement/PlayerCardBox.qml \
    script/Gui/RoomElement/ProgressBar.qml \
    script/Gui/RoomElement/Prompt.qml \
    script/Gui/RoomElement/RoleComboBox.qml \
    script/Gui/RoomElement/SimpleEquipArea.qml \
    script/Gui/RoomElement/SimpleEquipItem.qml \
    script/Gui/RoomElement/SimpleRoleIcon.qml \
    script/Gui/RoomElement/SkillButton.qml \
    script/Gui/RoomElement/SkillPanel.qml \
    script/Gui/RoomElement/TablePile.qml \
    script/engine.js \
    script/utility.js \
    script/main.qml

# Embed some essential system resources
embedresource{
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

        equals(QMAKE_HOST.os, Windows): MCD_LS = "dir /A:-D /B /S \"image/system\""
        equals(QMAKE_HOST.os, Linux): MCD_LS = "find \"image/system\" -type f"
        defined(MCD_LS, var): generate_qrc("image.qrc", $$system($$MCD_LS))
    }

    RESOURCES += \
        image.qrc \
        qml.qrc
    DEFINES += QsSrc=\\\"qrc:/\\\"
    DEFINES += EMBED_RESOURCE
} else {
    DEFINES += QsSrc=\\\"\\\"
}

lupdate_only {
    SOURCES += $$QML_FILES
}
QML_FILES =

TRANSLATIONS += \
    translations/zh_CN.ts \
    translations/en_US.ts

OTHER_FILES += src/resource/android/AndroidManifest.xml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/src/resource/android

# Default rules for deployment.
include(deployment.pri)

# Cardirector
DEFINES += MCD_STATIC
INCLUDEPATH += Cardirector/include
LIBS += -L$$PWD/Cardirector/lib -lCardirector -lbreakpad -lvorbis -logg
