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
    minimumHeight: 400
    minimumWidth: 800
    title: qsTr("Saiko");

    property bool showFolderView: true

    Rectangle
    {
        id: displayRoot
        anchors.fill: parent;
        color: "#404040";

        ListModel
        {
            id: optionsModel
            ListElement
            {
                description: "Enable edit mode"
                optionID: 0
                iconPath: "qrc:///resources/2x/sharp_build_white_18dp.png"
            }

            ListElement
            {
                description: "Show tooltips"
                optionID: 1
                iconPath: "qrc:///resources/2x/sharp_info_white_18dp.png"
            }
        }

        Row
        {
            id: optionsRow
            spacing: 10
            height: 25
            anchors {
                top: parent.top
                topMargin: 10
                left: currentFolderText.right
                right: parent.right
                leftMargin: 20
                rightMargin: 40
            }

            BinaryOption
            {
                id: editModeOption

                Binding {
                    target: editModeOption
                    property: "enabled"
                    value: MFileSys.editMode
                }

                width: 30
                height: 30

                iconPath: "qrc:///resources/2x/sharp_build_white_18dp.png";
                tooltipDescription: MFileSys.toolTipsEnabled ? "Enable Edit Mode" : "";

                function onInvoked() {
                    MFileSys.editMode = !MFileSys.editMode;
                }
            }

            BinaryOption
            {
                id: enableToolTipsOption

                Binding {
                    target: enableToolTipsOption
                    property: "enabled"
                    value: MFileSys.toolTipsEnabled;
                }

                width: 30
                height: 30

                iconPath: "qrc:///resources/2x/sharp_info_white_18dp.png";
                tooltipDescription: "Enable Tool Tips";

                function onInvoked() {
                    MFileSys.toolTipsEnabled = !MFileSys.toolTipsEnabled;
                }
            }
        }

        Text
        {
            id: currentFolderText
            color: "white"
            text: MFileSys.libraryViewDirName
            font.pointSize: 12;
            width: 250

            anchors {
                left: musicFolderView.left
                verticalCenter: optionsRow.verticalCenter
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
            visible: false;
            height: 1
            color: "grey"
            anchors {
                left: sideMenuBackground.right
                top: parent.top
                topMargin: 80
                leftMargin: 50
                right: audioFileListView.left
            }
        }

        Rectangle
        {
            id: backButton;
            anchors
            {
                left: sideMenuBackground.right
                top: parent.top;
                leftMargin: 5
                topMargin: 10
            }

            width: 25;
            height: 25
            color: "transparent";

            Image
            {
                sourceSize.height: 25;
                sourceSize.width: 25;
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
                top: currentFolderText.bottom;
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
            horizontalAlignment: Text.AlignRight
            width: 300
            anchors
            {
                verticalCenter: optionsRow.verticalCenter
                right: closeFileViewContainer.left
                rightMargin: 5
            }
            font.pointSize: 11
        }

        AudioFileListView
        {
            id: audioFileListView
            width: (MFileSys.audioViewDirName == "" || showFolderView == false) ? 0 : 250;

            anchors
            {
                top: audioFolderName.bottom
                topMargin: 10
                bottom: audioControls.top;
                bottomMargin: 0;
                right: parent.right;
                rightMargin: (MFileSys.audioViewDirName == "" || showFolderView == false) ? 0 : 15;
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

            // Playlist Widget
            GridView
            {
                id: playlistGridView
//                model: playlistModel
                model: Playlists
                cellWidth: (nowPlayingImageRect.width / 4)
                cellHeight: 40 // cellWidth

                width: 180 // (sideMenuBackground.width / 2) - (10 * 2)
                height: 50
                interactive: false

                anchors {
                    left: nowPlayingImageRect.left
                    bottom: sideMenuBackground.bottom
                    bottomMargin: 0
                }

                delegate: Rectangle
                {
                    id: playlistDelegate
                    width: 35
                    height: 35
                    border {
                        width: 1
                        color: "black"
                    }

                    color: "#404040"

                    Text
                    {
                        anchors.centerIn: parent
                        color: "White"
                        text: name
                    }

                    Text
                    {
                        anchors {
                            bottom: parent.bottom
                            right: parent.right
                            bottomMargin: 1
                            rightMargin: 4
                        }

                        font.pointSize: 9
                        color: "White"
                        text: (numberTracks !== 0) ? numberTracks : "-"
                    }

                    MouseArea
                    {
                        id: playlistItemMouseArea
                        anchors.fill: parent
                        onClicked: {
                            MFileSys.invokePlaylist(index);
                        }
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

//        Image
//        {
//            id: logoImg
//            anchors.left: parent.left
//            anchors.top: parent.top
//            anchors.topMargin: 20;
//            anchors.leftMargin: 50;

//            width: 100
//            height: 70
//            sourceSize.width: 100;
//            sourceSize.height: 70

//            source: "qrc:///resources/saiko_logo.png"
//        }
    }
}
