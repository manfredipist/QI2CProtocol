#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    i2cMultiplexer = new QI2CMultiplexer(this);

    double front_laser = i2cMultiplexer->getFrontLaser();
    double left_laser = i2cMultiplexer->getLeftLaser();
    double right_laser = i2cMultiplexer->getRightLaser();

    int angle = i2cMultiplexer->getAngle();

    double temperature = i2cMultiplexer->getTemperature();

    float accX, accY, accZ, gyroX, gyroY, gyroZ;
    i2cMultiplexer->getMotion6(&accX, &accY, &accZ, &gyroX, &gyroY, &gyroZ);

    qDebug()<<"Front Laser: "<<front_laser << "cm";
    qDebug()<<"Left Laser: " << left_laser << "cm";
    qDebug()<<"Right Laser: " << right_laser << "cm";

    qDebug()<<"Compass Angle: " << angle << "°";
    qDebug()<<"Temperature: " << temperature << "C°";
    qDebug()<<"AccX: " << accX << "  AccY: " << accY << "  AccZ: " << accZ;
    qDebug()<<"GyroX: " << gyroX << "  GyroY: " << gyroY << "  GyroZ: " << gyroZ;

}

MainWindow::~MainWindow()
{

}
