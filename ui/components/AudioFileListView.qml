import QtQuick 2.0

Item {

    Rectangle {

        anchors.fill: parent;
        color: "#404040"

        Rectangle
        {
            id: beginDividerLine
            height: 1
            color: "light grey"
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
            interactive: false;

            Component
            {
                id: listDelegate

                Rectangle
                {
                    id: audioItemRect
                    height: 30;
                    color: "#404040";

                    anchors {
                        left: parent.left;
                        right: parent.right;
                    }

                    Rectangle
                    {
                        id: dividerLine
                        height: 1
                        color: "light grey"
                        anchors {
                            left: parent.left
                            right: parent.right
                            bottom: parent.bottom
                        }
                    }

                    Text
                    {
                        text: model.modelData.title;
                        color: "white"
                        elide: Text.ElideRight;
                        verticalAlignment: Text.AlignVCenter;
                        anchors {
                            fill: parent
                            leftMargin: 10
                            rightMargin: 10
                        }
                    }

                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked: {
                            MFileSys.invokeMediaItem(model.modelData.itemName, model.modelData.extension);
                            audioListView.currentIndex = index;
                            MFileSys.playFromCurrentAudioSelection(index);
                        }
                    }
                }
            }

            focus: true
            model: CurrentFolderAudioList
            delegate: listDelegate
            highlight: Rectangle {
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
