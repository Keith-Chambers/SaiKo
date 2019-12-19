import QtQuick 2.12
import QtQuick.Controls 2.12
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
            value: 0.0

            onValueChanged: {

                if(MFileSys.currentPlaylistIndex == -1) {
                    value = 0.0;
                }

                if(playPosSlider.pressed) {
                    AudioPlayer.playPosition = value;
                    return;
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
                        color: "#4F4F4F"
                        radius: 8

                        Rectangle
                        {
                            id: coloredPart
                            color: "light grey"
                            radius: 8
                            anchors {
                                left: parent.left
                                top: parent.top
                                bottom: parent.bottom
                            }
                            width: playPosSlider.value * parent.width
                        }
                    }
                    handle: Rectangle {
//                        anchors.centerIn: parent
//                        color: control.pressed ? "white" : "lightgray"
//                        border.color: "gray"
//                        border.width: 2
//                        implicitWidth: 10
//                        implicitHeight: 15
//                        radius: 5
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

                width: 30;
                height: 30;
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
                        MFileSys.prevTrack();
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

                width: 30;
                height: 30;
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
                        MFileSys.nextTrack();
                    }
                }
            }
        }
    }
}
