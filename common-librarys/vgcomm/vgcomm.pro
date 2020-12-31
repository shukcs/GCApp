TEMPLATE = lib

include(../libs.pri)

DEFINES += QGC_ENABLE_BLUETOOTH
CONFIG += x11 staticlib

QT -= gui
QT += concurrent serialport network

contains(DEFINES, QGC_ENABLE_BLUETOOTH) {
    QT += bluetooth
}

DEFINES += VGCOMM_LIBRARY

INCLUDEPATH += .

INCLUDEPATH += src \
    ../vgbase/src \
    ../../mavlink/v2.0/common \
    ../../mavlink/v2.0

SOURCES += \
    src/LinkCommand.cc \
    src/LinkManager.cc \
    src/MAVLinkProtocol.cc \
    src/SerialPortInfo.cc \
    src/SerialLink.cc \
    src/UDPLink.cc \
    src/LinkInterface.cc

contains(DEFINES, QGC_ENABLE_BLUETOOTH) {
    SOURCES += src/BluetoothLink.cc
}

HEADERS += \
    src/LinkCommand.h \
    src/LinkInterface.h \
    src/LinkManager.h \
    src/MAVLinkProtocol.h \
    src/SerialPortInfo.h \
    src/SerialLink.h \
    src/UDPLink.h \
    src/vgcomm_global.h

contains(DEFINES, QGC_ENABLE_BLUETOOTH) {
    HEADERS += src/BluetoothLink.h
}

DESTDIR = $${OUT_ROOT}
