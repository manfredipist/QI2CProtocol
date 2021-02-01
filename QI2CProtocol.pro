QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    QBus/qi2c.cpp \
    QDevices/PiI2CMultiplexer/pitca9548a.cpp \
    QDevices/PiSensors/pihcsr04.cpp \
    QDevices/PiSensors/pimpu6050.cpp \
    QDevices/PiSensors/piqmc5883l.cpp \
    QDevices/PiSensors/pitof10120.cpp \
    QDevices/PiSensors/pivl53l1x.cpp \
    QDevices/PiPWMController/pipca9685.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    QBus/qi2c.h \
    QDevices/PiI2CMultiplexer/pitca9548a.h \
    QDevices/PiSensors/pihcsr04.h \
    QDevices/PiSensors/pimpu6050.h \
    QDevices/PiSensors/piqmc5883l.h \
    QDevices/PiSensors/pitof10120.h \
    QDevices/PiSensors/pivl53l1x.h \
    QDevices/PiPWMController/pipca9685.h \
    mainwindow.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
