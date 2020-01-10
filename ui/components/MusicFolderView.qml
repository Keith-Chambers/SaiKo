import QtQuick 2.0
import QtQuick.Controls 2.4
import QtQuick.Particles 2.11

Item {
    id: root;

    property int activeItemIndex: -10

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

                Popup {
                        id: popup
                        x: 0
                        y: 0

                        dim: false

                        bottomInset: 0;
                        topInset: 0;
                        leftInset: 0;
                        rightInset: 0;

                        padding: 0;
                        margins: 0;

                        width: 200
                        height: 100
                        modal: true
                        focus: true
                        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

                        Rectangle {
                            id: container
                            anchors.fill: parent;
                            color: "grey"

                            Column
                            {
                                id: menuColumn
                                anchors.fill: parent

                                Text {
                                    text: "Edit"
                                    color: "white"
                                }


                                Text {
                                    text: "Else"
                                    color: "white"
                                }
                            }
                        }
                    }

                Rectangle
                {
                    id: editSaikoData
                    visible: activeItemIndex === index;
                    width: 30
                    height: 30
                    color: "black"
                    z:1
                    anchors {
                        left: musicFolderRect.right
                        leftMargin: 2
                        top: musicFolderRect.top
                    }

                    onWidthChanged: {
                        visible = false;
                    }

                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked: {
                            console.log("clicked");
                            MFileSys.generateSaikoInCurrentLibView(model.modelData.itemName);
                        }
                    }
                }

                Rectangle
                {
                    id: musicFolderRect
                    height: 150;
                    width: 150;
                    anchors {
                        left: parent.left
//                        right: parent.right
//                        leftMargin: 20;
//                        rightMargin: 20;
                        top: parent.top
                        topMargin: 20

                    }

                    Canvas
                    {
                        id: saikoOptions
                        visible: false
                        z: 1
                        anchors {
                            left: parent.left
                            top: parent.top
                        }

                        height: 40
                        width: 40

                        onPaint: {
                            var context = getContext("2d");

                            // the triangle
                            context.beginPath();
                            context.moveTo(0, 0);
                            context.lineTo(40, 0);
                            context.lineTo(0, 40);
                            context.closePath();

                            // the fill color
                            context.fillStyle = "black";
                            context.fill();
                        }
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
                        id: musicFolderRectMouse
                        anchors.fill: parent;

                        acceptedButtons: Qt.LeftButton | Qt.RightButton

                        onClicked:
                        {
                            if(mouse.button === Qt.LeftButton) {
                                console.log("Enter dir : " + model.modelData.itemName);
                                MFileSys.pushLibraryViewPosition(index);
                                MFileSys.invokeFolder(model.modelData.itemName);
                                return;
                            }
                        }

                        hoverEnabled: true;
                        onHoveredChanged: {
                            if(editSaikoData.visible === false && containsMouse === true) {
//                                editSaikoData.visible = true;
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
                    width: 150

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
