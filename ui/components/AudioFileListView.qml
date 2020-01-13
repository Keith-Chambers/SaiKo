import QtQuick 2.0

Item {

    property int activeItemIndex: -1

    Rectangle {

        anchors.fill: parent;
        color: "#404040"

        Rectangle
        {
            id: beginDividerLine
            height: 1
            color: "grey"
            anchors
            {
                left: parent.left
                right: parent.right
                top: parent.top
            }
        }

        ListView
        {
            id: audioListView
            anchors.fill: parent;
            anchors.topMargin: 1
            interactive: true;
            currentIndex: (MFileSys.audioFolderViewIsCurrent) ? MFileSys.currentPlaylistIndex : -1
            clip: true

            Component
            {
                id: listDelegate

                Rectangle
                {
                    id: audioItemRect
                    height: 30;
                    color: "#404040";

                    MouseArea
                    {
                        id: outerMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onHoveredChanged: {
                            if(containsMouse === false) {
                                activeItemIndex = -1;
                            } else {
                                activeItemIndex = index;
                            }
                        }
                    }

                    anchors {
                        left: parent.left;
                        right: parent.right;
                    }

                    Rectangle
                    {
                        id: dividerLine
                        height: 1
                        color: "grey"
                        anchors {
                            left: parent.left
                            right: parent.right
                            bottom: parent.bottom
                        }
                    }

                    Rectangle
                    {
                        id: addToPlaylistContainer
                        visible: activeItemIndex === index;
                        color: "transparent"
                        height: 30
                        width: 30
                        anchors {
                            right: parent.right
                            top: parent.top
                            rightMargin: 5
                        }

                        Image
                        {
                            id: addToPlaylistPlusIcon
                            anchors.fill: parent
                            sourceSize.width: 30
                            sourceSize.height: 30
                            smooth: false
                            source: "qrc:///resources/2x/sharp_add_white_18dp.png"
                        }

                        MouseArea
                        {
                            id: addToPlaylistMouseArea
                            anchors.fill: parent
                            onClicked:
                            {
                                MFileSys.addTrackToPlaylist(0, index);
                            }
                        }
                    }

                    Text
                    {
                        id: audioItemText
                        text: model.modelData.title;
                        color: "white"
                        elide: Text.ElideRight;
                        verticalAlignment: Text.AlignVCenter;
                        anchors {
                            leftMargin: 10
                            left: parent.left
                            top: parent.top
                            bottom: parent.bottom
                            rightMargin: 5
                            right: addToPlaylistContainer.left
                        }
                    }

                    MouseArea
                    {
                        anchors.fill: audioItemText
                        onClicked: {
                            MFileSys.invokeAudioListing(index);
                        }
                    }
                }
            }

            focus: true
            model: AudioView
            delegate: listDelegate
            highlight: Rectangle {
                visible: MFileSys.audioFolderViewIsCurrent && MFileSys.currentPlaylistIndex != -1
                enabled: visible
                color: "light grey"
                opacity: 0.2
                anchors {
                    left: parent.left
                    right: parent.right
                }

                z: 2
            }
        }

    }
}
