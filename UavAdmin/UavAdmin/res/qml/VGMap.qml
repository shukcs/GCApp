import QtQuick 2.2
import QtLocation       5.5
import QtPositioning    5.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import VGUavAdmin 1.0

/*
  * 封装的地图页面：地图显示页面，包括地图的放大、缩小，位置，地块轮廓组件显示等；
  *
*/

Map {
    id:        root
    center:    mapManager.mapCenter
    zoomLevel: mapManager.zoomLevel
    plugin: Plugin {name: "VGMapControl"}
    gesture.flickDeceleration: 3000
    gesture.enabled: true
    gesture.acceptedGestures: MapGestureArea.PanGesture | MapGestureArea.FlickGesture | MapGestureArea.PinchGesture
    color:  "black"

    property bool   bShowScale:      true
    property double latDBC:          0
    property double lonDBC:          0
    property point  pntDBC:          Qt.point(0,0)
    property bool   bDBC:            false
    property real   fScaleBottom:    0

    signal clickBtn(string txt)
    function updateMap() {
        var mapTypeName = mapManager.propertyMapName()
        for (var i = 0; i < root.supportedMapTypes.length; i++) {
            if(mapTypeName === root.supportedMapTypes[i].name)
            {
                root.activeMapType = root.supportedMapTypes[i]
                calculateScale()
                break
            }
        }
    }

    //计算比例尺
    function calculateScale() {
        if (!bShowScale)
            return
        scaleText.text = mapManager.calculatePixLength(toCoordinate(Qt.point(0,0)), toCoordinate(Qt.point(0,100)))
    }
    Component.onCompleted: {
        updateMap()
    }
    Timer {
        id:         scaleTimer
        interval:   100
        running:    false
        repeat:     false
        onTriggered: calculateScale()
    }
    onZoomLevelChanged: {
        calculateScale()
        if(root.zoomLevel !== mapManager.zoomLevel)
            mapManager.zoomLevel = root.zoomLevel
        if (bDBC) {
            var newCoor = root.toCoordinate(pntDBC,false)
            bDBC = false
            var azimuth = newCoor.azimuthTo(root.center)
            var distance = newCoor.distanceTo(root.center)
            root.center = QtPositioning.coordinate(latDBC, lonDBC).atDistanceAndAzimuth(distance, azimuth)
        }
    }
    onWidthChanged:     scaleTimer.restart()
    onHeightChanged:    scaleTimer.restart()
    onCenterChanged:    mapManager.mapCenter = center
    MouseArea{
        anchors.fill: parent
        onDoubleClicked: {
            bDBC = true
            pntDBC = Qt.point(mouse.x, mouse.y)
            var coor = toCoordinate(pntDBC, false)
            latDBC = coor.latitude
            lonDBC = coor.longitude
            root.zoomLevel++
        }
    }
    //比例尺
    Item {
        id: scale
        z:  200
        visible: scaleText.text != "0 m" && bShowScale
        anchors {bottom: parent.bottom; bottomMargin: 10 + fScaleBottom; left: parent.left; leftMargin: 10}
        height: scaleText.height + scaleImageLeft.height+4
        width: mapManager.propLength

        Image {
            id: scaleImageLeft
            source: "qrc:/images/scale_tip.png"
            anchors {left: parent.left; bottom: parent.bottom}
        }
        Image {
            id: scaleImage
            source: "qrc:/images/scale.png"
            anchors{bottom: parent.bottom; left: parent.left; right: parent.right}
        }
        Image {
            id: scaleImageRight
            source: "qrc:/images/scale_tip.png"
            anchors {bottom: parent.bottom; right: parent.right}
        }
        Label {
            id: scaleText
            color:   "white"
            text:    "0 m"
            font:     vgMainPage.littleFont()
            anchors   {top: parent.top; horizontalCenter: parent.horizontalCenter}
        }
        Component.onCompleted: {
            root.calculateScale()
        }
    }
    Connections{
        target:mapManager
        onMapTypeIDChanged: {
            updateMap()
        }
        onSigMapCenterChanged: {
            root.center.latitude = lat;
            root.center.longitude = lon;
        }
    }
    Connections{
        target: vgMainPage
        onSigShowPage: {
            var vis = vgMainPage.isCurQmlChild(root)
            if (vis)
                updateMap()
        }
    }
}
