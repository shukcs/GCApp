import QtQuick          2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs  1.2

import VGGroundControl                       1.0

Rectangle {
    id:     root
    color:  "transparent"
    property alias fontTitle:       txtHeader.font
    property var   tcpCmd:          null
    implicitHeight: txtHeader.height + gridContent.height+21

    Component.onCompleted: {
        tcpCmd = linkManager.createLinkCmd(LinkCommand.TypeTcp)
    }

    Text {
        id: txtHeader
        anchors{top:parent.top; topMargin: height/2; left: parent.left; leftMargin: 25}
        text:   qsTr("TCP link:")//qsTr("TCP连接:")
    }
    Grid {
        id:             gridContent
        anchors {top:txtHeader.bottom; topMargin: 5; left: txtHeader.left; leftMargin: 6}
        columns:        2
        columnSpacing:  20
        rowSpacing:     4
        Label {
            text:   qsTr("Server IP:")//qsTr("IP地址:")
            verticalAlignment: Text.AlignVCenter
        }
        VGTextField {
            id:         ipInput
            bBorder:    true
            text:       tcpCmd.host
        }
        Label {
            text:   qsTr("Server port:")//qsTr("接收端口:")
            verticalAlignment: Text.AlignVCenter
        }
        VGTextField {
            id:         serverPortInput
            bBorder:    true
            text:   tcpCmd.port
        }
    }
    VGCheckBox {
        id:     cbAutoLink
        anchors {top: gridContent.top; left: gridContent.right; leftMargin: 16}
        text:           qsTr("Auto link")//qsTr("自动连接")
        checked:        linkManager.openTcp

        onClicked: {
            linkManager.openTcp = cbAutoLink.checked
        }
    }
    VGTextButton{
        id:                 btnConnect
        anchors {bottom: gridContent.bottom; right: cbAutoLink.right; leftMargin: 16}
        strText:            qsTr("Link")//qsTr("连接")
        onBtnClicked: {
            tcpCmd.setServer(ipInput.text, parseInt(serverPortInput.text))
            linkManager.endCreateLinkCmd(tcpCmd)
        }
    }
}
