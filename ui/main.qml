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
                top: parent.top;
                topMargin: 50;
                leftMargin: 10;
                rightMargin: 10;
                bottom: parent.bottom;
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
                bottom: parent.bottom;
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

            Text
            {
                id: nowPlayingTitleText
                height: 40
                visible: AudioPlayer.isPlaying
                anchors
                {
                    bottom: nowPlayingImageRect.top;
                    bottomMargin: 20;
                    left: parent.left
                    leftMargin: 20;
                    right: parent.right;
                }
                text: (AudioPlayer.isPlaying) ? AudioPlayer.currentAudio.title : "";
            }

            Rectangle
            {
                id: nowPlayingImageRect;
                anchors
                {
                    bottom: sideMenuBackground.bottom;
                    left: parent.left;
                    leftMargin: 50;
                    bottomMargin: 20;
                }
                width: 150;
                height: 150;

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
            anchors.leftMargin: 20;
            height: 30
            width: 100
            source: "qrc:///resources/saiko.png"
        }
    }
}
