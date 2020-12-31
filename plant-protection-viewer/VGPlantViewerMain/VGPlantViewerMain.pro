DEFINES += QGC_ENABLE_BLUETOOTH

QT += core gui widgets qml quick network positioning \
location sql texttospeech serialport xml svg location-private positioning-private

contains(DEFINES, QGC_ENABLE_BLUETOOTH){
    QT += bluetooth
}

include($$PWD/../../common-librarys/libs.pri)
include($$PWD/VGPlantViewerUI/VGPlantViewerUI.pri)

AndroidBuild {
    QT += androidextras
}
WindowsBuild{
    QT += axcontainer
}

CONFIG += qt \
    thread \
    c++11

INCLUDEPATH +=  $$PWD/../../mavlink/v2.0/common \
                $$PWD/../../mavlink/v2.0 \
                $$PWD/../../common-librarys/vgbase/src \
                $$PWD/../../common-librarys/vgcomm/src \
                $$PWD/../../protobuf \
                $$PWD/../share \
                $$PWD/../base \
                ../../vgmapplugin \
                $$PWD/../PX4 \
                $$PWD/VGPlantViewerUI \
                $$PWD/VGPlantViewerUI/land \
                $$PWD/VGPlantViewerUI/plant \
                $$PWD/VGPlantViewerUI/survey \
                $$PWD/VGPlantViewerUI/qx


# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Default rules for deployment.

SOURCES += main.cpp \
    VGApplication.cpp \
    VGDbManager.cpp \
    VGNetManager.cpp \
    VGTcpClient.cpp \
    VGToolBox.cpp \
    VGVehicle.cpp \
    VGVehicleLogManager.cpp \
    VGVehicleManager.cpp \
    srcload/VGIconEngine.cpp \
    srcload/VGImageProvider.cpp \
    ../base/logHelper.cpp \
    ../base/VGGlobalFun.cpp \
    ../base/VGMavLinkCode.cpp \
    ../base/ParameterLimit.cpp \
    ../base/ParametersLimit.cpp \
    MissionItem.cc \
    MissionManager.cc

RESOURCES += VGPlantViewer.qrc

HEADERS += \
    VGApplication.h \
    VGDbManager.h \
    VGNetManager.h \
    VGTcpClient.h \
    VGToolBox.h \
    VGVehicle.h \
    VGVehicleLogManager.h \
    VGVehicleManager.h \
    srcload/VGIconEngine.h \
    srcload/VGImageProvider.h \
    ../base/VGMavLinkCode.h \
    ../base/logHelper.h \
    ../base/QLog.h \
    ../base/VGGlobalFun.h \
    ../base/ParameterLimit.h \
    ../base/ParametersLimit.h \
    MissionItem.h \
    MissionManager.h
    
AndroidBuild {
    OTHER_FILES += \
        android/AndroidManifest.xml \
        android/src/QGCActivity.java \
        android/res/xml/device_filter.xml

    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
    xml.files += UavParamsAndEvents.xml
    xml.path = /assets/res
    lng.files += VigaApp.qm
    lng.path = /assets/lng
    INSTALLS += xml lng
}

LIBS += -L$${OUT_ROOT} -lvgbase -lvgcomm -lvglog -lshare -lprotobuf -lvgmapplugin
