QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    QBus/qi2c.cpp \
    QDevices/qi2cmultiplexer.cpp \
    QDevices/qmpu6050.cpp \
    QDevices/qqmc5883l.cpp \
    QDevices/qtof10120.cpp \
    QDevices/qvl53l1x.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    QBus/qi2c.h \
    QDevices/qi2cmultiplexer.h \
    QDevices/qmpu6050.h \
    QDevices/qqmc5883l.h \
    QDevices/qtof10120.h \
    QDevices/qvl53l1x.h \
    mainwindow.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
