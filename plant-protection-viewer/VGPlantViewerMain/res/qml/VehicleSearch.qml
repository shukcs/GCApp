import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import VGGroundControl   1.0

Page {
    id:             root
    anchors.fill:   parent

    signal  vehicleSelected(var vk)
    function searchVehicle() {
        rctInfo.strSearch = qsTr("VIGAU:")+editId.text
        plantManager.addNewPlant(rctInfo.strSearch)
    }
    Rectangle {
        id:             rctDeader
        anchors{top: parent.top; left: parent.left; right: parent.right}
        height:         tabFunc.height + 10
        z:              200
        VGImage {
            id: imgBack
            anchors {left: parent.left; leftMargin:5; verticalCenter:parent.verticalCenter}
            width: 30
            height: width
            iconName: "backb"
            onImgClicked: vgMainPage.onSigBack()
        }
        VGTabHeader {
            id:                         tabFunc
            anchors {left: imgBack.right; leftMargin: 5; verticalCenter:  parent.verticalCenter}
            txtFont:                    vgMainPage.font(true)
            Component.onCompleted: {
                addTab(qsTr("search Uav"))//搜索植保机
                addTab(qsTr("Query mission"))//查询作业"
                currentIndex = 0
            }
        }
    }
    Rectangle{
        id: rctInfo
        anchors{top: rctDeader.bottom; left: parent.left; right: parent.right; bottom: parent.bottom}
        color:  "#F6F6F6"
        height: rowEdit.height
        visible: tabFunc.currentIndex === 0
        property string strSearch: ""
        Text {
            id: txtID
            anchors{left: parent.left; leftMargin: 8; verticalCenter: btnSerch.verticalCenter}
            text: qsTr("VIGAU:")
        }
        VGTextField {
            id:					editId
            anchors{left: txtID.right; leftMargin: 6; verticalCenter: btnSerch.verticalCenter; right: btnSerch.left; rightMargin: 6}
            colBg:				"white"
            bBorder:			true
            placeholderText:	qsTr("please input Uav ID of last 8")//qsTr("请输入韦加植保机ID后8位")
            font.capitalization: Font.AllUppercase
        }
        VGTextButton {
            id: btnSerch
            strText:        qsTr("Search")//qsTr("搜索")
            anchors         {top:parent.top; topMargin: 10; right: parent.right; rightMargin: 8}
            enabled:        editId.text.length===8
            onBtnClicked:   searchVehicle()
        }
        Column {
            id:     colV
            width:  parent.width
            anchors{top: editId.bottom; left: parent.left; right: parent.right; margins: 6}
            spacing: 6
            Repeater {
                model: mapManager.getSpecItems(MapAbstractItem.Type_PlantInfo)
                delegate: Rectangle{
                    height: 50
                    width:  parent.width
                    border  {width: object.planeId===rctInfo.strSearch?1:0; color: "#30308a"}
                    color:  object.planeId===rctInfo.strSearch ? "#A9BAFA" : "white"
                    VGImage{
                        id:imgPlant
                        anchors {left: parent.left;leftMargin: 5; verticalCenter: parent.verticalCenter}
                        height: 40
                        width: height
                        sourceSize {height: imgPlant.height; width: imgPlant.width}
                        iconName: "craft"
                        bImgNormal: object.status===VGPlantInformation.Connected
                        bMosEnable: false
                    }
                    Text{
                        id:     name
                        anchors {left: imgPlant.right;leftMargin: 5; top:parent.verticalCenter; topMargin: 3}
                        font:  vgMainPage.littleFont()
                        color: "gray"
                        text:   object.planeId.length>0?object.planeId:qsTr("Unnamed")//qsTr("未命名")
                    }
                    Text{
                        id:     mastor
                        anchors {left: name.left; bottom:parent.verticalCenter; bottomMargin: 3}
                        font:  vgMainPage.littleFont(true)
                        color: "black"
                        text:   object.master
                    }                 
                    VGToolButton{
                        anchors {right: parent.right; rightMargin: 8; verticalCenter: parent.verticalCenter}
                        icSize:   24
                        iconName: !object.monitor?"add":"minus"
                        onBtnClicked: {
                            if (object.monitor)
                                plantManager.remove(object)
                            else
                                plantManager.addNewPlant(name.text, true)
                        }
                    }
                }
            }
        }
    }
    Rectangle{
        id: rctMsQue
        anchors{top: rctDeader.bottom; left: parent.left; right: parent.right; bottom: parent.bottom}
        color:  "#F6F6F6"
        height: rowEdit.height
        visible: tabFunc.currentIndex === 1
        VGCalendarBox {
            id: calendar
            anchors.centerIn:   parent
            property bool bBeg: true
            visible: false
            z:1
            calendar.onPressed: {
                visible = false
                if (bBeg)
                    missionHistory.begTime = date.toLocaleDateString(Qt.locale("de_DE"), "yyyy-MM-dd")
                else
                    missionHistory.endTime = date.toLocaleDateString(Qt.locale("de_DE"), "yyyy-MM-dd")
            }
        }
        Text {
            id: txtBegin
            anchors{left: parent.left; leftMargin: 8; verticalCenter: editBeg.verticalCenter}
            text: qsTr("Time:")
        }
        VGTextField {
            id:					editBeg
            anchors{left: txtBegin.right; leftMargin: 6; top:parent.top; topMargin: 8}
            width:              120
            colBg:				"white"
            bBorder:			true
            readOnly:           true
            text:               missionHistory.begTime
            MouseArea {
                anchors.fill: parent
                onPressed: {
                    calendar.bBeg = true
                    calendar.visible = true
                }
            }
        }
        Text {
            id: txtEnd
            anchors{left: editBeg.right; leftMargin: 5; verticalCenter: editEnd.verticalCenter}
            text: qsTr("-")
        }
        VGTextField {
            id:					editEnd
            anchors{left: txtEnd.right; leftMargin: 5;top:parent.top; topMargin: 8}
            width:              editBeg.width
            colBg:				"white"
            bBorder:			true
            readOnly:           true
            text:               missionHistory.endTime
            MouseArea {
                anchors.fill: parent
                onPressed: {
                    calendar.bBeg = false
                    calendar.visible = true
                }
            }
        }
        Text {
            id: txtUavId
            anchors{left: parent.left; leftMargin: 8; verticalCenter: cbbUav.verticalCenter}
            text: qsTr("Uav ID:")
        }
        VGComboBox {
            id:                     cbbUav
            anchors {left: txtUavId.right;  leftMargin: 8;top:editBeg.bottom; topMargin: 6}
            width:                  editBeg.width
            model:                  plantManager.mntPants
        }
        VGTextButton {
            anchors {left: cbbUav.right; leftMargin: 20; bottom: cbbUav.bottom; topMargin: 6}
            strText:        qsTr("Query mission acreage")
            onBtnClicked:   {
                missionHistory.uav = cbbUav.currentText
                missionHistory.reqMissionArea()
            }
        }       
        Rectangle {
            anchors {left: parent.left; leftMargin: 12; top: cbbUav.bottom; topMargin: 8; bottom: parent.bottom; bottomMargin: 8}
            width: parent.width-24
            color: "transparent"
            Text {
                id: txtAcreageTip
                anchors{left: parent.left; top: parent.top}
                color: "#0b81ff"
                text: missionHistory.msTip
                font.bold: true
            }
            VGTextButton {
                anchors {left: txtAcreageTip.right; leftMargin: 20; top: parent.top;topMargin: 6}
                strText:        qsTr("Detail")
                height:         txtAcreageTip.height
                enabled:        missionHistory.valid
                onBtnClicked:   {
                    missionHistory.reqDetail()
                }
            }
            VGMissionHistoryDetail{
                anchors {left: parent.left; leftMargin: 12; top: txtAcreageTip.bottom; topMargin: 8;
                    bottom: parent.bottom; bottomMargin: 8;right:parent.right;rightMargin: 12}
            }
        }
    }
    Connections {
        target: vgMainPage
        onSigBack:{
            if (page === root)
                vgMainPage.onSigBack()
        }
        onSigShowPage: {
            var vis = vgMainPage.isVisible(root)
            root.visible = vis
            if (vis)
                vgMainPage.titleVisible = false
            else
                plantManager.clearTmpPlant()
        }
    }
}
