import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtLocation       5.5
import QtPositioning    5.5

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
        height:         txtTitle.contentHeight + 10
        z:              200
        VGImage {
            anchors {left: parent.left; leftMargin:5; verticalCenter:parent.verticalCenter}
            width: 30
            height: width
            iconName: "backb"
            onImgClicked: vgMainPage.onSigBack()
        }
        Text{
            id:                         txtTitle
            font:                       vgMainPage.biggerFont(true)
            text:                       qsTr("search Uav")//qsTr("搜索植保机")
            anchors.horizontalCenter:   parent.horizontalCenter
            anchors.verticalCenter:     parent.verticalCenter
        }
    }
    Rectangle {
        id:seperater
        anchors{top: rctDeader.bottom; left: parent.left; right: parent.right}
        height: 1
        color:  "#D3D3D3"
    }
    Rectangle{
        id: rctInfo
        anchors{top: seperater.bottom; left: parent.left; right: parent.right; bottom: parent.bottom}
        color:  "#F6F6F6"
        height: rowEdit.height
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
