import QtQuick 2.7
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.4
//import QtQuick.Layouts 1.1
import QtQuick.Layouts 1.11


import VGGroundControl   1.0

RowLayout{
    id:rowLayout
    property string inConversationWith:friendsManager.recipient
    anchors.fill: parent
    Rectangle{
        id:recContants
        implicitWidth:rowLayout.width/5 < 150? 150:rowLayout.width/5
        color: "#BABABA"
        anchors {top: parent.top ; left: parent.left; bottom: parent.bottom;}
        anchors.topMargin: 48
        Flickable {
            clip:                   true
            width:recContants.width
            contentHeight:          contantCol.height
            flickableDirection:     Flickable.VerticalFlick
            anchors.fill: parent
            Column{
                id:contantCol
                spacing: 1
                Rectangle {
                    property bool selected: false
                    id:recNewFriend
                    width:recContants.width
                    height: 48
                    color: selected ?"#CDE6FF":"#F6F6F6"
                    border.width:   1
                    border.color:   selected ? "#b7C2FF":"transparent"
                    Text {
                        text: qsTr("新的朋友")
                        font:  vgMainPage.font()
                        anchors{left:parent.left;right: parent.right}
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.leftMargin:10
                    }
                    MouseArea {
                        anchors.fill:   parent
                        onClicked: {
                            recNewFriend.selected = true;
                            friendsManager.setItemSelected(false);
                        }
                    }
                }
                Repeater {
                    id: recipient
                    model: friendsManager.vgFriends
                    delegate: Rectangle {
                        width:recContants.width
                        height: 48
                        color: object.itemColor
                        border.width:   1
                        border.color:   object.selected ? "#b7C2FF":"transparent"
                        Text {
                            text: object.recipient
                            font:  vgMainPage.font()
                            anchors{left:parent.left;right: parent.right}
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.leftMargin:10
                        }
                        MouseArea {
                            id:             mousearea
                            anchors.fill:   parent
                            onClicked: {
                                object.selected = true
                                recNewFriend.selected =false
                            }
                            onDoubleClicked:{
                                friendsManager.recipient = object.recipient
                            }
                        }
                    }
                }
            }
        }
    }
    StackLayout{
        id: layout
        anchors {top: parent.top ; left: recContants.right;right: parent.right; bottom: parent.bottom}
        implicitWidth: parent.width * 0.8
        currentIndex:0
        VGNewFriendPage{
        }
    }
}

