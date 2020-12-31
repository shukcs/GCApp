QT += qml quick network positioning location-private positioning-private\
location sql texttospeech svg

include(VGViewerUI/VGViewerUI.pri)

AndroidBuild {
    QT += androidextras
}
WindowsBuild{
    QT += axcontainer
}

CONFIG += qt \
    thread \
    c++11

INCLUDEPATH +=  ../PX4 \
    ../base \
    VGViewerUI \
    VGViewerUI/plant \
    ../../common-librarys/vgbase/src \
    ../../vgmapplugin \
    ../../protobuf

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
    VGNetManager.cpp \
    VGTcpClient.cpp \
    srcload/VGIconEngine.cpp \
    srcload/VGImageProvider.cpp \
    ../base/VGGlobalFun.cpp \

RESOURCES += VGPlantViewer.qrc

HEADERS += \
    VGApplication.h \
    VGNetManager.h \
    VGTcpClient.h \
    srcload/VGIconEngine.h \
    srcload/VGImageProvider.h \
    ../base/VGMavLinkCode.h \
    ../base/VGGlobalFun.h
    
AndroidBuild {
    OTHER_FILES += \
        android/AndroidManifest.xml \
        android/src/QGCActivity.java \
        android/res/xml/device_filter.xml

    lng.files += VigaApp.qm
    lng.path = /assets/lng
    INSTALLS += lng
}

!iOSBuild {
    OBJECTS_DIR  = $${OUT_PWD}/obj
    MOC_DIR      = $${OUT_PWD}/moc
    UI_DIR       = $${OUT_PWD}/ui
    RCC_DIR      = $${OUT_PWD}/rcc
}

CONFIG(debug, debug|release) {
    LIBS += -L$${OUT_PWD}/../../output/debug/ -lvgbase -lprotobuf -lvgmapplugin
} else {
    LIBS += -L$${OUT_PWD}/../../output/release/ -lvgbase -lprotobuf -lvgmapplugin
}
