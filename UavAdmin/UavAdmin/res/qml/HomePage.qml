import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtLocation       5.5
import QtPositioning    5.5
import VGUavAdmin   1.0


Rectangle{
    id:             homeRoot
    anchors.fill:   parent

    VGMap{
        id:             mapPage
        anchors.fill:   parent
        bShowScale:     true
        MapItemView {
            model:    mapManager.getSpecItems(MapAbstractItem.Type_PlantInfo)
            delegate: MapQuickItem{
                anchorPoint.x:  plantImg.width/2
                anchorPoint.y:  plantImg.height/2
                coordinate :    object.coordinate
                z:              111                          //显示层次，越大越上
                rotation:       object.compass-45
                sourceItem:		VGImage {
                    id:         plantImg
                    height:     30
                    width:      height
                    iconName:   "plane"
                    bMosEnable: false
                    bImgNormal: object.status !== VGPlantInformation.Unlogin
                }
            }
        }
        Rectangle {
            property real left: value
            anchors {top: parent.top; left: parent.left; bottom: parent.bottom}
            border {color: "#0b81ff"; width: 1}
            visible: anchors.right > parent.left
            width:   150
            Behavior on width   {
                NumberAnimation{duration: 200}
            }
            Flickable {
                anchors{left:parent.left; right: parent.right; top:parent.top; bottom: parent.bottom}
                clip:                   true
                contentWidth: width
                contentHeight:          rowPlant.height
                flickableDirection:     Flickable.VerticalFlick
                Column {
                    id: rowPlant
                    anchors {top: headerRct.bottom; topMargin: 15; left: parent.left; leftMargin: 10}
                    spacing:                5
                    z:                      200
                    Repeater{
                        model: mapManager.getSpecItems(MapAbstractItem.Type_PlantInfo)
                        delegate: Rectangle{
                            height:     50
                            width:      50+name.width+2
                            visible:    object.monitor
                            color:      "#1F5F5FFF"
                            border  {width: 1; color:object===curPlant?"#0b81ff":"transparent"}
                            VGImage{
                                id:imgPlant
                                anchors {left: parent.left;leftMargin: 5; verticalCenter: parent.verticalCenter}
                                height: 40
                                width:	height
                                iconName: "craft"
                                bImgNormal: object.status===VGPlantInformation.Connected
                                bMosEnable: false
                            }
                            VGImage{
                                visible: object.binded
                                anchors {left: parent.left;leftMargin: 2; top: parent.top; topMargin: 2}
                                height: 16
                                width: height
                                iconName: "bind"
                                bMosEnable: false
                            }
                            Text{
                                id:     name
                                anchors {left: imgPlant.right;leftMargin: 5; top:parent.verticalCenter; topMargin: 1}
                                font:  vgMainPage.littleFont()
                                color: "gray"
                                elide: Text.ElideLeft
                                text:   object.planeId.length>0?object.planeId:qsTr("00000000")
                                width: 70
                            }
                            Text{
                                id:     mastor
                                anchors {left: name.left; bottom:parent.verticalCenter; bottomMargin: 1}
                                font:  vgMainPage.littleFont(true)
                                color: "black"
                                elide: Text.ElideLeft
                                text:   object.master.length>0?object.master:qsTr("Unnamed")//"未命名"
                                width: 70
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    plantManager.currentPlant = object
                                    if (mapManager.mgrObj&VGMapManager.Mission)
                                        missionCtrl.enterVMission()
                                    if (object.status===VGPlantInformation.Connected && object.coordinate.isValid)
                                        mapRect.center = object.coordinate
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Connections{
        target: vgMainPage
        onSigShowPage: {
            homeRoot.visible = vgMainPage.isVisible(homeRoot)
            if (homeRoot.visible) {
                vgMainPage.titleVisible = true
                vgMainPage.headerName = qsTr("Vigua app")//qsTr("农飞宝")
                mapPage.center = mapManager.mapCenter
            }
        }
        onSigBack:{
            if (page === homeRoot)
                vgMainPage.onSigBack()
        }
    }
}
