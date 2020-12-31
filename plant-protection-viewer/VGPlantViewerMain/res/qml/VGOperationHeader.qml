import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.1

import VGGroundControl   1.0

Rectangle{
    id:     root
    color:  "#7FFFFFFF"
    height: rowPlantStat.height+6
    signal backClickd()
    VGImage {
        id: imgBack
        anchors {left: parent.left; leftMargin:5; verticalCenter:parent.verticalCenter}
        width: 30
        height: width
        iconName: "backb"
        onImgClicked:   root.backClickd()
    }
    Row{
        id:     rowPlantStat
        anchors {left: imgBack.right; leftMargin: 5; verticalCenter:parent.verticalCenter}
        spacing:    1
        VGParamView{
            id:paramMed
            width:      (root.width-imgBack.width-20)/13-1
            iconName:   (curPlant&&!curPlant.landed)?"model2":"model"
            strText:    plantManager.connected?curPlant.flightMode:"N/A"
        }
        VGParamView{
            width:      paramMed.width
            iconName:   plantManager.imGnd ? "height2":"height"
            strText:    plantManager.connected ? curPlant.heigth.toFixed(2):"N/A"
        }
        VGParamView{
            width:      paramMed.width
            iconName:   "speed"
            strText:    plantManager.connected ? curPlant.horSpeed.toFixed(2):"N/A"
        }
        VGParamView{
            width:      paramMed.width
            iconName:   "power"
            strText:    plantManager.connected ? curPlant.voltage.toFixed(1):"N/A"
        }
        VGParamView{
            width:      paramMed.width
            iconName:   "postip"
            strText:     curPlant ? curPlant.posTip :"N/A"
        }
        VGParamView{
            width:      paramMed.width
            iconName:   "satellite"
            strText:    plantManager.connected ? curPlant.satlateNumb : "N/A"
        }
        VGParamView{
            width:      paramMed.width
            id:          paramMedSpd
            iconName:   "velocity"
            strText:    plantManager.connected ? curPlant.medSpeed.toFixed(2):"N/A"
        }
        VGParamView{
            width:      paramMed.width
            iconName:   "medVol"
            strText:    plantManager.connected ? curPlant.medVol.toFixed(2):"N/A"
        }
        VGParamView{
            width:      paramMed.width
            iconName:   "gps"
            strText:    plantManager.connected ? curPlant.gpsTip:"N/A"
        }
        VGParamView{
            width:      paramMed.width
            iconName:   "rtk"
            strText:    plantManager.connected ? curPlant.accTip:"N/A"
        }
        VGParamView{
            width:      paramMed.width
            iconName:   "compass"
            strText:    plantManager.connected ? curPlant.compass.toFixed(1)+qsTr("degree"):"N/A"
        }
        VGParamView{
            width:      paramMed.width
            iconName:   "roll"
            strText:    plantManager.connected ? curPlant.roll.toFixed(1)+qsTr("degree"):"N/A"
        }
        VGParamView{
            width:      paramMed.width
            iconName:   "pitch"
            strText:    plantManager.connected ? curPlant.pitch.toFixed(1)+qsTr("degree"):"N/A"//"°"
        }
    }
}
