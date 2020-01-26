import QtQuick 2.0
import QtQuick.Controls 2.4
import QtQuick.Dialogs 1.3

Item
{
    id: root;

    property int activeItemIndex: -1

    function calculateCellWidth()
    {
        var viewWidth = libraryView.width;

        if(MFileSys.getNumberItemsLibraryView() < (viewWidth / 200)) {
            return 200;
        }

        var itemsPerLine = viewWidth / 200;

        itemsPerLine = Math.floor(itemsPerLine);

        console.log("Width -> " + viewWidth);

        if(viewWidth % 200 > 0) {
            itemsPerLine++;
        }

        var resultCellWidth = Math.floor(viewWidth / itemsPerLine);

        console.log("Items per line -> " + itemsPerLine);

        console.log("Grid cell width -> " + resultCellWidth);

        return resultCellWidth;
    }

    FileDialog
    {
        id: selectLibraryFolderDialog
        title: "Please choose a file"
        folder: "/"
        selectFolder: true
        selectMultiple: false

        onAccepted:
        {
            console.log("You chose: " + selectLibraryFolderDialog.fileUrls)
            MFileSys.addLibraryRootPath(selectLibraryFolderDialog.fileUrls[0]);
        }
    }

    GridView
    {
        id: libraryView
        cellWidth: 200
        cellHeight: libraryView.cellWidth

        clip: true;

        onWidthChanged: {
            cellWidth = calculateCellWidth();
        }

        anchors.fill: parent;
        ScrollBar.vertical: ScrollBar {}

        Component
        {
            id: fileDelegate

            Item
            {
                id: musicFolderItemContainer
                width: libraryView.cellWidth - 20;
                height: libraryView.cellHeight - 20;

                Rectangle {
                    anchors.fill: parent
                    anchors.rightMargin: 8

//                    color: "yellow"
                    opacity: 0.0

                    MouseArea
                    {
                        id: outerMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onHoveredChanged: {
                            if(containsMouse === false) {
                                activeItemIndex = -1;
                            }
                        }
                    }
                }

                Rectangle
                {
                    id: regenSaikoData
                    visible: MFileSys.editMode && activeItemIndex === index;
                    width: 30
                    height: 30
                    color: "transparent"
                    z:1
                    anchors {
                        left: musicFolderRect.right
                        leftMargin: 2
                        top: musicFolderRect.top
                    }

                    onWidthChanged: {
                        visible = false;
                    }

                    Image
                    {
                        id: reloadSaikoImage
                        anchors.fill: parent
                        sourceSize.width: 30
                        sourceSize.height: 30
                        smooth: false

                        source: "qrc:///resources/2x/sharp_loop_white_18dp.png"
                    }

                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked: {
                            console.log("clicked");

                            MFileSys.pushLibraryViewPosition(index);
                            MFileSys.restoreLibraryViewPosition = true;
                            MFileSys.generateSaikoInCurrentLibView(model.modelData.itemName);
                        }
                    }
                }

                Rectangle
                {
                    id: editSaikoData
                    visible: MFileSys.editMode && activeItemIndex === index;
                    width: 28
                    height: 28
                    color: "transparent"
                    z:1
                    anchors {
                        left: musicFolderRect.right
                        leftMargin: 2
                        top: regenSaikoData.bottom
                        topMargin: 5
                    }

                    Image
                    {
                        id: editSaikoImage
                        anchors.fill: parent
                        sourceSize.width: 28
                        sourceSize.height: 28
                        smooth: false
                        source: "qrc:///resources/2x/sharp_create_white_18dp.png"
                    }

                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked: {
                            console.log("clicked edit");

//                            MFileSys.pushLibraryViewPosition(index);
//                            MFileSys.restoreLibraryViewPosition = true;
//                            MFileSys.generateSaikoInCurrentLibView(model.modelData.itemName);
                        }
                    }
                }

                Rectangle
                {
                    id: musicFolderRect
                    height: libraryView.cellHeight - 50;
                    width: libraryView.cellWidth - 50;
                    anchors {
                        left: parent.left
                        top: parent.top
                        topMargin: 20
                    }

                    color: "transparent"

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
                        sourceSize.width: parent.width
                        sourceSize.height: parent.height
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
                        id: musicFolderRectMouse
                        anchors.fill: parent;

                        acceptedButtons: Qt.LeftButton | Qt.RightButton

                        onClicked:
                        {
                            // TODO: Fix hack
                            if(MFileSys.isHomeDirectory && itemName === "") {
                                selectLibraryFolderDialog.open();
                                return;
                            }

                            if(mouse.button === Qt.LeftButton) {
                                MFileSys.restoreLibraryViewPosition = false;
                                MFileSys.pushLibraryViewPosition(index);
                                MFileSys.invokeFolder(model.modelData.itemName);
                                return;
                            }
                        }

                        hoverEnabled: MFileSys.editMode || MFileSys.isHomeDirectory;
                        onHoveredChanged: {

                            if(containsMouse && MFileSys.isHomeDirectory) {
                                musicFolderRect.color = "dark grey";
                            } else {
                                musicFolderRect.color = "transparent";
                            }

                            if(regenSaikoData.visible === false && containsMouse === true) {
                                activeItemIndex = index
                            }
                        }
                    }
                }

                Text
                {
                    anchors {
                        top: musicFolderRect.bottom
                        horizontalCenter: musicFolderRect.horizontalCenter
                        topMargin: 5;
                    }
                    width: parent.width

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

            libraryView.cellWidth = calculateCellWidth();

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
