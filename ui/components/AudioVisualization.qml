import QtQuick 2.0

Item {

    height: 200

    ListView {
        anchors.fill: parent

        orientation: Qt.Horizontal
        model: AudioPlayer.currentAudioData

        delegate: Rectangle {
            width: 5
            height: model.modelData.value * parent.height
            color: model.modelData.color
        }
    }
}
