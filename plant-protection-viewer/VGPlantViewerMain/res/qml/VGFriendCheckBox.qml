import QtQuick 2.0
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.4

/*
  * 封装的组件：只有图标的按钮
  *
*/
RadioButton {
    property string  checkedName:      ""
    property string  uncheckedName:      ""
    id: control
    text: qsTr("")
    anchors.horizontalCenter:parent.horizontalCenter
    spacing: 0
    enabled: true
    background:red

    indicator:
        Image {
        id: image
        height: 32
        width: 32
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter:parent.horizontalCenter
        source: control.checked ? checkedName : uncheckedName
    }
}
