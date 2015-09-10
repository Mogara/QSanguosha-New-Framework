import QtQuick 2.4
import Cardirector.Resource 1.0

Image {
    source: "image://root/magatama/0"
    state: "3"

    states: [
        State {
            name: "3"
            PropertyChanges {
                target: main
                source: "image://root/magatama/3"
                opacity: 1
                scale: 1
            }
        },
        State {
            name: "2"
            PropertyChanges {
                target: main
                source: "image://root/magatama/2"
                opacity: 1
                scale: 1
            }
        },
        State {
            name: "1"
            PropertyChanges {
                target: main
                source: "image://root/magatama/1"
                opacity: 1
                scale: 1
            }
        },
        State {
            name: "0"
            PropertyChanges {
                target: main
                source: "image://root/magatama/0"
                opacity: 0
                scale: 4
            }
        }
    ]

    transitions: Transition {
        PropertyAnimation {
            properties: "opacity,scale"
        }
    }

    Image {
        id: main
    }
}
