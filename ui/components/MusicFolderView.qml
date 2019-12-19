import QtQuick 2.0
import QtQuick.Controls 2.4

Item {
    id: root;

    GridView
    {
        id: libraryView
        cellWidth: 200;
        cellHeight: 200;
        clip: true;

        anchors.fill: parent;
        ScrollBar.vertical: ScrollBar {}

        Component
        {
            id: fileDelegate

            Item
            {
                id: musicFolderItemContainer
                width: 190;
                height: 190;

                Rectangle
                {
                    id: musicFolderRect
                    height: 150;
                    anchors {
                        left: parent.left
                        right: parent.right
                        leftMargin: 20;
                        rightMargin: 20;
                        top: parent.top
                    }

                    color: "grey"

                    Image
                    {
                        id: folderImage
                        anchors {
                            top: parent.top;
                            left: parent.left
                            right: parent.right
                        }
                        height: parent.height

                        asynchronous: true
                        sourceSize.width: 150
                        sourceSize.height: 150
                        smooth: false

                        source:
                        {
                            if(model.modelData.imagePath !== "")
                                return model.modelData.imagePath;
                            else
                                return "qrc:///resources/cover.jpg";
                        }
                    }

                    MouseArea
                    {
                        anchors.fill: parent;
                        onClicked:
                        {
                            console.log("Enter dir : " + model.modelData.itemName);
                            MFileSys.pushLibraryViewPosition(index);
                            MFileSys.invokeFolder(model.modelData.itemName);
                        }
                    }
                }

                Text
                {
                    anchors {
                        top: musicFolderRect.bottom
                        topMargin: 5;
                        left: parent.left
                        right: parent.right
                    }
                    color: "white";


                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: model.modelData.itemName
                    elide: Text.ElideRight
                }

            }
        }

        onModelChanged: {
            console.log("Model changed!");

            if(MFileSys.restoreLibraryViewPosition == true)
            {
                console.log("Restoring view");
                var savedIndex = MFileSys.popLibraryViewPosition();
                console.log("View set to " + savedIndex);
                positionViewAtIndex(savedIndex, GridView.Center);
                MFileSys.restoreLibraryViewPosition = false;
            }
        }

        model: LibraryView
        delegate: fileDelegate
    }
}
