import QtQuick 2.7
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1

import VGGroundControl   1.0


Rectangle{
    id: colCov
    anchors.fill: parent
    Rectangle{
        id: rctNew
        anchors {top: parent.top; left: parent.left}
        width: parent.width
        height: 48
        color: "#BABABA"
        VGTextField {
            id:					editId
            anchors{verticalCenter: parent.verticalCenter; left: parent.left; leftMargin: 5}
            colBg:          "transparent"
            bBorder:        true
            borderColor:    "#F6F6F6"
            placeholderText:	qsTr("please input friend id")//qsTr("请输入好友ID")
            font:           vgMainPage.font()
        }
        VGImage {
            id: addImg
            anchors {right: parent.right; rightMargin:15; verticalCenter: parent.verticalCenter}
            width:  32
            height: width
            iconName: "add"
            onImgClicked: friendsManager.requestFriend(editId.text)
        }
    }
    Flickable{
        id:covFlickable
        anchors {top: rctNew.bottom ; left: parent.left;right: parent.right; bottom: parent.bottom}
        width: parent.width
        clip:                   true
        flickableDirection:     Flickable.VerticalFlick
        Column{
            anchors {fill: parent; topMargin: 2}
            spacing: 2
            Repeater {
                model:  friendsManager.newFriendMsgs
                delegate: Rectangle {
                    width: parent.width
                    height: 48
                    color: "#CACACA"
                    Label {
                        id: lbUser
                        anchors {left: parent.left; leftMargin: 15; bottom: parent.verticalCenter; bottomMargin: 2}
                        text: object.from
                        font:vgMainPage.font(true)
                    }
                    Label {
                        anchors {left: lbUser.left; top: lbUser.bottom; topMargin: 6}
                        text: object.text
                        font: vgMainPage.font()
                    }
                    Label{
                        anchors {right: parent.right; rightMargin:30; verticalCenter: parent.verticalCenter}
                        text:object.process === VGGSMessage.Agree ? qsTr("Agreed"):qsTr("Rejected")
                        visible: object.type===VGGSMessage.Msg_RequestFriend && object.process!==VGGSMessage.UnProcess
                        font:vgMainPage.font()
                    }
                    RowLayout {
                        visible: object.type===VGGSMessage.Msg_RequestFriend && object.process===VGGSMessage.UnProcess
                        anchors {right: parent.right; rightMargin:30; verticalCenter: parent.verticalCenter}
                        VGTextButton {
                            strText: qsTr("Agree")
                            onBtnClicked:object.process = VGGSMessage.Agree
                        }
                        VGTextButton {
                            strText: qsTr("Reject")
                            onBtnClicked:object.process = VGGSMessage.Reject
                        }
                    }
                }
            }
        }
    }
}
