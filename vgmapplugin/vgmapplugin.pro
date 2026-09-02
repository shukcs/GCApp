TEMPLATE = lib
CONFIG += x11 staticlib

QT  += widgets location-private positioning-private network positioning \
location sql

INCLUDEPATH += . \
    ../common-librarys/vgbase/src \
	
HEADERS += QGCMapEngine.h \
    QGCMapEngineData.h \
    QGCMapTileSet.h \
    QGCMapUrlEngine.h \
    QGCTileCacheWorker.h \
    QGeoCodeReplyQGC.h \
    QGeoCodingManagerEngineQGC.h \
    QGeoMapReplyQGC.h \
    QGeoServiceProviderPluginQGC.h \
    QGeoTileFetcherQGC.h \
    QGeoTiledMappingManagerEngineQGC.h \
    VGMapGlobalFunction.h

SOURCES += QGCMapEngine.cpp \
    QGCMapTileSet.cpp \
    QGCMapUrlEngine.cpp \
    QGCTileCacheWorker.cpp \
    QGeoCodeReplyQGC.cpp \
    QGeoCodingManagerEngineQGC.cpp \
    QGeoMapReplyQGC.cpp \
    QGeoServiceProviderPluginQGC.cpp \
    QGeoTileFetcherQGC.cpp \
    QGeoTiledMappingManagerEngineQGC.cpp\
    VGMapGlobalFunction.cpp

OTHER_FILES += vg_maps_plugin.json

!iOSBuild {
    OBJECTS_DIR  = $${OUT_PWD}/obj
    MOC_DIR      = $${OUT_PWD}/moc
    UI_DIR       = $${OUT_PWD}/ui
    RCC_DIR      = $${OUT_PWD}/rcc
}

CONFIG(debug, debug|release) {
    DESTDIR = $${OUT_PWD}/../output/debug/
}
else:CONFIG(release, debug|release) {
    DESTDIR = $${OUT_PWD}/../output/release/
}
