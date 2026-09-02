import QtQuick 2.0
import QtQuick.Layouts 1.11
import QtQuick.Controls 2.4
import QtQuick.Controls 1.4

Rectangle{
    Rectangle{
        id:title
        anchors{left:parent.left;right: parent.right; top:parent.top;}
        height: 35
        Row{
            anchors.fill: parent
            Text {
                id:date
                text: qsTr("Date")
                horizontalAlignment:Text.AlignHCenter
               width: (parent.width - detail.width)/6 * 1.5
            }
            Text {
                id:uavID
                text: qsTr("Uav ID")
                horizontalAlignment:Text.AlignHCenter
                width: (parent.width - detail.width)/6 * 1.5
            }
            Text {
                id:planID
                text: qsTr("Plan ID")
                horizontalAlignment:Text.AlignHCenter
                width: (parent.width - detail.width)/6
            }
            Text {
                id:mode
                text: qsTr("Flight Mode")
                horizontalAlignment:Text.AlignHCenter
                width: (parent.width - detail.width)/6
            }
            Text {
                id:acreage
                text: qsTr("Acreage")
                horizontalAlignment:Text.AlignHCenter
                width: (parent.width - detail.width)/6
            }
            Text {
                id:detail
                text: qsTr("Detail")
                horizontalAlignment:Text.AlignHCenter
                width: 40
            }
        }
    }
    Flickable {
        anchors{left:parent.left;right: parent.right; top:title.bottom;bottom: parent.bottom}
        clip:                   true
        contentWidth: width
        contentHeight:          detailCol.height
        flickableDirection:     Flickable.VerticalFlick
        Column{
            id:detailCol
            width: title.width
            spacing: 1
            Repeater {
                model: missionHistory.histories
                delegate: Rectangle {
                    width: title.width
                    height: 40
                    Row{
                        anchors.fill: parent
                        Rectangle{
                            width: date.width
                            height: parent.height
                            Text {
                                anchors{bottom: time.top;top:parent.top}
                                text:object.date
                                width: date.width
                                height: parent.height/2
                                horizontalAlignment:Text.AlignHCenter
                            }
                            Text{
                                id:time
                                anchors{bottom: parent.bottom}
                                width: date.width
                                text: object.time
                                height: parent.height/2
                                horizontalAlignment:Text.AlignHCenter
                            }

                        }
                        Text {
                            text: object.uav
                            horizontalAlignment:Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            width: uavID.width
                            height: parent.height
                        }
                        Text {
                            text: object.planId
                            horizontalAlignment:Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            width: planID.width
                            height: parent.height
                        }
                        Text {
                            text: object.mode
                            horizontalAlignment:Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            width: mode.width
                            height: parent.height
                        }
                        Text {
                            text: object.acreage.toFixed(2)
                            horizontalAlignment:Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            width: acreage.width
                            height: parent.height
                        }
                        Rectangle{
                            width: detail.width
                            height: parent.height
                            VGImage {
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.verticalCenter: parent.verticalCenter
                                iconName:"detail"
                                width: 30
                                height: 35
                            }
                        }
                    }

                }
            }
        }
    }
}




























//    //header数据
//    function getObjectsToArray(model) {
//        var array = []
//        for (var i = 0; i < model.count; ++i) {
//            array.push(model.get(i))
//        }
//        return array
//    }
//    Component {
//        id: headerDelegate
//        Rectangle {
//            width: 300
//            height: 30
//            color:  "#2d2d2d"
//            border.width: 1
//            border.color: "#838383"
//            Text {
//                id: headerTextInput
//                anchors.centerIn: parent
//                text: styleData.value
//                color: "#e5e5e5"

//            }
//        }
//    }

//    Component {
//        id: itemDelegate
//        //Loader 动态加载不同的组件
//        Loader {
//            id: itemLoader
//            anchors.fill: parent
//            visible: status === Loader.Ready
//            //根据role加载相应的组件
//            sourceComponent: {
//                var role = styleData.role;
//                if (role === "detail")
//                    return detailComponent;
////                if (role === "finishTime")
////                    return finishTimeComponent;
//                else return defaultComponent;
//            }

//            //Note: 各种component需要写在loader内部。因为要访问styleData，在外部会
//            //提示找不到styleData
//            Component {
//                id: defaultComponent
//                Item {
//                    anchors.fill: parent
//                    Text {
//                        anchors.fill: parent
//                        text: styleData.role === "acreage"? styleData.value.toFixed(2):styleData.value
//                        horizontalAlignment:Text.AlignHCenter
//                    }
//                }
//            }
////            Component {
////                id: acreageComponent
////                Item {
////                    anchors.fill: parent
////                    Text {
////                        anchors.fill: parent
////                        text:styleData.value.toFixed(2)
////                        horizontalAlignment:Text.AlignHCenter
////                    }
////                }
////            }
//            Component {
//                id: detailComponent
//                Rectangle{
//                    anchors.fill: parent
//                    VGImage{
//                        anchors.horizontalCenter: parent.horizontalCenter
//                        width:                  parent.height-10
//                        height:                 width
//                        iconName:               "detail"
//                        //onImgClicked:			onDetail(object)
//                    }
//                }
//            }
//        }
//    }

//    TableView{
//        id:tableView
//        anchors{left:parent.left;right: parent.right; top:parent.top;bottom: parent.bottom}

//        TableViewColumn {
//            role: "finishTime"
//            title: qsTr("Date")
//            resizable: true
//            horizontalAlignment:Text.AlignHCenter
//        }

//        TableViewColumn {
//            role: "uav"
//            title: qsTr("Uav ID")
//            resizable: true
//            horizontalAlignment:Text.AlignHCenter
//        }
//        TableViewColumn {
//            role: "planId"
//            title: qsTr("Plan ID")
//            horizontalAlignment:Text.AlignHCenter
//        }
//        TableViewColumn {
//            role: "mode"
//            title: qsTr("Flight Mode")
//            horizontalAlignment:Text.AlignHCenter
//        }
//        TableViewColumn {
//            role: "acreage"
//            title: qsTr("Acreage")
//            resizable: true
//            horizontalAlignment:Text.AlignHCenter
//        }
//        TableViewColumn {
//            role: "detail"
//            title: qsTr("Detail")
//            resizable: true
//            horizontalAlignment:Text.AlignHCenter
//        }
//        model:getObjectsToArray(missionHistory.histories)
//        itemDelegate: itemDelegate
//        headerDelegate: headerDelegate
//    }

