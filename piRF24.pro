QT += core
QT -= gui

CONFIG += c++11

TARGET = piRF24
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SPEC = $${QMAKESPEC}
message("QMAKESPEC: $${SPEC}")

SYSROOT = $$[QT_SYSROOT]
message("QMAKESYSROOT: $${SYSROOT}")

isEmpty(SYSROOT) {
        message("SYSROOT empty, assuming compiling for host")
}

!isEmpty(SYSROOT) {
        message("SYSROOT not empty, assuming compiling for target")
        DEFINES += __RPI__
        LIBS += -lwiringPi
        LIBS += -lrf24-bcm
        LIBS += -lrf24
        LIBS += -lrf24network
        LIBS += -lrf24mesh
        message("$${LIBS}")
}

SOURCES += main.cpp \
    rf24interface.cpp \
    rf24functions.cpp \
    wiringpiinterface.cpp

HEADERS += \
    rf24interface.h \
    rf24functions.h \
    wiringpiinterface.h

unix {
    target.path = /usr/share/piRF24
    INSTALLS += target
}
