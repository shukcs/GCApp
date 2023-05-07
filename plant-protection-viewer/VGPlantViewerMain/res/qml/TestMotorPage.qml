import QtQuick 2.3
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import VGGroundControl   1.0

Rectangle
{
    id:             root
    anchors.fill:   parent
    MouseArea{
        anchors.fill:       parent
        onClicked:          {}
        onWheel:            {}
    }
    Rectangle {
        id:     rctHeader
        width:  parent.width
        height: txtHeader.contentHeight*2
        color: "transparent"
        VGImage {
            id: imgBack
            anchors {left: parent.left; leftMargin:5; verticalCenter:parent.verticalCenter}
            width: 30
            height: width
            iconName: "backb"
            onImgClicked: vgMainPage.onSigBack()
        }
        Text {
            id:                 txtHeader
            anchors.centerIn:   parent
            text:               qsTr("Test motor")//qsTr("测试电机")
            font:               vgMainPage.biggerFont(true)
            color:              "#0b81ff"
        }
        Rectangle{
            anchors.bottom: parent.bottom
            color: "#C8C8C8"
            height: 1
            width:  parent.width
        }
    }
    Row{
        id:          rowLst
        spacing:     10
        anchors      {horizontalCenter: parent.horizontalCenter; top:rctHeader.bottom; topMargin: 3}
        ExclusiveGroup { id: vcType }
        VGRadioButton {
            id:			kg20
            text:       qsTr("20Kg")
            checked:    true
            ftText:     vgMainPage.font()
            exclusiveGroup: vcType
        }
        VGRadioButton {
            id:			kg10
            text:       qsTr("10Kg")
            checked: false
            ftText:     vgMainPage.font()
            exclusiveGroup: vcType
        }
        VGRadioButton {
            id:			kg5
            text:       qsTr("5Kg")
            checked:    false
            ftText:     vgMainPage.font()
            exclusiveGroup: vcType
        }
    }
    Rectangle{
        id:     sep1
        anchors{top:rowLst.bottom; topMargin: 3; left:parent.left; right: parent.right}
        height: 1
        color: "#C8C8C8"
    }
    Rectangle {
        id: tmRct
        anchors {top: sep1.bottom; horizontalCenter: parent.horizontalCenter}
        width:  parent.width
        height: txtTime.height+6
        Label{
            anchors {verticalCenter: parent.verticalCenter;left: parent.left; leftMargin: 20}
            text:                       qsTr("Rotate time(s):")//qsTr("转动时间(S):")
            font:                       vgMainPage.font()
            color:                      "gray"
        }
        VGTextField{
            id:                         txtTime
            anchors {verticalCenter: parent.verticalCenter; right: parent.right; rightMargin: 20}
            width:                      parent.width*2/3
            font:                       vgMainPage.font()
            inputMethodHints:           Qt.ImhPreferNumbers
            text:                       "1"
            horizontalAlignment:        TextInput.AlignHCenter
            validator:                  DoubleValidator{bottom: 0; decimals:2}
            onTextChanged:              txtConfirm.enabled = (text.length>0)
        }
    }
    Rectangle{
        id:     sep2
        height: 1
        width:  parent.width
        anchors{top:tmRct.bottom; left:parent.left; right: parent.right}
        color: "#C8C8C8"
    }
    Rectangle {
        id:     spRct
        width:  parent.width
        height: txtSpeed.height+6
        anchors {top: sep2.bottom; horizontalCenter: parent.horizontalCenter}
        Label{
            anchors {verticalCenter: parent.verticalCenter; left: parent.left; leftMargin: 20}
            text:                       qsTr("PWM width(uS):")//PWM脉宽(uS):
            font:                       vgMainPage.font()
            color:                      "gray"
        }
        VGTextField{
            id:                         txtSpeed
            anchors {verticalCenter:  parent.verticalCenter; right: parent.right;rightMargin: 20}
            width:                      parent.width*2/3
            font:                       vgMainPage.font()
            inputMethodHints:           Qt.ImhPreferNumbers
            text:                       "1150"
            horizontalAlignment:        TextInput.AlignHCenter
            validator:                  DoubleValidator{bottom: 0; decimals:2}
            onTextChanged:              txtConfirm.enabled = (text.length>0)
        }
    }
    Rectangle{
        id:     sep3
        height: 1
        anchors{top:spRct.bottom; left:parent.left; right: parent.right}
        color: "#C8C8C8"
    }
    Column{
        id: colMt
        anchors {top: sep3.bottom; topMargin: 10; horizontalCenter: parent.horizontalCenter; bottom: btnClose.top; bottomMargin: 10}
        property real impRule: width<height?width:height
        Rectangle {
            width:      root.width
            height:     parent.height
            visible:    kg20.checked
            VGParamView{
                anchors    {left:cw2.left; top:parent.verticalCenter; topMargin: -colMt.impRule/2}
                icSz:       38
                iconName:   "ccw"
                strText:    qsTr("Motor 3")//qsTr("电机1")
            }
            VGParamView{
                anchors{top: ccw4.top; left: parent.horizontalCenter; leftMargin: -colMt.impRule/2}
                icSz:       38
                iconName:   "cw"
                strText:   qsTr("Motor 2")
            }
            VGParamView{
                anchors{left:cw2.left; bottom: parent.verticalCenter; bottomMargin: -colMt.impRule/2}
                icSz:       38
                iconName:   "ccw"
                strText:   qsTr("Motor 4")
            }
            VGParamView{
                opacity: 0.8
                anchors{top: ccw4.top; right: parent.horizontalCenter; rightMargin: -colMt.impRule/2}
                icSz:       38
                iconName:   "cw"
                strText:   qsTr("Motor 1")
            }
            VGParamView{
                id:     cw2
                anchors{horizontalCenter: parent.horizontalCenter; top:parent.verticalCenter; topMargin: -colMt.impRule/4}
                icSz:       38
                iconName:   "cw"
                strText:   qsTr("Motor 7")
            }
            VGParamView{
                id:     ccw4
                anchors {verticalCenter:parent.verticalCenter; left:parent.horizontalCenter; leftMargin: -colMt.impRule/4}
                icSz:       38
                iconName:   "ccw"
                strText:   qsTr("Motor 6")
            }
            VGParamView{
                id:     cw6
                anchors{left:cw2.left; bottom: parent.verticalCenter; bottomMargin: -colMt.impRule/4}
                icSz:       38
                iconName:   "cw"
                strText:   qsTr("Motor 8")
            }
            VGParamView{
                id:     ccw8
                anchors {top: ccw4.top; right:parent.horizontalCenter; rightMargin: -colMt.impRule/4}
                icSz:       38
                iconName:   "ccw"
                strText:   qsTr("Motor 5")
            }
        }
        Rectangle {
            width:      root.width
            height:     parent.height
            visible:    kg10.checked
            VGParamView{
                id:     cw10_2
                anchors {top:parent.top; topMargin:10; right: parent.horizontalCenter;rightMargin:(colMt.impRule-20)/4-width/2}
                icSz:       38
                iconName:   "cw"
                strText:   qsTr("Motor 3")
            }
            VGParamView{
                id:     cw10_1
                anchors{top:cw10_2.top; left: parent.horizontalCenter; leftMargin:(colMt.impRule-20)/4-width/2}
                icSz:       38
                iconName:   "ccw"
                strText:   qsTr("Motor 5")
            }
            VGParamView{
                anchors{bottom:parent.bottom; bottomMargin:10; left: cw10_2.left}
                icSz:       38
                iconName:   "cw"
                strText:   qsTr("Motor 6")
            }
            VGParamView{
                anchors{bottom:parent.bottom; bottomMargin:10; left: cw10_1.left}
                icSz:       38
                iconName:   "ccw"
                strText:   qsTr("Motor 4")
            }
            VGParamView{
                anchors{verticalCenter:parent.verticalCenter; left:parent.horizontalCenter; leftMargin: (colMt.impRule-20)/2-width/2}
                icSz:       38
                iconName:   "cw"
                strText:   qsTr("Motor 1")
            }
            VGParamView{
                anchors{verticalCenter:parent.verticalCenter; right:parent.horizontalCenter; rightMargin: (colMt.impRule-20)/2-width/2}
                icSz:       38
                iconName:   "ccw"
                strText:   qsTr("Motor 2")
            }
        }
        Rectangle {
            width:      root.width
            height:     parent.height
            visible:    kg5.checked
            VGParamView{
                id:     ccw1
                anchors {left: parent.horizontalCenter; bottom: parent.verticalCenter; leftMargin: colMt.impRule/6; bottomMargin: colMt.impRule/6}
                icSz:       38
                iconName:   "ccw"
                strText:   qsTr("Motor 1")
            }
            VGParamView{
                id:     ccw3
                anchors{top: parent.verticalCenter;right: parent.horizontalCenter; rightMargin: colMt.impRule/6; topMargin: colMt.impRule/6}
                icSz:       38
                iconName:   "ccw"
                strText:   qsTr("Motor 2")
            }
            VGParamView{
                anchors{top:ccw1.top; left: ccw3.left}
                icSz:       38
                iconName:   "cw"
                strText:   qsTr("Motor 3")
            }
            VGParamView{
                anchors{top:ccw3.top; left: ccw1.left}
                icSz:       38
                iconName:   "cw"
                strText:   qsTr("Motor 4")
            }
        }
    }
    VGTextButton {
        id: btnClose
        anchors {right: parent.right; rightMargin: 25; bottom:parent.bottom; bottomMargin: 13}
        strText: qsTr("Test motor")
        onBtnClicked: plantManager.testMotor(0,txtTime.text * 1000,txtSpeed.text);
    }
    Connections {
        target:         vgMainPage
        onSigBack:      if (page===root)vgMainPage.onSigBack()
        onSigShowPage:  root.visible = vgMainPage.isVisible(root)
    }
}

