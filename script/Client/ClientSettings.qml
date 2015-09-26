import Cardirector.Client 1.0

ClientSettings {
    id: config

    property string backgroundImage: config.value("backgroundImage", "image://system/bg.jpg")
    property string tableImage: config.value("tableImage", "image://root/background/table.jpg")
    property string backgroundMusic: config.value("backgroundMusic", "audio/system/background.ogg")
}
