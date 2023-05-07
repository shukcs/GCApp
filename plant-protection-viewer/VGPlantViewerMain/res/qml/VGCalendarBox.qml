import QtQuick 2.0

import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4


Rectangle{
    width: 300
    height: 300
    border.width: 1
    border.color: "#0b81ff"
    color: "black"
    property alias calendar: the_calendar
    property string colorB: "#0b81ff"
    //这里没有把字体颜色之类的引出来，作为控件的时候需要定义这些

    Calendar{
        id:the_calendar
        anchors.fill: parent
        anchors.margins: 2

        style: CalendarStyle {
            //gridColor: "white"
            //gridVisible: false

            background: Rectangle{
                id:background
                anchors.fill: parent
                //color:"white"
            }

            //标题年月
            navigationBar:Item{
                //color: "transparent"
                height: the_calendar.height/8
                Canvas {
                    id: prevYear
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    width: parent.height/2
                    height: width
                    //color: bgColor
                    onPaint: {
                        var ctx = getContext("2d")
                        ctx.lineWidth = 1
                        ctx.strokeStyle = colorB
                        ctx.beginPath()
                        ctx.moveTo(0,height*3/4)
                        ctx.lineTo(width/2,height/4)
                        ctx.lineTo(width,height*3/4)
                        //ctx.closePath()
                        ctx.stroke()
                    }

                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            control.showPreviousYear()
                        }
                    }
                }
                Canvas {
                    id: nextYear
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: prevYear.right
                    anchors.leftMargin: 10
                    width: parent.height/2
                    height: width
                    //color: bgColor
                    onPaint: {
                        var ctx = getContext("2d")
                        ctx.lineWidth = 1
                        ctx.strokeStyle = colorB
                        ctx.beginPath()
                        ctx.moveTo(0,height/4)
                        ctx.lineTo(width/2,height*3/4)
                        ctx.lineTo(width,height/4)
                        //ctx.closePath()
                        ctx.stroke()
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            control.showNextYear()
                        }
                    }
                }
                Label {
                    id: labelYear
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: nextYear.right
                    anchors.leftMargin: 15
                    //text: the_calendar.selectedDate.getFullYear()+qsTr('年')
                    text:control.visibleYear+qsTr('年')
                    //elide: Text.ElideRight
                    horizontalAlignment: Text.AlignLeft
                    font.pixelSize: 14
                    font.family: "SimSun"
                    color: "black"
                }

                Canvas {
                    id: nextMonth
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    width: parent.height/2
                    height: width
                    //color: bgColor
                    onPaint: {
                        var ctx = getContext("2d")
                        ctx.lineWidth = 1
                        ctx.strokeStyle = colorB
                        ctx.beginPath()
                        ctx.moveTo(0,height/4)
                        ctx.lineTo(width/2,height*3/4)
                        ctx.lineTo(width,height/4)
                        //ctx.closePath()
                        ctx.stroke()
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            control.showNextMonth()
                        }
                    }
                }
                Canvas {
                    id: prevMonth
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: nextMonth.left
                    anchors.rightMargin: 10
                    width: parent.height/2
                    height: width
                    //color: bgColor
                    onPaint: {
                        var ctx = getContext("2d")
                        ctx.lineWidth = 1
                        ctx.strokeStyle = colorB
                        ctx.beginPath()
                        ctx.moveTo(0,height*3/4)
                        ctx.lineTo(width/2,height/4)
                        ctx.lineTo(width,height*3/4)
                        //ctx.closePath()
                        ctx.stroke()
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            control.showPreviousMonth()
                        }
                    }
                }
                Label {
                    id: labelMonth
                    anchors {verticalCenter: parent.verticalCenter; right: prevMonth.left; rightMargin: 15}
                    ///注意Date原本的月份是0开始
                    text: (control.visibleMonth+1)+qsTr('月')
                    ///elide: Text.ElideRight
                    horizontalAlignment: Text.AlignRight
                    font.pixelSize: 14
                    font.family: "SimSun"
                    color: "black"
                }

                Rectangle{
                    width: parent.width
                    height: 1
                    anchors.bottom: parent.bottom
                    color: "black"
                }
            }
            //星期
            dayOfWeekDelegate: Item{
                //color: "transparent"
                height: the_calendar.height/8
                Label {
                    text: control.__locale.dayName(styleData.dayOfWeek, control.dayOfWeekFormat)
                    anchors.centerIn: parent
                    color: "black"
                    font.pixelSize: 12
                    font.family: "SimSun"
                }
            }
            dayDelegate: Rectangle {
                //选中-当月未选中-其他
                color: styleData.selected
                       ?colorB
                       : (styleData.visibleMonth && styleData.valid
                          ?"white"
                          : Qt.rgba(255,255,255,1));

                Label {
                    text: styleData.date.getDate()
                    anchors.centerIn: parent
                    font.pixelSize: 12
                    font.family: "SimSun"
                    color: styleData.valid
                           ?"black"
                           : "black"
                }
            }
        }
    }
}

