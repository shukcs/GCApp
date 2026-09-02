import QtQuick 2.7
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1

import VGGroundControl   1.0

Rectangle{
    signal shareBoundary()
    anchors.fill:   parent
    color:          "#A8A8FF"
    Column{
        id: colCov
        anchors {top: parent.top ; left: parent.left;right: parent.right; bottom: parent.bottom}
        Flickable{
            id:covFlickable
            anchors {top: parent.top ; left: parent.left;right: parent.right; bottom: sendRec.top}
            width: parent.width - recContants.width
            contentHeight:colCovItem.height
            clip:                   true
            flickableDirection:     Flickable.VerticalFlick
            anchors.margins: 5
            Column{
                id:colCovItem
                spacing: 6
                width: parent.width
                Repeater {
                    id: covRepeater
                    model:friendsManager.curFriend.msgs
                    delegate: Rectangle {
                        anchors.right: object.recv ? undefined : parent.right
                        width: object.type === VGGSMessage.Msg_Text ? txtMsg.contentWidth + 12 : landShow.width
                        height: object.type === VGGSMessage.Msg_Text ? txtMsg.contentHeight + 12 : landShow.height
                        color: object.recv ? "lightgrey" : "steelblue"
                        Label {
                            id: txtMsg
                            text: object.text
                            font:vgMainPage.font()
                            color: "black"
                            anchors {top:parent.top; left:parent.left; margins: 6}
                            wrapMode: Label.Wrap
                            visible: object.type === VGGSMessage.Msg_Text
                        }
                        Rectangle{
                            id:landShow
                            anchors.fill: parent
                            visible: object.type === VGGSMessage.Msg_Land
                            color: "white"
                            width: (landName.contentWidth>lbSize.contentWidth?landName.contentWidth:lbSize.contentWidth)+68
                            height: 60
                            Label{
                                id:landName
                                text: object.land?object.land.address:qsTr("unkown")
                                font: vgMainPage.biggerFont(true)
                                color: "black"
                                anchors{top:parent.top;left:parent.left; topMargin: 6; leftMargin: 6}
                            }
                            Label{
                                id: lbSize
                                text: qsTr("size:") + (object.land?object.land.landArea.toFixed(2):qsTr("unkown"))
                                font: vgMainPage.font()
                                color: "black"
                                anchors{top: landName.bottom; left:landName.left; topMargin: 6}
                            }
                            VGImage{
                                width:      48
                                height:     width
                                iconName:   "sendLand"
                                bMosEnable: false
                                anchors {right: parent.right; verticalCenter: parent.verticalCenter; rightMargin: 6}
                            }
                        }
                    }
                }
            }
        }
        Rectangle {
            id: sendRec
            anchors{top:colCovItem.bottom;left:parent.left;right:parent.right;bottom: parent.bottom}
            height: 150
            color: "#F8F8FF"
            Column{
                anchors.fill: parent
                VGImage{
                    width:      36
                    height:     width
                    iconName:   "sendLand"

                    anchors {left: parent.left; leftMargin: 10}
                    onImgClicked: {emit: shareBoundary()}
                }
                RowLayout {
                    width: parent.width
                    TextArea {
                        id: messageField
                        Layout.fillWidth: true
                        placeholderText: qsTr("Compose message")
                        color: "black"
                        font :vgMainPage.font();
                        wrapMode: TextArea.Wrap
                    }
                    VGTextButton {
                        id: sendButton
                        strText: qsTr("Send")
                        enabled: messageField.length > 0 && friendsManager.curFriend
                        onBtnClicked: {
                            if (friendsManager.curFriend) {
                                friendsManager.curFriend.sendText(messageField.text)
                                messageField.text = ""
                            }
                        }
                    }
                }
            }
        }
    }
}

