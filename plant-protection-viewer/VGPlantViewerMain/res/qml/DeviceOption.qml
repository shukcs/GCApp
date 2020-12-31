import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Controls 2.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1

import VGGroundControl   1.0

Rectangle {
    id:                             root
    anchors.fill:                   parent
    color:                          "#1f000000"

    MouseArea{
        anchors.rightMargin: 0
        anchors.fill:               parent
        onClicked:                  {}
        onWheel:                    {}
    }
    Component {
        id: tipCom
        VGTip {
            strTitle:   qsTr("Tip")//qsTr("提示")
            strTip:     qsTr("Ensure modify P900 work mode?")//qsTr("是否更改P900工作模式？")
            onFinished: {
                if (accept)
                    surveyMonitor.p900Mode = !surveyMonitor.p900Mode
                else if(surveyMonitor.p900Mode)
                    rbP2P.checked = true
                else
                    rbP2M.checked = true
            }
        }
    }
    Rectangle{
        id:                     contentRect
        anchors.centerIn:       parent
        width:                  parent.width*2/3
        height:                 lyContent.height
        color:                  vgMainPage.backColor
        radius:                 4
        Column{
            id:                 lyContent
            width:              parent.width
            anchors.centerIn:   parent
            Rectangle {
                width:  parent.width
                height: txtHeader.contentHeight*2
                color: "transparent"
                Text{
                    id:                 txtHeader
                    anchors.centerIn:   parent
                    text:               qsTr("Survey station setting")//qsTr("测绘站配置")
                    font:               vgMainPage.biggerFont(true)
                    color:              "#0b81ff"
                }
                Rectangle{
                    height: 1
                    width:  parent.width
                    anchors.bottom: parent.bottom
                    color: "#C8C8C8"
                }
            }
            Rectangle {
                color:      "transparent"
                height:     sysMod.height+rowStation.height+14
                width:      parent.width
                Text{
                    id:     sysMod
                    anchors {left: parent.left; leftMargin: 10; top:parent.top; topMargin: 5}
                    text:   qsTr("Survey station mode")    //测绘站模式
                    font:   vgMainPage.font(true)
                }
                RowLayout {
                    id:     rowStation
                    anchors {top: sysMod.bottom; topMargin: 4; left: sysMod.left; leftMargin: 6}
                    spacing: 10
                    enabled:    surveyMonitor.connected
                    ExclusiveGroup { id: groupSys }
                    VGRadioButton {
                        text: qsTr("Survey station")//qsTr("测绘站")
                        ftText: vgMainPage.font()
                        exclusiveGroup: groupSys
                        checked: !surveyMonitor.sysMode
                        onClicked: {
                            if (checked)
                                surveyMonitor.sysMode = false
                        }
                    }
                    VGRadioButton {
                        text: qsTr("Base station")//qsTr("基站")
                        ftText: vgMainPage.font()
                        exclusiveGroup: groupSys
                        checked: surveyMonitor.sysMode
                        onClicked: {
                            if (checked)
                                surveyMonitor.sysMode = true
                        }
                    }
                }
            }
            Rectangle {
                color:      "transparent"
                height:     txtWork.height+rowWork.height+14
                width:      parent.width
                Text{
                    id:     txtWork
                    anchors {left: parent.left; leftMargin: 10; top:parent.top; topMargin: 5}
                    text:   qsTr("P900 work stat:")    //P900工作模式
                    font:   vgMainPage.font(true)
                }
                RowLayout {
                    id:     rowWork
                    anchors {top: txtWork.bottom; topMargin: 4; left: txtWork.left; leftMargin: 6}
                    spacing: 10
                    enabled:    surveyMonitor.connected
                    ExclusiveGroup { id: groupWork }
                    VGRadioButton {
                        id: rbP2M
                        text: qsTr("Point to group")//qsTr("点对多")
                        ftText: vgMainPage.font()
                        exclusiveGroup: groupWork
                        checked: !surveyMonitor.p900Mode
                        onCheckedChanged: {
                            if (checked && surveyMonitor.p900Mode)
                                vgMainPage.curQmlPage = tipCom.createObject(root)
                        }
                    }
                    VGRadioButton {
                        id: rbP2P
                        text: qsTr("Point to point")//qsTr("点对点")
                        ftText: vgMainPage.font()
                        exclusiveGroup: groupWork
                        checked: surveyMonitor.p900Mode
                        onCheckedChanged: {
                            if (checked && !surveyMonitor.p900Mode)
                                vgMainPage.curQmlPage = tipCom.createObject(root)
                        }
                    }
                }
            }
            Rectangle {
                color:      "transparent"
                height:     txtLocAddr.height+rowLoc.height+14
                width:      parent.width
                Text{
                    id:     txtLocAddr
                    anchors {left: parent.left; leftMargin: 10; top:parent.top; topMargin: 5}
                    text:   qsTr("P900 local address")    //P900本机地址
                    font:   vgMainPage.font(true)
                }
                RowLayout {
                    id: rowLoc
                    anchors {top: txtLocAddr.bottom; topMargin: 4; left: txtLocAddr.left; leftMargin: 6}
                    spacing: 10
                    enabled:    surveyMonitor.connected
                    Text{
                        text:   qsTr("Local address:")    //本机地址
                    }
                    Text{
                        text:  surveyMonitor.unitAddr ? 1: "N/A"     //P900本机地址
                        color:  "#0b81ff"
                    }
                }
            }
            Rectangle {
                color:      "transparent"
                height:     txtDstAddr.height+rowDst.height+14
                width:      parent.width
                enabled:    surveyMonitor.connected
                Text{
                    id:     txtDstAddr
                    anchors {left: parent.left; leftMargin: 10; top:parent.top; topMargin: 5}
                    text:   qsTr("P900 Destination address(2-255)")    //P900目标地址(2-255)
                    font:   vgMainPage.font(true)
                }
                RowLayout {
                    id: rowDst
                    anchors {top: txtDstAddr.bottom; topMargin: 4; left: txtDstAddr.left; leftMargin: 6}
                    spacing: 10
                    enabled:    surveyMonitor.connected
                    Text{
                        text:   qsTr("Destination address:")    //目标地址:
                    }
                    SpinBox{
                        id:                 editDest
                        from:               2
                        to:                 255
                        inputMethodHints:   Qt.ImhPreferNumbers
                        value:               surveyMonitor.destAddr     //P900本机地址
                    }
                    VGTextButton {
                        id:             btn
                        strText:        qsTr("Set")//qsTr("设置")
                        onBtnClicked:   surveyMonitor.destAddr = editDest.value
                    }
                }
            }
            Rectangle {
                width:  parent.width
                height: btnClose.height*2
                color: "transparent"
                VGTextButton {
                    id: btnClose
                    anchors {right: parent.right; rightMargin: 15; verticalCenter: parent.verticalCenter}
                    strText: qsTr("Close")//qsTr("关闭")
                    onBtnClicked: vgMainPage.onSigBack()
                }
            }
            Component.onCompleted:{
                imLst.setParamID("MPC_ALT_MODE")
                dgrLst.setParamID("MPC_Z_P")
            }
        }
    }
    Connections{
        target: vgMainPage
        onSigShowPage: {
            root.visible = vgMainPage.isVisible(root)
        }
    }
}
