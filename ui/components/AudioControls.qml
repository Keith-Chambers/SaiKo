import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.4

Item
{
    id: root;

    Rectangle
    {
        id: bottomBar
        anchors.fill: parent;

        color: "#363636";

        Slider
        {
            id: playPosSlider

            anchors
            {
                top: parent.top;
                topMargin: 10;
                horizontalCenter: parent.horizontalCenter;
            }

            width: 400;

            onValueChanged: {
                if(playPosSlider.pressed && AudioPlayer.isPlaying) {
                    AudioPlayer.playPosition = value;
                    return;
                }

                if(!AudioPlayer.isPlaying) {
                    value = 0.0;
                }
            }

            updateValueWhileDragging: true;

            Binding {
                target: playPosSlider
                property: "value"
                value: AudioPlayer.playPosition
            }

            style: SliderStyle {
                    groove: Rectangle {
                        implicitWidth: 700
                        implicitHeight: 5
                        color: "gray"
                        radius: 8
                    }
                    handle: Rectangle {
                        anchors.centerIn: parent
                        color: control.pressed ? "white" : "lightgray"
                        border.color: "gray"
                        border.width: 2
                        implicitWidth: 10
                        implicitHeight: 15
                        radius: 5
                    }
                }
        }

        RowLayout
        {
            id: audioControlsLayout
            anchors
            {
                top: playPosSlider.bottom;
                bottom: parent.bottom;
                topMargin: 5;
                bottomMargin: 5;
                horizontalCenter: parent.horizontalCenter;
            }

            width: 120;

            Rectangle
            {

                width: 35;
                height: 35;
                color: "transparent"

                Image
                {
                    id: audioPreviousImage;
                    anchors.fill: parent
                    source: "qrc:///resources/back.png";
                }

                MouseArea {
                    id: audioPrevSongMouseArea;
                    anchors.fill: parent;
                    onClicked: {
                        AudioPlayer.prevSong();
                    }
                }
            }

            Rectangle
            {
                height: 35;
                width: 35;
                color: "transparent";

                Image
                {
                    id: audioTogglePlayImage;
                    anchors.fill: parent;
                    fillMode: Image.Stretch;
                    source: (AudioPlayer.isPlaying) ? "qrc:///resources/pause.png" : "qrc:///resources/play.png";

                    MouseArea
                    {
                        id: audioTogglePlayMouse
                        anchors.fill: parent
                        onClicked:
                        {
                            console.log("Clicked");
                            AudioPlayer.togglePause();
                        }
                    }
                }
            }

            Rectangle
            {

                width: 35;
                height: 35;
                color: "transparent"

                Image
                {
                    id: audioNextImage;
                    anchors.fill: parent
                    source: "qrc:///resources/forward.png";
                }

                MouseArea {
                    id: audioNextSongMouseArea;
                    anchors.fill: parent;
                    onClicked: {
                        AudioPlayer.nextSong();
                    }
                }
            }
        }
    }
}
