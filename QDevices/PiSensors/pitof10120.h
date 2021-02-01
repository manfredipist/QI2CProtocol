#ifndef PITOF10120_H
#define PITOF10120_H

#include <QObject>
#include <QDebug>
#include <QtMath>

#include "PiBus/qi2c.h"

class PiTOF10120 : public QObject
{
    Q_OBJECT
public:
    explicit PiTOF10120(uint8_t i2c_bus, uint8_t i2c_address, QObject *parent = nullptr);
    ~PiTOF10120();
public slots:
    void initialize();
    double getDistance();
signals:
private slots:
    uint16_t getUIntDistance();
private:
    QI2C *i2c;

    const uint8_t i2c_bus;
    const uint8_t i2c_address;
};

#endif // PITOF10120_H
