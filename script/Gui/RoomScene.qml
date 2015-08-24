import QtQuick 2.4
import QtQuick.Layouts 1.1
import Cardirector.Gui 1.0
import Cardirector.Device 1.0
import Cardirector.Resource 1.0
import "RoomElement"

Image {
    property int playerNum: 8
    property int firstPlayerIndex: 0

    id: roomScene
    source: config.tableImage
    anchors.fill: parent

    ImageProvider {
        id: generalImage
        providerId: "general"

        function imagePath(imageId, requestedSize)
        {
            return "image/general/" + imageId + ".png";
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: Device.gu(3)

        RowLayout {
            spacing: 1

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                Item {
                    id: roomArea
                    anchors.fill: parent
                    anchors.margins: Device.gu(10)

                    Repeater {
                        id: photos
                        model: playerNum - 1
                        Photo {
                        }
                    }

                    onWidthChanged: arrangePhotos();
                    onHeightChanged: arrangePhotos();
                }
            }

            ColumnLayout {
                spacing: 1
                Layout.fillWidth: false
                Layout.preferredWidth: Device.gu(275)

                LogBox {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                ChatBox {
                    Layout.fillWidth: true
                    Layout.preferredHeight: Device.gu(200)
                }
            }
        }

        Dashboard {
        }
    }

    Loader {
        id: dialogLoader

        onSourceChanged: {
            if (item === null)
                return;
            item.x = (roomArea.width - item.width) / 2;
            item.y = (roomArea.height - item.height) / 2;
            item.finished.connect(function(){
                dialogLoader.source = "";
            });
        }
    }

    onPlayerNumChanged: arrangePhotos();

    function arrangePhotos()
    {
        /*
        Layout:
           col1           col2
        _______________________
        |_2_|______1_______|_0_| row1
        |   |              |   |
        | 4 |    table     | 3 |
        |___|______________|___|
        |      dashboard       |
        ------------------------
        region 5 = 0 + 3, region 6 = 2 + 4, region 7 = 0 + 1 + 2
        */

        var regularSeatIndex = [
            [1],
            [5, 6],
            [5, 1, 6],
            [3, 1, 1, 4],
            [3, 7, 7, 7, 4],
            [5, 5, 1, 1, 6, 6],
            [5, 5, 1, 1, 1, 6, 6],
            [5, 5, 1, 1, 1, 1, 6, 6],
            [3, 3, 7, 7, 7, 7, 7, 4, 4]
        ];
        var seatIndex = regularSeatIndex[playerNum - 2];
        var horizontalBorder = roomArea.height * 0.4;
        var sideWidth = playerNum < 9 ? 0.2 : 0.15;
        var verticalBorders = [roomArea.width * sideWidth, roomArea.width * (1 - sideWidth)];
        var regions = [
            {top: 0, bottom: horizontalBorder, left: verticalBorders[1], right: roomArea.width, players: []},
            {top: 0, bottom: horizontalBorder, left: verticalBorders[0], right: verticalBorders[1], players: []},
            {top: 0, bottom: horizontalBorder, left: 0, right: verticalBorders[0], players: []},
            {top: horizontalBorder, bottom: roomArea.height, left: verticalBorders[1], right: roomArea.width, players: []},
            {top: horizontalBorder, bottom: roomArea.height, left: 0, right: verticalBorders[0], players: []},
            {top: 0, bottom: roomArea.height, left: verticalBorders[1], right: roomArea.width, players: []},
            {top: 0, bottom: roomArea.height, left: 0, right: verticalBorders[0], players: []},
            {top: 0, bottom: horizontalBorder, left: 0, right: roomArea.width, players: []}
        ];

        var roomAreaPadding = Device.gu(10);
        var item, region, i, subindex, x, y, spacing;

        for (i = 0; i < playerNum - 1; i++)
            regions[seatIndex[i]].players.push(i);

        for (i = 0; i < playerNum - 1; i++) {
            item = photos.itemAt(i);
            region = regions[seatIndex[i]];
            subindex = region.players.indexOf(i);
            if (seatIndex[i] === 1 || seatIndex[i] === 7) {
                if (playerNum === 6 || playerNum === 10) {
                    spacing = ((region.right - region.left) - region.players.length * item.width) / (region.players.length + 1);
                    x = region.left + spacing * (subindex + 1) + item.width * subindex;
                } else {
                    x = region.left + (region.right - region.left) / region.players.length / 2 * (subindex * 2 + 1) - item.width / 2;
                }
            } else {
                x = (region.left + region.right - item.width) / 2;
            }
            if ((seatIndex[i] >= 0 && seatIndex[i] <= 2) || seatIndex[i] === 7) {
                y = (region.top + region.bottom - item.height) / 2;
            } else {
                spacing = ((region.bottom - region.top) - region.players.length * item.height) / (region.players.length + 1);
                y = region.top + spacing * (subindex + 1) + item.height * subindex;
            }
            item.x = Math.round(x);
            item.y = Math.round(y);
        }
    }
}
