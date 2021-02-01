#ifndef PITCA9548A_H
#define PITCA9548A_H

#include <QObject>
#include <QDebug>
#include <QMutex>
#include <QMutexLocker>

#include "PiBus/qi2c.h"
#include "PiDevices/PiSensors/pivl6180x.h"
#include "PiDevices/PiSensors/pivl53l1x.h"
#include "PiDevices/PiSensors/piqmc5883l.h"
#include "PiDevices/PiSensors/pimpu6050.h"
#include "PiDevices/PiPWMController/pipca9685.h"

class PiTCA9548A : public QObject
{
    Q_OBJECT
public:
    explicit PiTCA9548A(QObject *parent = nullptr);
    ~PiTCA9548A();
public slots:
    double getFrontLaser();

    double getLeftLaser();
    double getRightLaser();

    int getAngle();

    double getTemperature();
    void getMotion6(float* ax, float* ay, float* az, float* gx, float* gy, float* gz);

signals:
private slots:
    void initializeFrontLaser();
    void initializeLeftRightLasers();
    void initializeMagnetometer();
    void initializeGyroAccelerometer();
    void initializePwmController();

private:    
    Q_DISABLE_COPY(PiTCA9548A)
    QMutex m_mutex;

    QI2C *i2c;

    const uint8_t i2c_bus = 0x1;
    const uint8_t i2c_multiplexer_address = 0x70;

    const uint8_t i2c_vl6180x_address = 0x29;
    const uint8_t i2c_vl53l1x_address = 0x29;
    const uint8_t i2c_qmc5883l_address = 0x0D;
    const uint8_t i2c_mpu6050_address = 0x68;
    const uint8_t i2c_pca9685_address = 0x40;


    PiVL6180X* piVL6180X;
    const uint8_t piVL6180X_bus = 0x5;

    PiVL53L1X* piVL53L1X_L;
    const uint8_t piVL53L1X_L_bus = 0x6;
    PiVL53L1X* piVL53L1X_R;
    const uint8_t piVL53L1X_R_bus = 0x4;

    PiQMC5883L* piQMC5883L;
    const uint8_t piQMC5883L_bus = 0x2;

    PiMPU6050* piMPU6050;
    const uint8_t piMPU6050_bus = 0x3;

    PiPCA9685* piPCA9685;
    const uint8_t piPCA9685_bus = 0x7;

};

#endif // PITCA9548A_H
