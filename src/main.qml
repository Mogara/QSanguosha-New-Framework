import QtQuick 2.4
import Cardirector.Resource 1.0
import "Client"
import "utility.js" as Utility

Item {
    id: root

    ClientSettings {
        id: config
    }

    ImageProvider {
        id: mogaraImage
        providerId: "mogara"

        function imagePath(imageId, requestedSize)
        {
            return ":/image/mogara/" + imageId + ".png";
        }
    }

    ImageProvider {
        providerId: "background"

        function imagePath(imageId, requestedSize) {
            // We prefer to using compact pictures as background to save storage space
            // @todo: consider supporting more common image formats
            return ":/image/background/" + imageId + ".jpg";
        }
    }

    ImageProvider {
        providerId: "tileicon"

        function imagePath(imageId, requestedSize) {
            return ":/image/tileIcon/" + imageId + ".png"
        }
    }

    Loader {
        id: startSceneLoader
        anchors.fill: parent
    }

    Loader {
        id: dialogLoader
        z: 100
        anchors.fill: parent
        onSourceChanged: startSceneLoader.visible = (source == "");
    }

    Loader {
        id: splashLoader
        anchors.fill: parent
        focus: true
    }

    Component.onCompleted: {
        if (Qt.application.arguments.contains("--skip-splash")) {
            startSceneLoader.source = "Gui/StartScene.qml";
        } else {
            splashLoader.source = "Gui/Splash.qml";
            splashLoader.item.disappearing.connect(function(){
                startSceneLoader.source = "Gui/StartScene.qml";
            });
            splashLoader.item.disappeared.connect(function(){
                splashLoader.source = "";
            });
        }
    }
}
