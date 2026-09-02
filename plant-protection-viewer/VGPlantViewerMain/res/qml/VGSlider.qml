import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1


Slider{
    id:           slider
    implicitHeight:             40
    implicitWidth:              100
    updateValueWhileDragging:   false

    property color colHover:    "blue"
    property color colHoverPre: "#0b81ff"
    property color colHoverbd:  "#A0CEFF"
    property color colSld:      "gray"
    style: SliderStyle{
        groove: Rectangle{
            width:  slider.width
            height: 6
            anchors.centerIn: slider.centerIn
            color:  colSld
            radius: 3
        }
        handle: Rectangle{
            anchors.verticalCenter: parent.verticalCenter
            color: control.pressed ? colHover : colHoverPre
            border {color: colHoverbd; width: 4}
            width:  slider.height-4
            height: width
            radius: width/2
        }
    }
}
