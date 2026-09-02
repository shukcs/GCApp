import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import VGGroundControl                      1.0

Rectangle
{
    id:     rootPage
    anchors.fill:   parent
    color:                          "#1f000000"
    MouseArea {
        anchors.fill:   parent
        onClicked:  {
            mouse.accepted = true
            vgMainPage.onSigBack()
        }
        onWheel:    {}
    }
    Rectangle {
        clip: true
        anchors.centerIn: parent
        width: parent.width*2/3
        height: parent.height/2
        color:  "white"
        radius: 6
        MouseArea{
            anchors.fill:   parent
            onClicked:      mouse.accepted = true
        }
        Text {
            id: txtHeader
            anchors{top:parent.top; topMargin: height/2; horizontalCenter: parent.horizontalCenter}
            text:   qsTr("Survey plant select")//"测绘飞机选择"
            font:   vgMainPage.font(true)
            color:  "#01113E"
        }
        Rectangle{
            id: sperater
            height: 1
            width:              parent.width
            anchors {top: txtHeader.bottom; topMargin: txtHeader.height/2}
            color: "#C8C8C8"
        }

        Flickable {
            anchors{ top:sperater.bottom; topMargin: 10; horizontalCenter:parent.horizontalCenter;
                bottom: btnClose.top; bottomMargin: 10 }
            clip:                   true
            width:                  contentWidth
            contentWidth:           rowPlant.width
            contentHeight:          rowPlant.height
            flickableDirection:     Flickable.VerticalFlick
            Column {
                id: rowPlant
                anchors    {top: sperater.bottom; topMargin: 10; horizontalCenter: parent.horizontalCenter}
                spacing:   3
                Repeater{
                    model: mapManager.getSpecItems(MapAbstractItem.Type_PlantInfo)
                    delegate: Rectangle{
                        height:     50
                        width:      54+name.contentWidth
                        visible:    object.monitor
                        color:      object===plantManager.surveyUav ? "#5F5FFF5F" : "#1F5F5FFF"
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
                            onClicked: plantManager.surveyUav = object
                        }
                    }
                }
            }
        }
        VGTextButton {
            id: btnClose
            anchors {bottom: parent.bottom; bottomMargin: 10; right: parent.right; rightMargin: 15}
            strText:      qsTr("Close")//qsTr("关闭")
            onBtnClicked: vgMainPage.onSigBack()
        }
    }
    Connections{
        target: vgMainPage
        onSigBack: {
            if (rootPage === page)
                vgMainPage.onSigBack()
        }
        onSigShowPage: {
            rootPage.visible = page === rootPage
            if (rootPage.visible)
                vgMainPage.titleVisible = false
            else if (linkCmd)
                linkManager.endCreateLinkCmd(linkCmd, false)
        }
    }
}
