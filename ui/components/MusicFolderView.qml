import QtQuick 2.0

Item {
    id: root;

    GridView
    {
        cellWidth: 150;
        cellHeight: 170;

        anchors.fill: parent;

        Component
        {
            id: fileDelegate

            Item
            {
                id: musicFolderItemContainer
//                opacity: 0.0;
                width: 145;
                height: 165;

                Rectangle
                {
                    id: musicFolderRect
                    width: 145;
                    height: 145;
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
                        sourceSize.width: 150
                        sourceSize.height: 150
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
