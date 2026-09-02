import QtQuick 2.7
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtLocation       5.5
import QtPositioning    5.5
import QtQuick.Layouts 1.11

import VGGroundControl   1.0

Rectangle {
    id:             root
    Component{
        id: bounaryShareCom
        LandManager {
            anchors.fill: parent
            bCanEdit: false
            onBoundarySelected: {
                if (friendsManager.curFriend)
                    friendsManager.curFriend.sendLand(bdr)
            }
        }
    }

    Column{
        id:           colContent
        anchors.fill: parent
        RowLayout{
            anchors.fill: parent
            Rectangle{
                id:     recContants
                anchors {top: parent.top; left: parent.left; bottom: parent.bottom}
                width:  parent.width/5 < 120 ? 120: parent.width/5
                height: parent.height
                color: "#BABABA"
                Flickable {
                    anchors.fill: parent
                    clip:                   true
                    contentHeight:          contantCol.height
                    flickableDirection:     Flickable.VerticalFlick
                    Column{
                        id:contantCol
                        spacing: 1
                        Rectangle {
                            property bool selected: false
                            id:     sysMsgs
                            width:  recContants.width
                            height: 48
                            color:  selected ?"#CDE6FF":"#F6F6F6"
                            border {width: 1; color: selected ? "#b7C2FF":"transparent"}
                            Text {
                                text: qsTr("System messages") //系统消息
                                font:  vgMainPage.font()
                                anchors{left:parent.left;right: parent.right; verticalCenter: parent.verticalCenter;leftMargin:10}
                            }
                            MouseArea {
                                anchors.fill:   parent
                                onClicked: {
                                    friendsManager.curFriend = null
                                    sysMsgs.selected = true
                                }
                            }
                        }
                        Repeater {
                            id: recipient
                            model: mapManager.getSpecItems(MapAbstractItem.Type_Friend)
                            delegate: Rectangle {
                                width:recContants.width
                                height: 48
                                color: object.itemColor
                                border {width: 1; color: object.selected ? "#b7C2FF":"transparent"}
                                Text {
                                    text: object.name
                                    font:  vgMainPage.font()
                                    anchors{left:parent.left;right: parent.right; verticalCenter: parent.verticalCenter; leftMargin:10}
                                }
                                MouseArea {
                                    id:             mousearea
                                    anchors.fill:   parent
                                    onClicked: {
                                        sysMsgs.selected = false
                                        friendsManager.curFriend = object
                                    }
                                    onPressAndHold: {
                                        friendsManager.deleteFriend(object.name)
                                    }
                                }
                            }
                        }
                    }
                }
            }
            StackLayout {
                id: layout
                currentIndex: friendsManager.curFriend?1:0
                anchors {top: parent.top; left: recContants.right; bottom: parent.bottom}
                VGNewFriendPage{
                    id: recNewFriendPage
                    implicitWidth:  parent.width
                    implicitHeight: parent.height
                }
                VGConversationPage{
                    id: recConversation
                    implicitWidth:  parent.width
                    implicitHeight: parent.height
                    onShareBoundary: {
                        vgMainPage.curQmlPage = bounaryShareCom.createObject(root.parent)
                    }
                }
            }
        }
    }
}
