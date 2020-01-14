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

        Text
        {
            id: trackDuration
            text: (MFileSys.currentPlaylistIndex != -1) ? MFileSys.currentAudio.getAudioLength() : "";
            anchors {
                left: playPosSlider.right
                leftMargin: 10
                verticalCenter: playPosSlider.verticalCenter
            }
            color: "white"
        }

        Text
        {
            id: trackCurrentTimePosition
            text: AudioPlayer.playPositionTime;
            anchors {
                right: playPosSlider.left
                rightMargin: 10
                verticalCenter: playPosSlider.verticalCenter
            }
            color: "white"
        }

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

//        Binding {
//            target: volumeControlSlider
//            property: "value"
//            value: AudioPlayer.volume
//        }

        Slider
        {
            id: volumeControlSlider
            width: 10
            height: 50
            anchors {
                right: parent.right
                top: parent.top;
                topMargin: 10
                rightMargin: 20
            }

            orientation: Qt.Vertical
            value: 0.0

            onValueChanged: {
                if(volumeControlSlider.pressed) {
                    AudioPlayer.volume = value;
                }
            }

            style: SliderStyle {
                    groove: Rectangle {
                        implicitWidth: 100
                        implicitHeight: 7
                        color: "#4F4F4F"
                        radius: 8

                        Rectangle
                        {
                            id: volumeColored
                            color: "light grey"
                            radius: 8
                            anchors {
                                left: parent.left
                                top: parent.top
                                bottom: parent.bottom
                            }
                            width: volumeControlSlider.value * parent.width
                        }
                    }
                    handle: Rectangle {
//                        height: 12
//                        width: 12
//                        radius: 6
//                        color: "grey"
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
                    source: "qrc:///resources/2x/sharp_skip_previous_white_18dp.png";
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
                    source: (AudioPlayer.isPlaying) ? "qrc:///resources/2x/sharp_pause_white_18dp.png" : "qrc:///resources/2x/sharp_play_arrow_white_18dp.png";

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
                    source: "qrc:///resources/2x/sharp_skip_next_white_18dp.png";
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
