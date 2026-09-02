TEMPLATE = lib

include(../libs.pri)

QT -= gui
QT += core

DEFINES += VGBASE_LIBRARY
CONFIG += x11 staticlib

INCLUDEPATH += .

INCLUDEPATH += src

SOURCES += \
    src/base.cc \

HEADERS += \
    src/base.h \
    src/vgbase_global.h


DESTDIR = $${OUT_ROOT}
