import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1

import VGGroundControl   1.0

Rectangle {
    id:                             root
    color:                          "white"
    radius:                         6
    clip:                           true
    property alias  strHeader:      txtHeader.text
    property bool  bInit:           false
    signal finishValue(string strKey, double valLit, double valBig, bool exit)
    function initial(max, little, bigger, othStr) {
        bInit = false
        rctBg.from = 1
        rctBg.to = max
        rctBg.stepSize = 1
        rctBg.first.value = little
        rctBg.second.value = bigger
        txtOther.text = othStr
        bInit = true
    }

    Rectangle {
        id:         rctHeader
        anchors{top: parent.top;left: parent.left;right:parent.right}
        color: "transparent"
        height: txtHeader.contentHeight*2

        function qmlExit(){
            emit:finishValue(strHeader, txtValL.text, txtValB.text, true)
            vgMainPage.onSigBack()
        }
        VGImage {
            id: backImg
            anchors {left: parent.left; leftMargin: 10; verticalCenter: parent.verticalCenter}
            width:  30
            height: width
            iconName: "backb"
            onImgClicked: rctHeader.qmlExit()
        }
        Label{
            id:                         txtHeader
            anchors.centerIn:           parent
            font:                       vgMainPage.font(true)
            color:                      "#0b81ff"
        }
        Rectangle{
            id:                     seperator
            anchors{bottom: parent.bottom; left: parent.left;right:parent.right}
            height:                 1
            color:                  "#D3D3D3"
        }
    }
    Rectangle{
        id:rctContent
        anchors {top: rctHeader.bottom;bottom: parent.bottom;left: parent.left;right: parent.right}
        color: "transparent"
        function minusValLitter() {
            if (rctBg.first.value>rctBg.from) {
                rctBg.first.decrease()
                root.finishValue(strHeader, txtValL.text, txtValB.text, false)
            }
        }
        function plusValLitter() {
            if (rctBg.first.value<rctBg.second.value) {
                rctBg.first.increase()
                root.finishValue(strHeader, txtValL.text, txtValB.text, false)
            }
        }
        function minusValBigger() {
            if (rctBg.first.value<rctBg.second.value) {
                rctBg.second.decrease()
                root.finishValue(strHeader, txtValL.text, txtValB.text, false)
            }
        }
        function plusValBigger() {
            if (rctBg.second.value<rctBg.to) {
                rctBg.second.increase()
                root.finishValue(strHeader, txtValL.text, txtValB.text, false)
            }
        }
        Rectangle {
            id:rctLE
            color: "transparent"
            anchors {left: parent.left;leftMargin: 40;top:parent.top;right: parent.horizontalCenter;rightMargin: 30}
            height: 40
            Text{
                id:txtValL
                anchors.centerIn: parent
                color:  "#2C2C2C"
                font:   vgMainPage.font()
                text:   rctBg.first.value.toFixed(0)
            }
            VGToolButton  {
                id: minusImgL
                anchors {right: parent.horizontalCenter; rightMargin: 25; verticalCenter: parent.verticalCenter}
                iconName: "arrow_left"
                icSize: 24
                onBtnClicked: rctContent.minusValLitter()
            }
            VGToolButton  {
                id: addImgL
                anchors {left: parent.horizontalCenter; leftMargin: 25; verticalCenter: parent.verticalCenter}
                iconName: "arrow_right"
                icSize: 24
                MouseArea{
                    anchors.fill: parent
                    onClicked:  rctContent.plusValLitter()
                }
            }
        }
        Rectangle {
            color:  "#0b81ff"
            anchors {verticalCenter: rctLE.verticalCenter; horizontalCenter: parent.horizontalCenter}
        }
        Rectangle {
            color: "transparent"
            anchors {left: parent.horizontalCenter;leftMargin: 30;top:parent.top; right: parent.right; rightMargin: 40;}
            height: 40
            Text{
                id:txtValB
                anchors.centerIn: parent
                color:  "#2C2C2C"
                font:   vgMainPage.font()
                text:   rctBg.second.value.toFixed(0)
            }
            VGToolButton  {
                id: minusImgB
                anchors {right: parent.horizontalCenter; rightMargin: 25; verticalCenter: parent.verticalCenter}
                iconName: "arrow_left"
                icSize: 24
                onBtnClicked: rctContent.minusValBigger()
            }
            VGToolButton  {
                id: addImgB
                anchors {left: parent.horizontalCenter; leftMargin: 25; verticalCenter: parent.verticalCenter}
                iconName: "arrow_right"
                icSize: 24
                onBtnClicked: rctContent.plusValBigger()
            }
        }
        RangeSlider {
            id: rctBg
            anchors.centerIn: parent
            width:      parent.width-80
            from:       1
            to:         10
            stepSize:   1
            first.value: 1
            second.value: 10
            first.onValueChanged: root.finishValue(strHeader, first.value, second.value, false)
            second.onValueChanged:root.finishValue(strHeader, first.value, second.value, false)
        }
        Text{
            anchors {left:rctBg.left; bottom:txtOther.bottom}
            color:  "#2C2C2C"
            font:   vgMainPage.littleFont()
            text:   rctContent.fMinVal
        }
        Text{
            id:     txtOther
            anchors {bottom: rctBg.top; bottomMargin: 2; horizontalCenter: parent.horizontalCenter}
            color:  "#2C2C2C"
            font:   vgMainPage.littleFont()
        }
        Text{
            anchors {right:rctBg.right; bottom: txtOther.bottom}
            color:  "#2C2C2C"
            font:   vgMainPage.littleFont()
            text:   rctContent.fMaxVal
        }
    }
    Connections{
        target: vgMainPage
        onSigShowPage: root.visible = root === page
        onSigBack:{
            if (page === root)
                rctHeader.qmlExit()
        }
    }
}
