import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts  1.0
import QtQuick.Dialogs  1.2

import VGGroundControl   1.0

ApplicationWindow {
    id: root
    width:      500
    height:     700
    property var	pageHome:			null
    property var	pageLoggin:			null
    property var	vclSchPage:			null
    property var    guiSettingsPage:    null
    property bool bSplashScreen:true
    Component {
        id:         splashCom
        VGSplashScreen {
            onTimeout: {
                root.showMaximized(bSplashScreen)
                visible = false
                bSplashScreen = false
                vgMainPage.destroyQmlObject(vgMainPage.curQmlPage)
                vgMainPage.curQmlPage = pageHome
            }
        }
    }
    Component{
        id:     homePageComponent
        HomePage{
            visible: false
            anchors.fill: contentRec
        }
    }
    Component{
        id:     logginComponent
        LogginPage{
            visible: false
            anchors.fill: contentRec
        }
    }
    Component {
        id: guiSettingsPageCom
        GUISettings { anchors.fill: parent }
    }
    Component {
        id: vclSchCom
        VehicleSearch{}
    }
    Component{
    	id:     tipComponent
        VGTip {
            z:          120
            strTitle:   qsTr("Warming")//qsTr("警告")
            strTip:     vgMainPage.messgeTip
            bCancel:false
        }
    }
    Component.onCompleted: {
        visible = true
        vgMainPage.addQmlObject(root)
        pageLoggin = logginComponent.createObject(contentRec)
        pageHome = homePageComponent.createObject(contentRec)
        vgMainPage.addQmlObject(pageLoggin, root)
        vgMainPage.addQmlObject(pageHome, root)
        vgMainPage.curQmlPage = splashCom.createObject(contentRec)
        showFullScreen()
    }

    function logginQml() {
        if (!pageLoggin) {
            pageLoggin = logginComponent.createObject(contentRec)
            vgMainPage.addQmlObject(pageLoggin, root)
        }
        vgMainPage.curQmlPage = pageLoggin
    }
    function vehicleQml() {
        if (!vclSchPage) {
            vclSchPage = vclSchCom.createObject(contentRec)
            vgMainPage.addQmlObject(vclSchPage, root)
        }
        vgMainPage.curQmlPage = vclSchPage
    }
    header: Rectangle {
        id:         topRect
        width:      parent.width
        height:     titles.contentHeight*2
        color:      vgMainPage.bgColor
        visible:    vgMainPage.titleVisible && !bSplashScreen
        Row {
			spacing:    6
            anchors {verticalCenter: parent.verticalCenter; left: parent.left; leftMargin: 10}
            VGToolButton {
                id:                     logginImg
                icSize: 24
                colNormal:   "transparent"
                bICNormal:   netManager.connectState===1
                iconName:    "user"
                onBtnClicked: {
                    if (!pageLoggin) {
                        pageLoggin = logginComponent.createObject(contentRec)
                        vgMainPage.addQmlObject(pageLoggin, root)
                    }
                    vgMainPage.curQmlPage = pageLoggin
                }
            }
            VGToolButton {
                id:                     planeImg
                icSize:                 24
                colNormal:   "transparent"
                bICNormal:   plantManager.connected
                iconName:    "plantG"
                onBtnClicked: {
                    if (netManager.connectState===1)
                        vehicleQml()
                    else
                        logginQml()
                }
            }
        }
        Label {
            id: titles
            text: vgMainPage.headerName
            textFormat: Text.AutoText
            color: "white"
            font:                       vgMainPage.biggerFont(true)
            anchors.horizontalCenter:   parent.horizontalCenter
            anchors.verticalCenter:     parent.verticalCenter
        }	
		VGImage {
			id: settingsImg
			anchors {right: parent.right; rightMargin:15; verticalCenter: parent.verticalCenter }
			width: 36
			height: width
			iconName:   "settings"
			onImgClicked: {
				if (!guiSettingsPage) {
					guiSettingsPage = guiSettingsPageCom.createObject(root)
					vgMainPage.addQmlObject(guiSettingsPage, root)
				}
				vgMainPage.curQmlPage = guiSettingsPage
			}
		}
    }
    Rectangle{
        id:             contentRec
        anchors {top: header.visible ? header.bottom:parent.top; left: parent.left; right:  parent.right; bottom: parent.bottom}
        color:          vgMainPage.backColor
    }
    Rectangle{
        id:     tipRct
        anchors {centerIn: parent; verticalCenterOffset: parent.height/3}
        height:     tipText.height+20
        width:      img.width+tipText.width+height+6
        visible:    vgMainPage.tipVisible && !bSplashScreen
        radius:     height/2
        color: "#DFFFFFFF"
        VGImage {
            id: img
            anchors {verticalCenter: tipRct.verticalCenter; left: tipRct.left; leftMargin: tipRct.height/2}
            height:     20
            width:      height
            iconName:   vgMainPage.tipWarn?"tip_error":"tip_success"
            bMosEnable: false
        }
        Text {
            id: tipText
            anchors {verticalCenter: tipRct.verticalCenter; right: tipRct.right; rightMargin: tipRct.height/2}
            text: vgMainPage.qmlTip
            textFormat: Text.AutoText
            font: vgMainPage.biggerFont()
            color: vgMainPage.tipWarn?"#C81E06":"#0b81ff"
        }
    }
	Connections{
        target: vgMainPage
        onMessgeTipChanged: {
            var curpage = vgMainPage.curQmlPage;
            vgMainPage.curQmlPage = tipComponent.createObject(curpage);
        }
        onScrHorizChanged : {
            if (scrHor)
                showFullScreen()
            else
                root.showMaximized(bSplashScreen)
        }
    }
}
