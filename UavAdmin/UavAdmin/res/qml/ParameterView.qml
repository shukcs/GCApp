import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1

import VGGroundControl   1.0

Rectangle {
    id:                             root
    anchors.fill: parent
    MouseArea{
        anchors.fill:               parent
        onClicked:                  {}
        onWheel:                    {}
    }
    Component {
        id:   tipCom
        VGTip {
            strTitle:   qsTr("Parameters")//qsTr("设置参数")
            bCancel:    false
        }
    }
    function headerChange(idx) {
        flickCnt.visible = idx===0
        rctQX.visible = idx===1
        rctCntlr.visible = idx===2
        if(idx===2)
            controller.step = VGController.Step_None
    }
    Rectangle {
        id:     rctHeader
        width:  parent.width
        height: tabFunc.height
        color: "transparent"
        VGImage {
           id: imgBack
           anchors {left: parent.left; leftMargin:5; verticalCenter:parent.verticalCenter}
           width: 30
           height: width
           iconName: "backb"
           onImgClicked: vgMainPage.onSigBack()
        }
        VGTabHeader {
            id:                     tabFunc
            txtFont:                vgMainPage.font(true)
            anchors {verticalCenter: parent.verticalCenter; left: imgBack.right; leftMargin: 5}
            Component.onCompleted: {
                addTab(qsTr("Uav parameters"))//qsTr("飞机参数")
                addTab(qsTr("QX account"))//qsTr("千寻账号")
                addTab(qsTr("Controller correct"))//qsTr("遥控器校准")
                setCurrentIndex(0)
            }
            onCurrentChanged: headerChange(idx)
        }

        Rectangle{
            anchors.bottom: parent.bottom
            color: "#C8C8C8"
            height: 1
            width:  parent.width
        }
    }
    Flickable {
        id:                     flickCnt
        anchors {top: rctHeader.bottom; topMargin: 6; bottom: rowBtn.top; bottomMargin: 6
            left: parent.left; leftMargin: 10; right: parent.right; rightMargin: 10}
        clip:                   true
        contentHeight:          colCnt.height
        contentWidth:           width
        flickableDirection:     Flickable.VerticalFlick
        visible: true
        Column {
            id: colCnt
            property string strIdEdit: ""
            spacing: 6
            Repeater{
                model: parameters.getParams("undef")
                Rectangle{
                    width:flickCnt.width
                    height: rowShow.height + (paramEdit.visible ? paramEdit.height + 9 : 4)
                    Row {
                        id: rowShow
                        anchors {left: parent.left; leftMargin: 2; top:parent.top; topMargin: 2}
                        spacing: 15
                        Text {
                            id:     nameTxt
                            height: contentHeight*1.5
                            text:   parameters.getName(modelData)+":"
                            font:   vgMainPage.font(true)
                            verticalAlignment: Text.AlignVCenter
                        }
                        Text {
                            id:     txtVal
                            height: nameTxt.height
                            text:   plantManager.getParamValue(modelData)
                            font:   vgMainPage.font()
                            color:  "#0b81ff"
                            verticalAlignment: Text.AlignVCenter
                        }
                        Text {
                            height:     nameTxt.height
                            visible:    parameters.isScope(modelData)
                            text:       "(" + parameters.getLeast(modelData) + "~~" + parameters.getBiggest(modelData)+")"
                            font:       vgMainPage.font()
                            color:      "#ff0b81"
                            verticalAlignment: Text.AlignVCenter
                        }
                        VGTextButton {
                            id: btn
                            height:     nameTxt.height
                            enabled:    plantManager.connected
                            visible:    !parameters.isParamReadonly(modelData) && !paramEdit.visible
                            strText:    qsTr("Modify")//qsTr("修改")
                            onBtnClicked: colCnt.strIdEdit = modelData
                        }
                        Connections{
                            target: plantManager
                            onParamChanged: {
                                if (modelData !== idStr)
                                    return
                                txtVal.text = plantManager.getParamValue(idStr)
                            }
                        }
                    }
                    VGParamEdit {
                        id:      paramEdit
                        visible: colCnt.strIdEdit === modelData
                        bEdit:   true
                        anchors  {top:rowShow.bottom; topMargin: 5; left: parent.left; leftMargin: 12}
                        Component.onCompleted: setParamID(modelData)
                        onFinishEdit: {
                            if (bSelect || plantManager.setParameter(modelData, str)) {
                                colCnt.strIdEdit = ""
                                return
                            }
                            vgMainPage.curQmlPage = tipCom.createObject(root)
                            vgMainPage.curQmlPage.strTip = qsTr("Range:") + parameters.getLeast(modelData) + "~~\r\n" + parameters.getBiggest(modelData)+qsTr("!")//qsTr("参数范围：");qsTr("！")
                        }
                    }
                }
            }
        }
    }
    Rectangle{
        id: rctQX
        anchors {top: rctHeader.bottom; bottom: rowBtn.top; left: parent.left; right: parent.right}
        visible: false
        Row{
            id: rowAcc
            visible: !flickCnt.visible
            anchors {top: parent.top; topMargin: 20; horizontalCenter: parent.horizontalCenter}
            height: txtAcc.contentHeight*1.2
            spacing: 10
            Text{
                id:                 txtAcc
                text:               qsTr("QX account:")//qsTr("千寻账号：")
                font:               vgMainPage.font()
                height:             parent.height
                verticalAlignment:  Text.AlignVCenter
            }
            VGTextField {
                id: editAcc
                height:             parent.height
                bBorder:            true
                text:               plantManager.currentPlant ? plantManager.currentPlant.qxacc : ""
            }
        }
        Row{
            id: rowPswd
            visible: !flickCnt.visible
            anchors {top: rowAcc.bottom; topMargin: 10; horizontalCenter: parent.horizontalCenter}
            height: txtAcc.contentHeight*1.2
            spacing: 10
            Text{
                id:                 txtPswd
                text:               qsTr("QX password:")//qsTr("千寻密码：")
                font:               vgMainPage.font()
                height:             parent.height
                verticalAlignment:  Text.AlignVCenter
            }
            VGTextField {
                id: editPswd
                height:             parent.height
                bBorder:            true
                text:               plantManager.currentPlant ? plantManager.currentPlant.qxpswd : ""
            }
        }
    }
    Rectangle {
        id:     rctCntlr
        anchors {top: rctHeader.bottom; bottom: rowBtn.top; left: parent.left; right: parent.right}
        visible: false
        Grid{
            id:     grid
            anchors {top: parent.top; topMargin: 20; horizontalCenter: parent.horizontalCenter}
            columns: 2
            columnSpacing: 15
            rowSpacing:     6
            Label{
                height:     20
                text:       controller.rollStr+":"
                font:       vgMainPage.font()
                verticalAlignment : Text.AlignVCenter|Text.Left
            }
            VGSlider{
                height: 20
                width: rctCntlr.width*1/2
                value: controller.roll
                stepSize: 1
                maximumValue:   100
                minimumValue:   -100
            }
            Label{
                height:     20
                text:       controller.pitchStr+":"
                font:       vgMainPage.font()
                verticalAlignment : Text.AlignVCenter|Text.Left
            }
            VGSlider{
                height: 20
                width: rctCntlr.width*1/2
                value: controller.pitch
                stepSize: 1
                maximumValue:   100
                minimumValue:   -100
            }
            Label{
                height:     20
                text:       controller.yawStr +":"
                font:       vgMainPage.font()
                verticalAlignment : Text.AlignVCenter|Text.Left
            }
            VGSlider{
                height: 20
                width: rctCntlr.width*1/2
                value: controller.yaw
                stepSize: 1
                maximumValue:   100
                minimumValue:   -100
            }
            Label{
                height:     20
                text:       controller.throttleStr+":"
                font:       vgMainPage.font()
                verticalAlignment : Text.AlignVCenter|Text.Left
            }
            VGSlider{
                height: 20
                width: rctCntlr.width*1/2
                value: controller.throttle
                stepSize: 1
                maximumValue:   100
                minimumValue:   -100
            }
        }
        Label{
            anchors {top: grid.bottom; topMargin: 15; left: grid.left}
            text:       controller.stepTip
            font:       vgMainPage.biggerFont()
            color:      "#4f1429"
        }
    }
    Row{
        id:         rowBtn
        anchors    {bottom: parent.bottom; bottomMargin: 15; right: parent.right; rightMargin: 20}
        spacing:    12

        VGTextButton {
            strText:        qsTr("Get")//qsTr("获取")
            visible:        rctQX.visible
            onBtnClicked:   plantManager.getQxAccount()
        }
        VGTextButton {
            strText:        qsTr("Clear")//qsTr("清除")
            visible:        rctQX.visible
            onBtnClicked:   plantManager.clearQxAccount()
        }
        VGTextButton {
            strText:        qsTr("Setting")//qsTr("设置")
            visible:        rctQX.visible
            onBtnClicked:   plantManager.setQxAccount(editAcc.text, editPswd.text)
        }
        VGTextButton {
            strText:        qsTr("Start")//qsTr("开始")
            enabled:        controller.step!==VGController.Step_Trim
            visible:        rctCntlr.visible
            onBtnClicked:   controller.step = VGController.Step_Trim
        }
        VGTextButton {
            strText:        qsTr("Next")//qsTr("下一步")
            enabled:        controller.step===VGController.Step_Trim
            visible:        rctCntlr.visible
            onBtnClicked:   controller.step = VGController.Step_Tip
        }
        VGTextButton {
            strText:        qsTr("Correct")//qsTr("校准")
            enabled:        controller.step===VGController.Step_Tip
            visible:        rctCntlr.visible
            onBtnClicked:   controller.step = VGController.Step_Finish
        }
        VGTextButton {
            strText:      qsTr("Close")//qsTr("关闭")
            onBtnClicked: vgMainPage.onSigBack()
        }
    }
    Connections{
        target: vgMainPage
        onSigShowPage: {
            root.visible = vgMainPage.isVisible(root)
        }
        onSigBack: {
            if (root === vgMainPage.curQmlPage)
                vgMainPage.onSigBack()
        }
    }
}
