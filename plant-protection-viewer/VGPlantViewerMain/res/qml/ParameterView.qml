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
                addTab(qsTr("Uav parameters"))//"飞机参数"
                addTab(qsTr("QX account"))//"千寻账号"
                addTab(qsTr("Controller correct"))//"遥控器校准"
                addTab(qsTr("Mission params"))//"换垄设置"
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
        visible: tabFunc.currentIndex == 0
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
        anchors {top: rctHeader.bottom; bottom: rowBtn.top;bottomMargin: 5; left: parent.left; right: parent.right}
        visible: tabFunc.currentIndex == 1
        Row {
            id:     rowQXSdk
            anchors {horizontalCenter: parent.horizontalCenter; top:parent.top; topMargin: 5}
            spacing: 10;
            Text{
                id:     txtIm
                text:   parameters.getName("RTCM_SOURCE")
            }
            VGParamEdit{
                id:imLst
            }
            Component.onCompleted:imLst.setParamID("RTCM_SOURCE")
        }
        Rectangle {
            id: rctNtrip
            color: "transparent"
            anchors {top: rowQXSdk.bottom; topMargin: 5; bottom: parent.bottom; left: parent.left; right: parent.right}
            visible: !plantManager.qxsdk
            Row{
                id: rowAcc
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
        Rectangle{
            anchors {top: rowQXSdk.bottom; topMargin: 5; left: parent.left; right: parent.right}
            visible: !rctNtrip.visible
            Grid{
                spacing: 6
                columns: 2
                anchors{top:parent.top; horizontalCenter: parent.horizontalCenter}
                Text{
                    id:                 txtKey
                    text:               qsTr("APP KEY:")//qsTr("APP Key：")
                    font:               vgMainPage.font()
                    verticalAlignment:  Text.AlignVCenter
                }
                VGTextField {
                    id: editKey
                    bBorder:            true
                    text:               plantManager.currentPlant ? plantManager.currentPlant.appkey : ""
                }
                Text{
                    id:                 txtSecret
                    text:               qsTr("APP SECRET:")//qsTr("App ：")
                    font:               vgMainPage.font()
                    verticalAlignment:  Text.AlignVCenter
                }
                TextArea{
                    id: editSecret
                    width:              editKey.width
                    height:             editKey.height * 3
                    text:               plantManager.currentPlant ? plantManager.currentPlant.appsecret : ""
                    wrapMode:           TextEdit.Wrap
                    font:               vgMainPage.font()
                }
                Text{
                    id:                 txtID
                    text:               qsTr("Device ID:")//qsTr("App ：")
                    font:               vgMainPage.font()
                    verticalAlignment:  Text.AlignVCenter
                }
                VGTextField {
                    id: editID
                    bBorder:            true
                    text:               plantManager.currentPlant ? plantManager.currentPlant.devid : ""
                }
                Text{
                    text:               qsTr("Device Type:")//qsTr("设备ID：")
                    font:               vgMainPage.font()
                    verticalAlignment:  Text.AlignVCenter
                }
                VGComboBox{
                    id:                         cbbDevType
                    width:						150
                    font:                       vgMainPage.font()
                    model:  [qsTr("Protection"), qsTr("Surveying"), qsTr("Power Inspection"),qsTr("AGV")]//植保, 测绘, 电力巡检,地面无人车
                    currentIndex:    plantManager.currentPlant ? plantManager.currentPlant.devtype : -1//plantManager.currentPlant.devtype
                }
            }
        }
        Row{
            anchors{horizontalCenter: parent.horizontalCenter; bottom: parent.bottom}
            spacing: 10
            Text{
                text:               qsTr("QX stat:")//qsTr("千寻运行状态：")
                font:               vgMainPage.font()
                verticalAlignment:  Text.AlignVCenter
            }
            Text {
                id: lbStat
                height:             parent.height
                font:               vgMainPage.font()
                text:               plantManager.currentPlant ? plantManager.currentPlant.qxwzStat : ""
            }
        }
    }
    Rectangle {
        id:     rctCntlr
        anchors {top: rctHeader.bottom; bottom: rowBtn.top; left: parent.left; right: parent.right}
        visible: tabFunc.currentIndex == 2
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
    Rectangle {
        id:     rctMsP
        anchors {top: rctHeader.bottom; bottom: rowBtn.top; left: parent.left; right: parent.right}
        visible: tabFunc.currentIndex == 3
        VGCheckBox {
            id: cbUTrance
            anchors {top: parent.top; topMargin: 20; left: rowTime.left}
            text:    qsTr("U-trance ridge")///"U-型换垄"
            checked: plantManager.uTrance
            onCheckedChanged: plantManager.uTrance=checked
        }
        Row{
            id: rowTime
            spacing: 10
            anchors {top: cbUTrance.bottom; topMargin: 10; horizontalCenter: parent.horizontalCenter}
            Text{
                text:               qsTr("Hold time:")//qsTr("停留时间：")
                font:               vgMainPage.font()
                height:             parent.height
                verticalAlignment:  Text.AlignVCenter
            }
            VGTextField {
                id:                 editTm
                bBorder:            true
                text:               plantManager.tmRoit.toFixed(2)
                validator:          DoubleValidator{decimals:2; bottom: 0; top:5}
                inputMethodHints:   Qt.ImhPreferNumbers
            }
        }
        VGTextButton {
            anchors {top: rowTime.bottom; topMargin: 15; right: rowTime.right}
            strText:      qsTr("Setting")//qsTr("设置")
            onBtnClicked: {
                plantManager.tmRoit = editTm.text
            }
        }
    }
    Row{
        id:         rowBtn
        anchors    {bottom: parent.bottom; bottomMargin: 15; right: parent.right; rightMargin: 20}
        spacing:    12

        VGTextButton {
            strText:        qsTr("Get")//qsTr("获取")
            visible:        rctQX.visible
            onBtnClicked:   rctNtrip.visible ? plantManager.getQxAccount():plantManager.getQxSdkAccount()
        }
        VGTextButton {
            strText:        qsTr("Clear")//qsTr("清除")
            visible:        rctQX.visible
            onBtnClicked:   rctNtrip.visible ? plantManager.clearQxAccount():plantManager.clearQxSdkAccount()
        }
        VGTextButton {
            strText:        qsTr("Setting")//qsTr("设置")
            visible:        rctQX.visible
            onBtnClicked:   rctNtrip.visible ? plantManager.setQxAccount(editAcc.text, editPswd.text):
                                            plantManager.setQxSdkAccount(editKey.text, editSecret.text, editID.text, cbbDevType.currentIndex)
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
