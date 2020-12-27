#include "qi2cmultiplexer.h"

QI2CMultiplexer::QI2CMultiplexer(QObject *parent) : QObject(parent)
{
    i2c = new QI2C(i2c_bus,i2c_multiplexer_address);

    qTOF10120 = new QTOF10120(i2c_bus,i2c_tof10120_address);
    i2c->writeBit16(i2c_multiplexer_address, TOF10120_bus, 0x1);
    qTOF10120->initialize();

    qVL53L1X_L = new QVL53L1X(i2c_bus,i2c_vl53l1x_address);
    qVL53L1X_R = new QVL53L1X(i2c_bus,i2c_vl53l1x_address);

    i2c->writeBit16(i2c_multiplexer_address, VL53L1X_L_bus, 0x1);
    qVL53L1X_L->initialize();
    i2c->writeBit16(i2c_multiplexer_address, VL53L1X_R_bus, 0x1);
    qVL53L1X_R->initialize();


    qQMC5883L = new QQMC5883L(i2c_bus,i2c_qmc5883l_address);
    i2c->writeBit16(i2c_multiplexer_address, QMC5883L_bus, 0x1);
    qQMC5883L->initialize();

//    piGyroAccelerometer = new PiGyroAccelerometer(i2c_bus,i2c_gyroaccelerometer_address);
//    i2c->writeBit16(i2c_multiplexer_address, piGyroAccelerometer_bus, 0x1);
//    piGyroAccelerometer->initialize();
}

QI2CMultiplexer::~QI2CMultiplexer(){
    i2c->i2cClose();

    i2c->deleteLater();

    qTOF10120->deleteLater();
    qVL53L1X_L->deleteLater();
    qVL53L1X_R->deleteLater();

    qQMC5883L->deleteLater();
    qMPU6050->deleteLater();
}

double QI2CMultiplexer::getFrontLaser(){
    i2c->writeBit16(i2c_multiplexer_address, TOF10120_bus, 0x1);
    double distance = qTOF10120->getDistance()/10.0;
    return distance;
}

double QI2CMultiplexer::getLeftLaser(){
    i2c->writeBit16(i2c_multiplexer_address, VL53L1X_L_bus, 0x1);
    if(qVL53L1X_L->isDataReady())
        return qVL53L1X_L->getDistance()/10.0;
    else
        return -1;
}

double QI2CMultiplexer::getRightLaser(){
    i2c->writeBit16(i2c_multiplexer_address, VL53L1X_R_bus, 0x1);
    if(qVL53L1X_R->isDataReady())
        return qVL53L1X_R->getDistance()/10.0;
    else
        return -1;
}

int QI2CMultiplexer::getAngle(){
    i2c->writeBit16(i2c_multiplexer_address, QMC5883L_bus, 0x1);
    if(qQMC5883L->isDataReady())
        return qQMC5883L->readCalibratedHeading();
    else
        return -1;
}

double QI2CMultiplexer::getTemperature(){
    i2c->writeBit16(i2c_multiplexer_address, MPU6050_bus, 0x1);
    return qMPU6050->getTemperature();
}

void QI2CMultiplexer::getMotion6(float* ax, float* ay, float* az, float* gx, float* gy, float* gz){
    i2c->writeBit16(i2c_multiplexer_address, MPU6050_bus, 0x1);
    qMPU6050->getMotion6(ax,ay,az,gx,gy,gz);
}

float QI2CMultiplexer::getYaw(){
    return qMPU6050->getYaw();
}
