import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import Cardirector.Device 1.0

GraphicsBox {
    title.text: qsTr("WINNERS")
    width: Device.gu(600)
    height: Device.gu(400)

    ListModel {
        id: winnerList
    }

    TableView {
        anchors.fill: parent
        anchors.margins: Device.gu(40)
        anchors.bottomMargin: Device.gu(20)
        backgroundVisible: false

        model: winnerList
        style: TableViewStyle {
            backgroundColor: Qt.rgba(0, 0, 0, 0)
            alternateBackgroundColor: Qt.rgba(0, 0, 0, 0)
        }

        TableViewColumn {
            role: "gameRole"
            title: qsTr("Role")
            width: Device.gu(100)
        }
        TableViewColumn {
            role: "userName"
            title: qsTr("User Name")
            width: Device.gu(150)
        }

        TableViewColumn {
            role: "general"
            title: qsTr("General")
            width: Device.gu(250)
        }

        headerDelegate: Item {
            height: Device.gu(30)
            Text {
                text: styleData.value
                font.pixelSize: Device.gu(24)
                color: Qt.rgba(255, 255, 255, 1)
            }
        }

        rowDelegate: Item {
            height: Device.gu(30)
        }

        itemDelegate: Text {
            text: styleData.value
            font.pixelSize: Device.gu(20)
            color: Qt.rgba(255, 255, 255, 1)
        }
    }

    function add(item)
    {
        winnerList.append(item);
    }
}
