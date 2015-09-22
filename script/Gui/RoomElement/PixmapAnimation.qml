import QtQuick 2.4
import Qt.labs.folderlistmodel 2.1

Item {
    property string source: ""
    property int currentFrame: 0
    property alias interval: timer.interval
    property int loadedFrameCount: 0

    signal loaded()
    signal started()
    signal finished()

    id: root

    FolderListModel {
        id: fileModel
        folder: "../../../image/animation/" + source
        showDirs: false
    }

    Repeater {
        id: frames
        model: fileModel.count

        Image {
            source: "image://root/animation/" + root.source + "/" + index
            visible: false
            onStatusChanged: {
                if (status == Image.Ready) {
                    loadedFrameCount++;
                    if (loadedFrameCount == fileModel.count)
                        root.loaded();
                }
            }
        }
    }

    onLoaded: timer.start();

    Timer {
        id: timer
        interval: 50
        repeat: true
        onTriggered: {
            if (currentFrame >= fileModel.count) {
                timer.stop();
                frames.itemAt(fileModel.count - 1).visible = false;
                root.finished();
                return;
            }

            if (currentFrame > 0)
                frames.itemAt(currentFrame - 1).visible = false;
            frames.itemAt(currentFrame).visible = true;

            currentFrame++;
        }
    }
}
