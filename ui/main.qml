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

    property bool showFolderView: true

    Rectangle
    {
        id: displayRoot
        anchors.fill: parent;
        color: "#404040";



        Text
        {
            id: currentFolderText
            color: "white"
            text: MFileSys.libraryViewDirName
            font.pointSize: 14;

            anchors {
                left: musicFolderView.left
                leftMargin: 0
//                bottom: topDivider.top
                bottomMargin: 15;
                bottom: musicFolderView.top
            }
        }

        Rectangle
        {
            id: closeFileViewContainer
            height: 25
            width: 25
            visible: MFileSys.audioViewDirName != ""
            anchors {
                right: parent.right
                rightMargin: 15
//                bottom: audioFolderName.bottom
                verticalCenter: audioFolderName.verticalCenter
            }

            color: "transparent"

            Image
            {
                id: closeFileViewButtonImage
                anchors.fill: parent
                source: "qrc:///resources/2x/sharp_arrow_drop_down_white_18dp.png"
            }

            MouseArea
            {
                anchors.fill: parent
                onClicked: {
                    console.log("Close clicked");
                    showFolderView = !showFolderView;
                }
            }
        }

        Rectangle
        {
            id: topDivider
//            visible: false;
            height: 1
            color: "grey"
            anchors {
                left: sideMenuBackground.right
                top: parent.top
                topMargin: 80
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
                source: "qrc:///resources/2x/sharp_arrow_back_white_18dp.png";
            }

            MouseArea
            {
                id: backButtonMouse
                anchors.fill: parent;
                onClicked: {
                    MFileSys.restoreLibraryViewPosition = true;
                    MFileSys.cdUp();
                }
            }
        }

        MusicFolderView
        {
            id: musicFolderView
            anchors
            {
                top: topDivider.bottom;
                topMargin: 10;
                leftMargin: 50;
                rightMargin: 0;
                bottom: audioControls.top;
                left: sideMenuBackground.right;
                right: audioFileListView.left
            }
        }

        Text
        {
            id: audioFolderName
            text: qsTr( MFileSys.audioViewDirName )
            color: "white"
            elide: Text.ElideRight;
//            width: 225
            anchors {
                bottom: topDivider.top
                bottomMargin: 5
                right: closeFileViewContainer.left
                rightMargin: 5
            }
            font.pointSize: 11
//            font.bold: true
        }

        AudioFileListView
        {
            id: audioFileListView
            width: (MFileSys.audioViewDirName == "" || showFolderView == false) ? 0 : 250;

            anchors
            {
                top: topDivider.bottom;
                topMargin: -1;
                bottom: audioControls.top;
                bottomMargin: 0;
                right: parent.right;
                rightMargin: 15;
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

            width: 210
//            color: "#333c34"
            color: "#26282b"
//            color: "#243D45";

            ListModel {
                id: playlistModel

                ListElement {
                    name: "A"
                }
                ListElement {
                    name: "B"
                }
                ListElement {
                    name: "C"
                }
                ListElement {
                    name: "D"
                }
            }

            // Playlist Widget
            GridView
            {
                id: playlistGridView
                model: playlistModel
                cellWidth: (nowPlayingImageRect.width / 4)
                cellHeight: 40 // cellWidth

                width: 180 // (sideMenuBackground.width / 2) - (10 * 2)
                height: 50
                interactive: false

                anchors {
                    left: nowPlayingImageRect.left
//                    leftMargin: 20
    //                top: logoImg.bottom
    //                topMargin: 70
                    bottom: sideMenuBackground.bottom
                    bottomMargin: 0
                }

                delegate: Rectangle
                {
                    id: playlistDelegate
    //                width: 60 // (parent.width / 4) - 10
    //                height: playlistDelegate.width
    //                anchors.centerIn: parent
    //                anchors.fill: parent
                    width: 35
                    height: 35
                    border {
                        width: 1
                        color: "black"
                    }

                    color: "dark grey"

                    Text
                    {
                        anchors.centerIn: parent
                        color: "black"
                        text: name
                    }
                }

                z: 1
            }

            Column
            {
                id: nowPlayingColumn
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

                    visible: MFileSys.currentPlaylistIndex != -1
                    text: (MFileSys.currentPlaylistIndex != -1) ? MFileSys.currentAudio.title : "";
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
                    visible: MFileSys.currentPlaylistIndex != -1
                    text: (MFileSys.currentPlaylistIndex != -1) ? MFileSys.currentAudio.artist : "";
                }
            }

            Rectangle
            {
                id: nowPlayingImageRect;
                visible: MFileSys.currentPlaylistIndex != -1;
                anchors
                {
                    bottom: playlistGridView.top;
                    left: parent.left;
                    leftMargin: 25;
                    bottomMargin: 15;
                }
                height: 160;
                width: 160;

                Image
                {
                    id: currentlyPlayingImage
                    source: MFileSys.currentAudioImagePath
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
