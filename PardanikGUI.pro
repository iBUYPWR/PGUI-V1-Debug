QT       += core gui
QT += serialport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    connectionmodal.cpp \
    hzmodal.cpp \
    main.cpp \
    mainwindow.cpp \
    portal.cpp \
    processor.cpp \
    txmodal.cpp \
    worker.cpp

HEADERS += \
    connectionmodal.h \
    hzmodal.h \
    mainwindow.h \
    newtype.h \
    portal.h \
    processor.h \
    txmodal.h \
    worker.h

FORMS += \
    connectionmodal.ui \
    hzmodal.ui \
    mainwindow.ui \
    txmodal.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources/green.png \
    Resources/red.png \
    Resources/gray.png \
    Resources/logo.png \
    Resources/logo.ico



