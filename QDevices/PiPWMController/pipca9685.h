#ifndef PIPCA9685_H
#define PIPCA9685_H

#include <QObject>
#include <QDebug>
#include <QtMath>

#include "PiBus/qi2c.h"

#define MODE1			0x00
#define MODE2			0x01
#define SUBADR1			0x02
#define SUBADR2			0x03
#define SUBADR3			0x04
#define PRESCALE		0xFE
#define LED0_ON_L		0x06
#define LED0_ON_H		0x07
#define LED0_OFF_L		0x08
#define LED0_OFF_H		0x09
#define ALL_LED_ON_L    0xFA
#define ALL_LED_ON_H	0xFB
#define ALL_LED_OFF_L	0xFC
#define ALL_LED_OFF_H	0xFD
// Bits
#define RESTART			0x80
#define SLEEP			0x10
#define ALLCALL			0x01
#define INVRT			0x10
#define OUTDRV			0x04

#define PAN			    0
#define TILT			1
#define FREQUENCY		50
#define CLOCKFREQ		25000000
#define PANOFFSET		1
#define PANSCALE		1.4
#define TILTOFFSET		30
#define TILTSCALE		1.43
#define PANMAX			270
#define PANMIN			90
#define TILTMAX			90
#define TILTMIN			-45

class PiPCA9685 : public QObject
{
    Q_OBJECT
public:
    explicit PiPCA9685(uint8_t i2c_bus, uint8_t i2c_address, QObject *parent = nullptr);
    ~PiPCA9685();
public slots:
    void initialize();
signals:
private slots:
    void setPwmFrequency(uint16_t frequency);
    void setPwm(uint8_t channel, uint16_t on, uint16_t off);
    void setAllPwm(uint16_t on, uint16_t off);
    void move(uint8_t channel , int deg);

private:
    QI2C *i2c;

    const uint8_t i2c_bus;
    const uint8_t i2c_address;
};

#endif // PIPCA9685_H
