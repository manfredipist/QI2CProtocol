#include "pitca9548a.h"

PiTCA9548A::PiTCA9548A(QObject *parent) : QObject(parent), m_mutex()
{
    i2c = new QI2C(i2c_bus,i2c_multiplexer_address,this);

    initializeFrontLaser();
    initializeLeftRightLasers();
    initializeMagnetometer();
    initializeGyroAccelerometer();
    initializePwmController();
}

PiTCA9548A::~PiTCA9548A(){
    i2c->deleteLater();
    piVL6180X->deleteLater();
    piVL53L1X_L->deleteLater();
    piVL53L1X_R->deleteLater();
    piQMC5883L->deleteLater();
    piMPU6050->deleteLater();
    piPCA9685->deleteLater();
}

void PiTCA9548A::initializeFrontLaser(){
    piVL6180X = new PiVL6180X(i2c_bus,i2c_vl6180x_address);
    i2c->writeBit16(i2c_multiplexer_address, piVL6180X_bus, 0x1);
    piVL6180X->initialize();
}

void PiTCA9548A::initializeLeftRightLasers(){
    piVL53L1X_L = new PiVL53L1X(i2c_bus,i2c_vl53l1x_address,this);
    piVL53L1X_R = new PiVL53L1X(i2c_bus,i2c_vl53l1x_address,this);

    i2c->writeBit16(i2c_multiplexer_address, piVL53L1X_L_bus, 0x1);
    piVL53L1X_L->initialize();
    i2c->writeBit16(i2c_multiplexer_address, piVL53L1X_R_bus, 0x1);
    piVL53L1X_R->initialize();
}
void PiTCA9548A::initializeMagnetometer(){
    piQMC5883L = new PiQMC5883L(i2c_bus,i2c_qmc5883l_address,this);
    i2c->writeBit16(i2c_multiplexer_address, piQMC5883L_bus, 0x1);
    piQMC5883L->initialize();
}
void PiTCA9548A::initializeGyroAccelerometer(){
    piMPU6050 = new PiMPU6050(i2c_bus,i2c_mpu6050_address,this);
    i2c->writeBit16(i2c_multiplexer_address, piMPU6050_bus, 0x1);
    piMPU6050->initialize();
}

void PiTCA9548A::initializePwmController(){
    piPCA9685 = new PiPCA9685(i2c_bus,i2c_pca9685_address,this);
    i2c->writeBit16(i2c_multiplexer_address, piPCA9685_bus, 0x1);
    piPCA9685->initialize();
}

double PiTCA9548A::getFrontLaser(){
    QMutexLocker locker(&m_mutex);
    i2c->writeBit16(i2c_multiplexer_address, piVL6180X_bus, 0x1);
    return piVL6180X->getDistance();
}

double PiTCA9548A::getLeftLaser(){
    QMutexLocker locker(&m_mutex);
    i2c->writeBit16(i2c_multiplexer_address, piVL53L1X_L_bus, 0x1);
    return piVL53L1X_L->getDistance();
}

double PiTCA9548A::getRightLaser(){
    QMutexLocker locker(&m_mutex);
    i2c->writeBit16(i2c_multiplexer_address, piVL53L1X_R_bus, 0x1);
    return piVL53L1X_R->getDistance();
}

int PiTCA9548A::getAngle(){
    QMutexLocker locker(&m_mutex);
    i2c->writeBit16(i2c_multiplexer_address, piQMC5883L_bus, 0x1);
    return piQMC5883L->readCalibratedHeading();
}

double PiTCA9548A::getTemperature(){
    i2c->writeBit16(i2c_multiplexer_address, piMPU6050_bus, 0x1);
    return piMPU6050->getTemperature();
}

void PiTCA9548A::getMotion6(float* ax, float* ay, float* az, float* gx, float* gy, float* gz){
    QMutexLocker locker(&m_mutex);
    i2c->writeBit16(i2c_multiplexer_address, piMPU6050_bus, 0x1);
    piMPU6050->getMotion6(ax,ay,az,gx,gy,gz);
}


