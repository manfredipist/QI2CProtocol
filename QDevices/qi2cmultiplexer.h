#ifndef QI2CMULTIPLEXER_H
#define QI2CMULTIPLEXER_H

#include <QObject>
#include <QDebug>
#include <QThread>

#include "QBus/qi2c.h"
#include "QDevices/qtof10120.h"
#include "QDevices/qvl53l1x.h"
#include "QDevices/qqmc5883l.h"
#include "QDevices/qmpu6050.h"

class QI2CMultiplexer : public QObject
{
    Q_OBJECT
public:
    explicit QI2CMultiplexer(QObject *parent = nullptr);
    ~QI2CMultiplexer();
public slots:
    double getFrontLaser();

    double getLeftLaser();
    double getRightLaser();

    int getAngle();

    double getTemperature();
    void getMotion6(float* ax, float* ay, float* az, float* gx, float* gy, float* gz);
    float getYaw();

signals:
private slots:
private:
    QI2C *i2c;

    const uint8_t i2c_bus = 0x1;
    const uint8_t i2c_multiplexer_address = 0x70;

    const uint8_t i2c_tof10120_address = 0x52;
    const uint8_t i2c_vl53l1x_address = 0x29;
    const uint8_t i2c_qmc5883l_address = 0x0D;
    const uint8_t i2c_mpu6050_address = 0x68;

    QTOF10120* qTOF10120;
    const uint8_t TOF10120_bus = 0x4;

    QVL53L1X* qVL53L1X_L;
    const uint8_t VL53L1X_L_bus = 0x2;
    QVL53L1X* qVL53L1X_R;
    const uint8_t VL53L1X_R_bus = 0x7;

    QQMC5883L* qQMC5883L;
    const uint8_t QMC5883L_bus = 0x3;

    QMPU6050* qMPU6050;
    const uint8_t MPU6050_bus = 0x5;
};

#endif // QI2CMULTIPLEXER_H
