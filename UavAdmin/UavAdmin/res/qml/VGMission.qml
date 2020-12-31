﻿import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1

import VGGroundControl   1.0

Rectangle {
    id:         root
    radius:                 6
    border {width: 1; color: "#EEEEEE"}
    clip:           true
    property bool bEditValue:     false
    property var  curPlant:       plantManager.currentPlant
    signal exitMission()
    signal addSupport(bool bEnter)
    signal showCenter(bool bVis)
    signal btnMissionClicked()
    signal startMission()
    function setOperationRoute(rt) {
        if (rt)
            rctEdit.vehicleMission = rt.beginOperation()
    }
    function enterVMission() {
        if (curPlant) {
            rctEdit.vehicleMission = curPlant.vm
            if (rctEdit.vehicleMission)
                rctEdit.vehicleMission.selected = true
        }
    }
    function setSupportPoint(coor, bEnter) {
        if (rctEdit.vehicleMission)
            rctEdit.vehicleMission.addSupport(coor, bEnter)
    }
    function qmlBack() {
        if (!bEditValue)
            emit:exitMission()
        else if(vgMainPage.isCurQmlChild(sliderValue))
            sliderValue.qmlBack()
        else {
            vgMainPage.onSigBack()
            bEditValue = false
            root.showCenter(false)
        }
    }
    function checkSupport(parent) {
        if (rctEdit.vehicleMission === null || !curPlant)
            return
        if (!plantManager.oneKeyArm && !curPlant.missionValible)
            return

        if(!rctEdit.vehicleMission.hasEnterSup || !rctEdit.vehicleMission.hasReturnSup) {
            vgMainPage.curQmlPage = supportTipCom.createObject(parent)
            return
        }
        if(plantManager.oneKeyArm) {
            rctEdit.vehicleMission.sync2Vehicle()
            return
        }
        emit:startMission()
    }
    function toSetSupport() {
        if (rctEdit.vehicleMission) {
            var pageSE = supportEditCom.createObject(root)
            vgMainPage.curQmlPage = pageSE
            root.showCenter(true)
            bEditValue = true
        }
    }
    Component{
        id: supportTipCom
        VGTip {
            strTitle:   qsTr("Tip of support")//辅助点提示
            strTip:     qsTr("No support set;Do set them right now?")//起飞与返航辅助点未设置完成；是否马上设置？
            onFinished: {
                if (accept)
                    toSetSupport()
            }
        }
    }
    VGSliderRect {
        id:             sliderValue
        anchors.fill:   parent
        visible:        bEditValue
        onFinishValue: {
            bEditValue = !exit
            if (!rctEdit.vehicleMission)
                return
            if (strKey === valHeight.strKey)
                rctEdit.vehicleMission.opHeight = val
            else if (strKey === valSpeed.strKey)
                rctEdit.vehicleMission.speed = val
            else if (strKey === valMedc.strKey)
                rctEdit.vehicleMission.medPerAcre = val
        }
    }
    Component {
        id: areaEditCom
        VGAreaEditRect {
            anchors.fill: parent
            strHeader: qsTr("Spray of route")//设置喷洒航线
            onFinishValue:{
                if (rctEdit.vehicleMission && bInit) {
                    bEditValue = !exit
                    rctEdit.vehicleMission.begin = valLit
                    rctEdit.vehicleMission.end = valBig
                    setOtherText(rctEdit.vehicleMission.length.toFixed(2)+"km")
                }
            }
        }
    }
    Component {
        id: supportEditCom
        VGSupportEditRect {
            anchors.fill: parent
            vehicleMission: rctEdit.vehicleMission
            onAddSupport: root.addSupport(bEnter)
            onFinishEdit: {
                bEditValue = false
                root.showCenter(false)
            }
        }
    }
    VGImage {
        id: backImg
        anchors {left: parent.left; leftMargin: 10; verticalCenter: txtHeader.verticalCenter}
        visible: !bEditValue
        width:      30
        height:     width
        iconName:   "backb"
        onImgClicked: {emit:exitMission()}
    }
    Text {
        id: txtHeader
        anchors{horizontalCenter: parent.horizontalCenter; top:parent.top; topMargin: height/2}
        visible: !bEditValue
        text: qsTr("Sortie setting")//设置架次参数
        color: "#0b81ff"
        font:   vgMainPage.font(true)
    }
    Rectangle{
        id:                     seperator
        anchors{top: txtHeader.bottom; topMargin: txtHeader.height/2; left: parent.left; right: parent.right}
        visible:                !bEditValue
        height:                 1
        color:                  "#D3D3D3"
    }
    Rectangle {
        id:rctEdit
        property var vehicleMission: null
        anchors{top: seperator.bottom; left: parent.left; right: parent.right;bottom: parent.bottom}
        visible:        !bEditValue
        color:          "#F6F6F6"
        VGValueItem {
            id:    valMissionArea
            property int begLine: rctEdit.vehicleMission?rctEdit.vehicleMission.begin:0
            property int endLine: rctEdit.vehicleMission?rctEdit.vehicleMission.end:0
            anchors{top: parent.top; topMargin: 5; left: parent.left}
            width:      parent.width*3/5
            enabled:    rctEdit.vehicleMission
            strKey:     qsTr("Spray")//喷洒航线
            strValue:   begLine+"-"+endLine
            onClickedBtn: {
                if (rctEdit.vehicleMission) {
                    var pageAE = areaEditCom.createObject(root)
                    pageAE.initial(rctEdit.vehicleMission.count, begLine, endLine, rctEdit.vehicleMission.length.toFixed(2)+"km")
                    vgMainPage.curQmlPage = pageAE
                    bEditValue = true
                }
            }
        }
        VGValueItem {
            id:    valSupport
            anchors{top: parent.top; topMargin: 5; left: valMissionArea.right; leftMargin: 4;right: parent.right}
            enabled: rctEdit.vehicleMission
            strKey:qsTr("Supports")//辅助点
            onClickedBtn: toSetSupport()
            Row{
                anchors{verticalCenter: parent.verticalCenter; right: parent.right; rightMargin: 15}
                spacing: 8
                Rectangle {
                    border {width: 1; color: "green"}
                    radius: 3
                    height: txtStar.height+4
                    width: txtStar.width+8
                    visible: rctEdit.vehicleMission && rctEdit.vehicleMission.hasEnterSup
                    Text{
                        id:txtStar
                        anchors.centerIn: parent
                        text:  qsTr("Start")//起
                        color: "green"
                        font:  vgMainPage.littleFont()
                    }
                }
                Rectangle {
                    border {width: 1; color: "red"}
                    radius: 3
                    height: txtEnd.height+4
                    width: txtEnd.width+8
                    visible: rctEdit.vehicleMission && rctEdit.vehicleMission.hasReturnSup
                    Text{
                        id:txtEnd
                        anchors.centerIn: parent
                        text:  qsTr("Return")//返
                        color: "red"
                        font:  vgMainPage.littleFont()
                    }
                }
            }
        }
        VGValueItem {
            id:     valHeight
            anchors {top: valSupport.bottom; topMargin: 5; left: parent.left; right: parent.horizontalCenter; rightMargin: 2}
            enabled:    rctEdit.vehicleMission
            strKey:     qsTr("Height")//高度
            strUnit:    qsTr("m")
            strValue:   rctEdit.vehicleMission ? rctEdit.vehicleMission.opHeight.toFixed(1) : 3
            onClickedBtn: {
                bEditValue = true
                sliderValue.strHeader = key
                sliderValue.setMaxValue(30)
                sliderValue.setMinValue(1)
                sliderValue.setStep(.1)
                sliderValue.strUnit = strUnit
                sliderValue.setValue(value)
            }
        }
        VGValueItem {
            id:         valSpeed
            anchors     {top: valHeight.top; left: valHeight.right; leftMargin: 4; right: parent.right}
            enabled:    rctEdit.vehicleMission
            strKey:     qsTr("Speed")//速度
            strUnit:    qsTr("m/s")
            strValue:   rctEdit.vehicleMission ? rctEdit.vehicleMission.speed.toFixed(1):1
            onClickedBtn: {
                bEditValue = true
                sliderValue.strHeader = key
                sliderValue.setMaxValue(7)
                sliderValue.setMinValue(1)
                sliderValue.setStep(.1)
                sliderValue.strUnit = strUnit
                sliderValue.setValue(value)
            }
        }
        VGValueItem {
            id:     valMedc
            anchors {top: valHeight.bottom;  topMargin: 5; left: parent.left; right: valHeight.right}
            enabled:    rctEdit.vehicleMission
            strKey:     qsTr("Volumn of acre")//亩用量
            strUnit:    qsTr("L/A") //L/亩
            strValue:   rctEdit.vehicleMission ? rctEdit.vehicleMission.medPerAcre.toFixed(1):1
            onClickedBtn: {
                bEditValue = true
                sliderValue.strHeader = key
                sliderValue.setMaxValue(5)
                sliderValue.setMinValue(.5)
                sliderValue.setStep(.1)
                sliderValue.strUnit = strUnit
                sliderValue.setValue(value)
            }
        }
        VGOnKeyStart {
            id: slider
            enabled: curPlant && curPlant.status===VGPlantInformation.Connected
            visible: plantManager.oneKeyArm
            anchors{left: parent.left; right: parent.right; bottom: parent.bottom
                leftMargin: 40; rightMargin: 40; bottomMargin: 8}
            strBgText:  qsTr("Move slider for execute")//滑动滑块执行
            fontBgTxt:  vgMainPage.font(true)
            onAccept:   root.btnMissionClicked()
        }
        Row{
            anchors{ bottom: parent.bottom; bottomMargin: 8; horizontalCenter: parent.horizontalCenter}
            visible: !plantManager.oneKeyArm
            spacing: 15
            VGTextButton {
                enabled: curPlant && curPlant.status===VGPlantInformation.Connected
                strText:qsTr("Send route")//发送航线
                onBtnClicked: {
                    if (rctEdit.vehicleMission!==null)
                        rctEdit.vehicleMission.sync2Vehicle()
                }
            }
            VGTextButton {
                enabled: curPlant && curPlant.missionValible
                strText:qsTr("Operation start")//启动作业
                onBtnClicked: root.btnMissionClicked()
            }
        }
    }
}
