import QtQuick 2.5
import QtQuick.Controls 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4
import Qt.labs.folderlistmodel 2.1

ApplicationWindow
{
    visible: true
    id: root;
    width: 1100
    height: 600
    minimumHeight: 600
    minimumWidth: 800
    title: qsTr("Saiko");
    color: "red";

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

        GridView
        {
            anchors
            {
                top: parent.top;
                bottom: parent.bottom;
                right: parent.right;
                left: sideMenuBackground.right;
                bottomMargin: 70;
                leftMargin: 50;
                topMargin: 100
            }

            cellWidth: 200
            cellHeight: 200

            Component
            {
                id: fileDelegate
                Rectangle
                {
                    width: 130;
                    height: 130
                    color:
                        {
                            (model.modelData.isPlayable) ? "grey" : "#999FCC"
                        }

                    Text
                    {
                        anchors.fill: parent
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        text: model.modelData.itemName
                        elide: Text.ElideRight
                    }

                    Image
                    {
                        id: folderImage
                        anchors.fill: parent;
                        anchors.margins: 2
                        asynchronous: true
                        sourceSize.width: 130
                        sourceSize.height: 130
                        smooth: false

                        source:
                        {
                            if(model.modelData.imagePath !== "")
                                return model.modelData.imagePath;
                            else
                                return "";
                        }
                    }

                    MouseArea
                    {
                        anchors.fill: parent;
                        onClicked:
                        {
                            console.log("Enter dir : " + model.modelData.itemName);
                            MFileSys.invokeMediaItem(itemName);
                        }
                    }
                }
            }

            model: MediaList
            delegate: fileDelegate
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

            width: 200
            color: "#243D45";

            Rectangle
            {
                id: nowPlayingImageRect;
                anchors
                {
                    bottom: sideMenuBackground.bottom;
                    left: parent.left;
                    rightMargin: 20;
                    leftMargin: 20;
                    bottomMargin: 20;
                    right: parent.right
                }
                height: 150;

                Image
                {
                    id: currentlyPlayingImage
                    source: "qrc:///resources/record.png"
                    anchors.fill: parent;
                }
            }
        }

        Rectangle
        {
            id: bottomBar
            anchors
            {
                top: sideMenuBackground.bottom;
                right: parent.right;
                left: parent.left;
                bottom: parent.bottom;
            }

            color: "#363636";

            Slider
            {
                id: playPosSlider

                anchors
                {
                    top: parent.top;
                    horizontalCenter: parent.horizontalCenter;
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

                value: 0.0;
            }

            RowLayout
            {
                id: audioControlsLayout
                anchors
                {
                    top: parent.top;
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
                        source: (AudioPlayer.getIsPlaying()) ? "qrc:///resources/play.png" : "qrc:///resources/pause.png";

                        MouseArea
                        {
                            id: audioTogglePlayMouse
                            anchors.fill: parent
                            onClicked:
                            {
                                console.log("Clicked");
                                AudioPlayer.togglePause();

                                if(AudioPlayer.getIsPlaying() == true)
                                    audioTogglePlayImage.source = "qrc:///resources/play.png"
                                else
                                    audioTogglePlayImage.source = "qrc:///resources/pause.png"
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
                }
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
