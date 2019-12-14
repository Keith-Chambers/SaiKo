import QtQuick 2.5
import QtQuick.Controls 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4
import Qt.labs.folderlistmodel 2.1

import "./components"

ApplicationWindow
{
    visible: true
    id: root;
    width: 1100
    height: 600
    minimumHeight: 600
    minimumWidth: 800
    title: qsTr("Saiko");

    Rectangle
    {
        id: displayRoot
        anchors.fill: parent;
        color: "#404040";

        Text
        {
            id: currentFolderText
            color: "white"
            text: MFileSys.currentFolderName
            font.pointSize: 14;

            anchors {
                left: musicFolderView.left
                leftMargin: 20
                bottom: topDivider.top
                bottomMargin: 0;
            }
        }

        Rectangle
        {
            id: topDivider
            visible: false;
            height: 1
            color: "white"
            anchors {
                left: sideMenuBackground.right
                top: backButton.bottom;
                topMargin: 20;
                leftMargin: 50
//                rightMargin: 50
                right: audioFileListView.left
            }
        }

        Rectangle
        {
            id: backButton;
            anchors
            {
                right: parent.right;
                top: parent.top;
                rightMargin: 10
                topMargin: 10
            }

            width: 30;
            height: 30
            color: "transparent";

            Image
            {
                anchors.fill: parent
                source: "qrc:///resources/back.png";
            }

            MouseArea
            {
                id: backButtonMouse
                anchors.fill: parent;
                onClicked: MFileSys.upDir();
            }
        }

        MusicFolderView
        {
            id: musicFolderView
            anchors
            {
                top: topDivider.bottom;
                topMargin: 20;
                leftMargin: 50;
                rightMargin: 10;
                bottom: audioControls.top;
                left: sideMenuBackground.right;
                right: audioFileListView.left
            }
        }

        AudioFileListView
        {
            id: audioFileListView
            width: 300;

            anchors
            {
                top: backButton.bottom;
                topMargin: 20;
                bottom: audioControls.top;
                bottomMargin: 0;
                right: parent.right;
                rightMargin: 10;
            }
        }

        Rectangle
        {
            id: sideMenuBackground
            anchors
            {
                left: parent.left;
                top: parent.top;
                bottom: parent.bottom;
                bottomMargin: 70;
            }

            width: 250
            color: "#243D45";

            Column
            {
                anchors
                {
                    bottom: nowPlayingImageRect.top;
                    bottomMargin: 10;
                    left: nowPlayingImageRect.left
                    right: parent.right;
                }

                Text
                {
                    id: nowPlayingTitleText
//                    height: 40
                    color: "white";
                    elide: Text.ElideRight
                    font {
                        bold: false;
                    }

                    anchors {
                        left: parent.left;
                        rightMargin: 30;
                        right: parent.right
                    }

                    visible: AudioPlayer.isPlaying

                    text: (AudioPlayer.isPlaying) ? AudioPlayer.currentAudio.title : "";
                }

                Text
                {
                    id: nowPlayingArtistText
//                    height: 40
                    color: "light grey";
                    elide: Text.ElideRight
                    anchors {
                        left: parent.left;
                        rightMargin: 30;
                        right: parent.right
                    }

                    font {
                        bold: true;
                    }
                    visible: AudioPlayer.isPlaying
                    text: (AudioPlayer.isPlaying) ? AudioPlayer.currentAudio.artist : "";
                }
            }

            Rectangle
            {
                id: nowPlayingImageRect;
                anchors
                {
                    bottom: sideMenuBackground.bottom;
                    left: parent.left;
                    leftMargin: 25;
                    bottomMargin: 20;
                }
                height: 200;
                width: 200;

                Image
                {
                    id: currentlyPlayingImage
                    source: {
                        console.log("Checking whether to change image");

                        if(!AudioPlayer.isPlaying) {
                            console.log("Not playing");
                            return "qrc:///resources/cover.jpg";
                        }

                        if(!AudioPlayer.currentAudio.hasArt) {
                            console.log("Current audio has no art");
                            console.log(AudioPlayer.currentAudio.title);
                            return "qrc:///resources/cover.jpg";
                        }

                        console.log("Returning path " + "file:" + AudioPlayer.currentAudio.artPath);

                        return AudioPlayer.currentAudio.artPath;
                    }
                    anchors.fill: parent;
                }
            }
        }

        AudioControls
        {
            id: audioControls;

            anchors
            {
                top: sideMenuBackground.bottom;
                right: parent.right;
                left: parent.left;
                bottom: parent.bottom;
            }
        }

        Image
        {
            id: logoImg
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.topMargin: 20;
            anchors.leftMargin: 50;
            height: 30
            width: 100
            source: "qrc:///resources/saiko.png"
        }
    }
}
