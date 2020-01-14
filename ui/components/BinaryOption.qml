import QtQuick 2.0
import QtQuick.Controls 2.4

Item
{
    id: root

    function onInvoked(){}  // Dummy implementation to be overloaded
    property bool enabled: false    // Bind this

    property string tooltipDescription: ""  // Leave empty to disable tooltips
    property string iconPath: ""
    property int margin: 5
    property int radius: 3

    Rectangle
    {
        id: optionButtonContainer
        anchors.fill: parent
        radius: root.radius
        color: (root.enabled) ? "#26282b" : "transparent"

        ToolTip
        {
            parent: optionButtonContainer
            visible: tooltipDescription !== "" && optionMouseArea.containsMouse
            text: tooltipDescription
        }

        Image
        {
            id: optionIcon
            anchors.centerIn: parent
            sourceSize.height: root.height - root.margin
            sourceSize.width: root.width - root.margin
            source: root.iconPath
        }

        MouseArea
        {
            id: optionMouseArea
            hoverEnabled: true
            anchors.fill: parent
            onClicked: {
                onInvoked();
            }

            onHoveredChanged: {
                if(root.enabled || containsMouse) {
                    optionButtonContainer.color = "#26282b"
                } else {
                    optionButtonContainer.color = "transparent"
                }
            }
        }
    }
}
