import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1

import VGGroundControl   1.0

VGValueShow {
    id:                             root
    anchors.fill:       parent

    property var    boundaryInfo:       null
    property bool   editInfo:           true
    property bool   bCanEdit:           true

    signal adjustBoudary(var bdr)

    onBoundaryInfoChanged: {
        if (!boundaryInfo)
            return
        addValue(qsTr("Owner"), boundaryInfo.owner, editInfo)//qsTr("主人")
        addValue(qsTr("Phone No"), boundaryInfo.ownerPhone, editInfo)//qsTr("联系方式")
        addValue(qsTr("Area"), boundaryInfo.landArea.toFixed(2), editInfo)//qsTr("亩数")
        addValue(qsTr("Block"), boundaryInfo.countBlocks.toString(), editInfo)//qsTr("障碍物")
        addValue(qsTr("Survey source"), boundaryInfo.surveyDescribe, editInfo)//"边界点来源"
        addValue(qsTr("Surveyor"), boundaryInfo.user, editInfo)//"创建人"
        addValue(qsTr("Create time"), boundaryInfo.editTime)//创建时间
        addValue(qsTr("Notes"), boundaryInfo.describe, editInfo)//备注

        strHeader = boundaryInfo.address
        if (bCanEdit) {
			strHeader += "("+boundaryInfo.boundaryId+")"
            addButton(qsTr("Boundary adjust"), "#0b81ff")//边界点调整
            addButton(qsTr("Delete"), "#E64331")//删除
        }
    }
    onClickedBtn:{
        if (!boundaryInfo)
            return
        if (str === qsTr("Boundary adjust"))
            emit:adjustBoudary(boundaryInfo)
        else if (str === qsTr("Delete"))
            boundaryInfo.releaseSafe()
        vgMainPage.destroyQmlObject(root)
    }
    Connections{
        target: vgMainPage
        onSigShowPage: root.visible = root === page
    }
}
