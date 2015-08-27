import QtQuick 2.4
import Cardirector.Gui 1.0
import Cardirector.Device 1.0
import "../../engine.js" as Engine

GraphicsBox {
    property var options: ["what if this is a very very very long text", "test2", "test3"]
    property var result

    id: root
    title.text: qsTr("Please choose")
    width: body.width + Device.gu(20)
    height: body.height + title.height + Device.gu(20)

    Column {
        id: body
        x: Device.gu(10)
        y: title.height + Device.gu(5)
        spacing: Device.gu(10)

        Repeater {
            model: options

            MetroButton {
                text: Engine.translate(modelData)
                anchors.horizontalCenter: parent.horizontalCenter

                onClicked: {
                    result = modelData;
                    root.close();
                }
            }
        }
    }
}
