#ifndef PIRANGEFINDER_H
#define PIRANGEFINDER_H

#include <QObject>
#include <QDebug>
#include <QtMath>

#include "QBus/qi2c.h"

class QTOF10120 : public QObject
{
    Q_OBJECT
public:
    explicit QTOF10120(uint8_t i2c_bus, uint8_t i2c_address, QObject *parent = nullptr);
    ~QTOF10120();
public slots:
    void initialize();
    uint16_t getDistance();
signals:
private:
    QI2C *i2c;

    const uint8_t i2c_bus;
    const uint8_t i2c_address;
};

#endif // PIRANGEFINDER_H
