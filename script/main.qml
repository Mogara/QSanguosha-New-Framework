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
        providerId: "root"

        function imagePath(imageId, requestedSize)
        {
            if (imageId.indexOf('.') === -1)
                return LocalDirPath + "/image/" + imageId + ".png";
            else
                return LocalDirPath + "/image/" + imageId;
        }
    }

    ImageProvider {
        providerId: "system"

        function imagePath(imageId, requestedSize)
        {
            if (imageId.indexOf('.') === -1)
                return QsSrc + "image/system/" + imageId + ".png";
            else
                return QsSrc + "image/system/" + imageId;
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
        var skip_splash = false;
        for (var i = 0; i < Qt.application.arguments.length; i++) {
            var schema = "qsanguosha://";
            var arg = Qt.application.arguments[i];
            if (arg.substr(0, schema.length).toLowerCase() === schema) {
                dialogLoader.setSource("Gui/Dialog/StartGameDialog.qml");
                arg = arg.substr(schema.length, arg.indexOf("/", schema.length) - schema.length);
                dialogLoader.item.serverAddress = arg;
                dialogLoader.item.accepted();
                skip_splash = true;
                break;
            }
        }

        if (skip_splash || Qt.application.arguments.contains("--skip-splash")) {
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
