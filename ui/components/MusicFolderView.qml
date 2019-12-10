import QtQuick 2.0

Item {
    id: root;

    GridView
    {
        cellWidth: 150;
        cellHeight: 150;

        anchors.fill: parent;

        Component
        {
            id: fileDelegate
            Rectangle
            {
                width: 150;
                height: 150;
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
        }

        model: MediaList
        delegate: fileDelegate
    }
}
