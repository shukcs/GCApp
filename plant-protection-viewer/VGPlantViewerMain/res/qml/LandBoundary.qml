import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtLocation       5.5
import QtPositioning    5.5

import VGGroundControl   1.0

/*
  * 子页面: 保存测绘地块；
  *
*/

Page {
    id:                                   root
    anchors.fill:                         parent

    property var    landInfo:             null
    property var    landBoundary:         null
    property bool   isInitial:            false
    property bool   bAdjustBoundary:      false
    property alias  pointSelected:        bdrAdjust.selectPoint
    property alias  mapCenter:            mapPage.center
    property bool   bEdit:                false

    function initialBoundary() {
        if (!isInitial) {
            tabFunc.addTab(qsTr("Informatica"))//qsTr("信息")
            if (bAdjustBoundary) {
                tabFunc.addTab(qsTr("Boundary adjust"))//qsTr("边界点调整")
                tabFunc.addTab(qsTr("Modify boundary"))//qsTr("修改边界")
            }
            else {
                tabFunc.addTab(qsTr("Add point of boundary"))//qsTr("添加边界"
            }
            tabFunc.currentIndex = 1

            if (landBoundary){
                headerRct.visible = true
                if (landBoundary.surveyType !== MapAbstractItem.Survey_DrawMap) {
                    mapPage.addBtn("blueTooth", "")
                    if (landBoundary.surveyType===MapAbstractItem.Survey_Vehicle)
                        mapPage.addBtn("deviceOpt", "")

                    if (landBoundary.surveyType===MapAbstractItem.Survey_Plant) {
                        mapPage.addBtn("Uav_select", "")
                        mapPage.addBtn("plant pos", "plant")
                    }
                    else {
                        mapPage.addBtn("survey", "survey")
                        setSurvey(landBoundary.surveyType)
                    }
                }
                else {
                    headerRct.visible = false
                }
                if (landBoundary.surveyType === MapAbstractItem.Survey_DrawMap)
                    mapPage.addBtn("clear", "clear")
            }
        }
        isInitial = true
        mapManager.mgrObj = VGMapManager.Land
    }
    function setSurvey(tp) {
        if (tp === MapAbstractItem.Survey_DrawMap)
            return
        switch(tp){
        case MapAbstractItem.Survey_BanLv:
            vgMainPage.curQmlPage = bltGPSCom.createObject(root); break
        case MapAbstractItem.Survey_Vehicle:
        case MapAbstractItem.Survey_Plant:
            vgMainPage.curQmlPage = bltSettingsCom.createObject(root); break
        }
    }
    function backQml(){
        var page = backTip.createObject(root)
        if (page)
            vgMainPage.curQmlPage = page
    }
    function headerChange(index, str){
        txtSave.text = index > 0 ? qsTr("Save") : qsTr("Next")//qsTr("保存") : qsTr("下一步")
        flickable.contentX = index * flickable.width
        bEdit = (str===qsTr("Modify boundary") || str===qsTr("Add point of boundary"))
        if (bEdit)
            mapPage.bDrawCenter = (landBoundary && landBoundary.surveyType===MapAbstractItem.Survey_DrawMap)
        else
            mapPage.bDrawCenter = false
    }
    function addBoundary(btn){
        if (!landBoundary || bAdjustBoundary)
            return
        if (0===btn){
            landBoundary.clearBoudary()
        }
        else if (1===btn) {
            if (landBoundary.surveyType === MapAbstractItem.Survey_DrawMap)
                landBoundary.addBoudaryPoint(mapCenter)
            else if(landBoundary.surveyType===MapAbstractItem.Survey_Plant && headerRct.bConnect)
                landBoundary.addBoudaryPoint(plantManager.surveyUav.coordinate)
            else if (surveyMonitor.posValid)
                landBoundary.addBoudaryPoint(surveyMonitor.coordinate)
        }
        else if (2===btn){
            if (landBoundary.surveyType === MapAbstractItem.Survey_DrawMap)
                landBoundary.addBlockPoint(mapCenter)
            else if(landBoundary.surveyType===MapAbstractItem.Survey_Plant && headerRct.bConnect)
                landBoundary.addBlockPoint(plantManager.surveyUav.coordinate)
            else if (surveyMonitor.posValid)
                landBoundary.addBlockPoint(surveyMonitor.coordinate)
        }
    }
    function editBoundary(btn){
        if (!landBoundary || !bAdjustBoundary)
            return

        if (1===btn) {
            if (landBoundary.surveyType === MapAbstractItem.Survey_DrawMap)
                landBoundary.addBoudaryPoint(mapCenter, pointSelected)
            else if(landBoundary.surveyType===MapAbstractItem.Survey_Plant && headerRct.bConnect)
                landBoundary.addBoudaryPoint(plantManager.surveyUav.coordinate)
            else if (surveyMonitor.posValid)
                landBoundary.addBoudaryPoint(surveyMonitor.coordinate, pointSelected)
        }
        else if (2===btn){
            if (landBoundary.surveyType === MapAbstractItem.Survey_DrawMap)
                landBoundary.addBlockPoint(mapCenter, pointSelected)
            else if(landBoundary.surveyType===MapAbstractItem.Survey_Plant && headerRct.bConnect)
                landBoundary.addBlockPoint(plantManager.surveyUav.coordinate)
            else if (surveyMonitor.posValid)
                landBoundary.addBlockPoint(surveyMonitor.coordinate, pointSelected)
        }
    }
    Component {
        id: pageOpt
        DeviceOption {}
    }
    Component {
        id: comUavSelect
        SurveyUavSelect {}
    }
    Column{
        anchors.fill: parent
        VGMap{
            id:                 mapPage
            clip:               true
            width:              parent.width
            height:             parent.height-rctInfo.height
            bEnableEdit:        true
            fBtnTop:            headerRct.visible?headerRct.height:0
            //边界距离
            MapItemView {
                model:      mapManager.getSpecItems(MapAbstractItem.Type_EdgeTip)
                delegate:   MapQuickItem {
                    anchorPoint.x: normalDis.width/2
                    anchorPoint.y: normalDis.height/2
                    coordinate: object.coor
                    z:          120
                    sourceItem: Rectangle {
                        id:         normalDis
                        width:      txtDis.widthProp
                        height:     width
                        color:     "transparent"
                        Text{
                            id: txtDis
                            property int widthProp: (contentHeight>contentWidth?contentHeight:contentWidth)+4
                            anchors.horizontalCenter:   parent.horizontalCenter
                            anchors.verticalCenter:     parent.verticalCenter
                            text:                       object.dis.toFixed(2)
                            font:                       vgMainPage.littleFont()
                            horizontalAlignment:        Text.AlignHCenter
                            verticalAlignment:          Text.AlignVCenter
                            color:                      "white"
                        }
                    }
                }
            }
            MapQuickItem {
                visible:        landBoundary && landBoundary.landArea>0
                coordinate :    landBoundary ? landBoundary.center : QtPositioning.coordinate()
                anchorPoint.x:  rctArea.width/2
                anchorPoint.y:  rctArea.height/2
                sourceItem: Rectangle {
                    id:         rctArea
                    width:      txtArea.contentWidth
                    height:     txtArea.contentHeight
                    color:      "transparent"
                    Text{
                        id: txtArea
                        anchors.horizontalCenter:   parent.horizontalCenter
                        anchors.verticalCenter:     parent.verticalCenter
                        text:                       landBoundary?(landBoundary.landArea.toFixed(2)+qsTr("Acre)")):""//qsTr("亩"))
                        font:                       vgMainPage.littleFont()
                        horizontalAlignment:        Text.AlignHCenter
                        verticalAlignment:          Text.AlignVCenter
                        color:                      "white"
                    }
                }
            }
            MapQuickItem {
                anchorPoint.x:  imgSv.width/2
                anchorPoint.y:  imgSv.height/2
                visible:        landBoundary && landBoundary.surveyType===MapAbstractItem.Survey_BanLv
                coordinate:     surveyMonitor.coordinate
                z:              111
                rotation:       surveyMonitor.compass + 22
                sourceItem: VGImage {
                    id:     imgSv
                    height: 30
                    width:  height
                    iconName: "survey"
                    bMosEnable: false
                    bImgNormal: surveyMonitor.connected
                }
            }
            MapQuickItem{
                anchorPoint.x:  plantImg.width/2
                anchorPoint.y:  plantImg.height/2
                visible:        plantManager.surveyUav && landBoundary && landBoundary.surveyType===MapAbstractItem.Survey_Plant
                coordinate :    visible?plantManager.surveyUav.coordinate : QtPositioning.coordinate()
                z:              111                          //显示层次，越大越上
                rotation:       visible ? plantManager.surveyUav.compass-45 : -45
                sourceItem:		VGImage {
                    id:         plantImg
                    height:     30
                    width:      height
                    iconName:   "plane"
                    bMosEnable: false
                    bImgNormal: plantManager.surveyUav.status === VGPlantInformation.Connected
                }
            }
            onDbCilickedCoordinate: {
                if (bEdit) {
                    if(coor === pointSelected)
                        pointSelected = null
                    coor.releaseSafe()
                }
            }
            onCilickedCoordinate: {
                if (bAdjustBoundary) {
                    editBdr.clickCoor(coor)
                    pointSelected = coor.selected ? coor : null
                }
            }
            onClickBtn:  {
                if (!landBoundary)
                    return
                if (txt === "blueTooth")
                    setSurvey(landBoundary.surveyType)
                else if (txt === "survey")
                    mapCenter = surveyMonitor.coordinate
                else if (txt === "plant pos" && headerRct.bConnect)
                    mapCenter = plantManager.surveyUav.coordinate
                else if (txt === "clear" && !bAdjustBoundary)
                    landBoundary.clearSurvey()
                else if (txt === "deviceOpt")
                    vgMainPage.curQmlPage = pageOpt.createObject(root)
                else if (txt === "Uav_select")
                    vgMainPage.curQmlPage = comUavSelect.createObject(root)
            }
        }
        Rectangle{
            id:     rctInfo
            height: parent.height*2/5
            width:  parent.width
            clip:   true
            color:  "#F6F6F6"
            VGImage {
                id: backImg
                anchors {left: parent.left; leftMargin: 4; verticalCenter: tabFunc.verticalCenter}
                width:        30
                height:       width
                iconName:     "backb"
                onImgClicked: backQml()
            }
            VGTabHeader {
                id:			tabFunc
                txtFont:	vgMainPage.font(true)
                anchors		{top: parent.top; left: backImg.right; leftMargin: 2}
                onCurrentIndexChanged: headerChange(currentIndex, currentStr())
            }
            Text {
                id: txtSave
                anchors {right: parent.right; rightMargin: 15; verticalCenter: tabFunc.verticalCenter}
                enabled: tabFunc.currentIndex>0 && landBoundary && landBoundary.valid
                color:   enabled ? "black":"gray"
                property bool   bSave:                false
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        if (tabFunc.currentIndex > 0) {
                            if (landBoundary) {
                                if (bAdjustBoundary){
                                    vgMainPage.curQmlPage =  changeNameCom.createObject(root)
                                    return
                                }
                                landBoundary.save(false)
                                landBoundary.save(true)
                            }
                            txtSave.bSave = true
                            vgMainPage.onSigBack()
                        }
                        else{
                            tabFunc.currentIndex = 1
                        }
                    }
                }
            }
            Rectangle{
                id:                     seperator
                anchors {top: tabFunc.bottom; left: parent.left; right: parent.right }
                height:                 1
                color:                  "#D3D3D3"
            }
            Flickable{
                id:                 flickable
                anchors {top: seperator.bottom; left: parent.left; bottom: parent.bottom}
                width:              parent.width
                clip:               true
                flickableDirection: Flickable.HorizontalFlick
                contentHeight:      height
                contentWidth:       rowContent.width
                contentX:           tabFunc.currentIndex*width
                Row {
                    id:     rowContent
                    height: parent.height
                    VGBoundaryInfo{
                        height:             parent.height
                        width:              flickable.width
                        landBoundary:       root.landBoundary
                        landInfo:           root.landInfo
                        parentEdit:         root
                    }
                    VGBoundaryAdjust {
                        id:                 bdrAdjust
                        height:             parent.height
                        width:              flickable.width
                        landBoundary:       root.landBoundary
                        visible:            bAdjustBoundary
                    }
                    VGBoundaryAdd {
                        height:             parent.height
                        width:              flickable.width
                        landBoundary:       root.landBoundary
                        onClickedBtn:       addBoundary(btn)
                        visible:            !bAdjustBoundary
                    }
                    VGBoundaryEdit {
                        id:                 editBdr
                        height:             parent.height
                        width:              flickable.width
                        landBoundary:       root.landBoundary
                        onClickedBtn:       editBoundary(btn)
                        visible:            bAdjustBoundary
                    }
                }
                pressDelay:         100
                onMovementEnded:    {
                    var idx = contentX<width/2? 0 : 1
                    if (idx === tabFunc.currentIndex)
                        contentX = idx===0?0:width
                    else
                        tabFunc.currentIndex = idx
                }
            }
        }
    }
    VGImage {
        id:         imgBack
        anchors {top: parent.top; topMargin: 5; left: parent.left; leftMargin: 10}
        z:          200
        width:      32
        height:     width
        iconName:   "backBg"
        visible:    landBoundary&&landBoundary.surveyType===MapAbstractItem.Survey_DrawMap
        onImgClicked: backQml()
    }
    Rectangle{
        id:     headerRct
        color:  "#DFFFFFFF"
        width:  parent.width
        height: paramMed.height+6
        property bool bConnect:  plantManager.surveyUav && plantManager.surveyUav.status===VGPlantInformation.Connected
        z:      200
        visible: !imgBack.visible
        VGImage {
            id: imgBck
            anchors {left: parent.left; leftMargin:5; verticalCenter:parent.verticalCenter}
            width: 30
            height: width
            iconName:           "backb"
            onImgClicked:       backQml()
        }
        Row {
            id:     rowSurvey
            anchors {left: imgBck.right; leftMargin: 0; right: parent.right; rightMargin: 10; verticalCenter:parent.verticalCenter}
            spacing:    0
            visible: landBoundary && landBoundary.surveyType!==MapAbstractItem.Survey_Plant
            VGParamView{
                width: parent.width/5
                iconName:   "postip"
                strText:    surveyMonitor.posTip
            }
            VGParamView{
                width: parent.width/5
                iconName:   "satellite"
                strText:    surveyMonitor.connected?surveyMonitor.satNum:"N/A"
            }
            VGParamView{
                id:paramMed
                width: parent.width/5
                iconName:   "precision"
                strText:    surveyMonitor.connected?surveyMonitor.precision.toFixed(2):"N/A"
            }
            VGParamView{
                width: parent.width/5
                iconName:   "power"
                strText:    surveyMonitor.connected?surveyMonitor.powerPercent.toFixed(1):"N/A"
            }
            VGParamView{
                width: parent.width/5
                iconName:   "qxwz"
                strText:    qxManager.rtcm ? qsTr("QX opened"):qsTr("QX closed")//qsTr("千寻开启"):qsTr("千寻关闭")
            }
        }
        Row {
            anchors {left: imgBck.right; leftMargin: 0; right: parent.right; rightMargin: 10; verticalCenter:parent.verticalCenter}
            spacing:    0
            visible:    !rowSurvey.visible
            VGParamView{
                width:      paramMed.width
                iconName:   plantManager.imGnd ? "height2":"height"
                strText:    headerRct.bConnect ? plantManager.surveyUav.heigth.toFixed(2):"N/A"
            }
            VGParamView{
                width:      paramMed.width
                iconName:   "speed"
                strText:    headerRct.bConnect ? plantManager.surveyUav.horSpeed.toFixed(2):"N/A"
            }
            VGParamView{
                width:      paramMed.width
                iconName:   "power"
                strText:    headerRct.bConnect ? plantManager.surveyUav.voltage.toFixed(1):"N/A"
            }
            VGParamView{
                width:      paramMed.width
                iconName:   "postip"
                strText:     headerRct.bConnect ? plantManager.surveyUav.posTip :"N/A"
            }
            VGParamView{
                width:      paramMed.width
                iconName:   "satellite"
                strText:    headerRct.bConnect ? plantManager.surveyUav.satlateNumb : "N/A"
            }
        }
    }

    Component {
        id: backTip
        VGTip {
            strTitle:   qsTr("Tip")//qsTr("提示")
            strTip:     bAdjustBoundary? qsTr("Ensure cancle boundary adjust？"):qsTr("Ensure cancle create land？")//qsTr("确认取消边界点调整？"):qsTr("确认取消创建地块？")
            onFinished: {
                if (accept) {
                    if (landBoundary)
                        landBoundary.releaseSafe()
                    txtSave.bSave = false
                    vgMainPage.onSigBack()
                }
            }
        }
    }
    Component{
        id: changeNameCom
        VGValueChange {
            strTitle:   qsTr("Modify land name(different of orientate)")//qsTr("修改地块名称（不能与之前一致）")
            strKey:     qsTr("Name")//qsTr("名称")
            strValue:   landBoundary ? landBoundary.address : ""
            onFinished:{
                if (accept && landBoundary){
                    landBoundary.address = str
                    landBoundary.save(false)
                    landBoundary.save(true)
                    txtSave.bSave = true
                    vgMainPage.onSigBack()
                }
            }
        }
    }
    Component{
        id: bltSettingsCom
        BluetoothSettings {}
    }
    Component{
        id: bltGPSCom
        BluetoothGPS {}
    }
    Component{
        id:   boundarySave
        LandBoundarySave {
            editInfo: true
        }
    }
    Connections{
        target: vgMainPage
        onSigBack: {
            if (page === root)
                backQml()
        }
        onSigShowPage: {
            var b = vgMainPage.isVisible(root)
            root.visible = b
            if (!b) {
                if(pointSelected)
                    pointSelected.selected = false
                if (landInfo)
                    landInfo.showContent(false)
                if (landBoundary)
                    landBoundary.selected = false
                landBoundary.showBelonged = false

                if (!txtSave.bSave) {
                    if (landInfo)
                        landInfo.releaseSafe()
                    if (landBoundary)
                        landBoundary.releaseSafe()
                }
                mapManager.saveConfig()
            }

            if (root === page)
                initialBoundary()
        }
    }
}
