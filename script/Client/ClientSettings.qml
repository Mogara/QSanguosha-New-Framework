import Cardirector.Client 1.0

ClientSettings {
    id: config

    property string backgroundImage: config.value("backgroundImage", "image://background/bg")
    property string tableImage: config.value("tableImage", "image://background/table")
    property string backgroundMusic: config.value("backgroundMusic", "audio/system/background.ogg")

    onBackgroundImageChanged: config.setValue("backgroundImage", backgroundImage)
}
