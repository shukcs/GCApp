import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1

import VGGroundControl   1.0

Rectangle{
    id:     root
    implicitHeight: fontMetrics.height*2
    implicitWidth:  rowTabs.width
    color:          "transparent"

    property font txtFont:      vgMainPage.font(true)
    property int currentIndex:  0
    signal currentChanged(int idx)

    function addTab(strTab) {
        tabNames.append({"txtTab":strTab, "tabIndex": tabNames.count})
    }
    function modifyTab(strTab, idx) {
        tabNames.setProperty(idx, "txtTab", strTab)
    }
    function currentStr() {
        return tabNames.get(currentIndex).txtTab
    }
    Row{
        id:             rowTabs
        height:         parent.height
        ListModel {
            id: tabNames
        }
        FontMetrics{
            id:      fontMetrics
            font:    txtFont
        }
        Repeater{
            model: tabNames
            delegate: Rectangle {
                id:                     rctBdInfo
                color:                  "transparent"
                clip:                   true
                anchors.leftMargin:     15
                height:                 parent.height
                width:                  fontMetrics.advanceWidth(txtTab)+height
                Label {
                    id: lbTab
                    anchors.horizontalCenter:   parent.horizontalCenter
                    anchors.verticalCenter:     parent.verticalCenter
                    text:               txtTab
                    font:               txtFont
                    color:              "#0b81ff"
                }
                Rectangle{
                    visible:                tabIndex === currentIndex
                    anchors.bottom:         parent.bottom
                    anchors.left:           parent.left
                    anchors.right:          parent.right
                    height:                 3
                    color:                  "#0b81ff"
                }
                MouseArea{
                    anchors.fill:   parent
                    onClicked:      {
                        root.currentIndex = tabIndex
                        root.currentChanged(tabIndex)
                    }
                }
            }
        }
    }
}

