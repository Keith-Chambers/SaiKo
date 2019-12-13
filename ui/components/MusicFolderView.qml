import QtQuick 2.0
import QtQuick.Controls 2.4

Item {
    id: root;

    GridView
    {
        cellWidth: 200;
        cellHeight: 220;
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
                height: 210;

                Rectangle
                {
                    id: musicFolderRect
                    width: 180;
                    height: 180;
                    color:
                        {
                            (model.modelData.isPlayable) ? "grey" : "dark grey"
                        }

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
                        sourceSize.width: 190
                        sourceSize.height: 190
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
                            MFileSys.invokeMediaItem(model.modelData.itemName, model.modelData.extension);
                        }
                    }
                }

                Text
                {
                    anchors {
                        top: musicFolderRect.bottom
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

        model: MediaList
        delegate: fileDelegate
    }
}
