#ifndef PIMAGNETOMETER_H
#define PIMAGNETOMETER_H

#include <QObject>
#include <QDebug>
#include <QtMath>

#include "QBus/qi2c.h"

/* Register numbers */
#define QMC5883L_X_LSB          0x00
#define QMC5883L_X_MSB          0x01
#define QMC5883L_Y_LSB          0x02
#define QMC5883L_Y_MSB          0x03
#define QMC5883L_Z_LSB          0x04
#define QMC5883L_Z_MSB          0x05
#define QMC5883L_STATUS         0x06
#define QMC5883L_TEMP_LSB       0x07
#define QMC5883L_TEMP_MSB       0x08
#define QMC5883L_CONFIG         0x09
#define QMC5883L_CONFIG2        0x0A
#define QMC5883L_RESET          0x0B
#define QMC5883L_RESERVED       0x0C
#define QMC5883L_CHIP_ID        0x0D

/* Values for the STATUS register */
#define QMC5883L_STATUS_DRDY    0x01        //Data ready
#define QMC5883L_STATUS_OVL     0x02        //Overflow flag
#define QMC5883L_STATUS_DOR     0x04        //Data skipped for reading

/* Values for the CONFIG2 register */
#define QMC5883L_CONFIG2_IENB   0x01        //Interrupt Pin Enabling.
#define QMC5883L_CONFIG2_POL    0x40        //Pointer Roll-over.
#define QMC5883L_CONFIG2_SRST   0x80        //Soft Reset.

/* Oversampling values for the CONFIG register */
#define QMC5883L_CONFIG_OS512   0x00
#define QMC5883L_CONFIG_OS256   0x40
#define QMC5883L_CONFIG_OS128   0x80
#define QMC5883L_CONFIG_OS64    0xC0

/* Range values for the CONFIG register */
#define QMC5883L_CONFIG_2GAUSS  0x00
#define QMC5883L_CONFIG_8GAUSS  0x10

/* Rate values for the CONFIG register */
#define QMC5883L_CONFIG_10HZ    0x00
#define QMC5883L_CONFIG_50HZ    0x04
#define QMC5883L_CONFIG_100HZ   0x08
#define QMC5883L_CONFIG_200HZ   0x0C

/* Mode values for the CONFIG register */
#define QMC5883L_CONFIG_STANDBY 0x00
#define QMC5883L_CONFIG_CONT    0x01

#define DECLINATION 3.339

class QQMC5883L : public QObject
{
    Q_OBJECT
public:
    explicit QQMC5883L(const uint8_t i2c_bus, const uint8_t i2c_address, QObject *parent = nullptr);
    ~QQMC5883L();
public slots:
    void initialize();

    bool isDataReady();
    int readHeading();
    int readCalibratedHeading();
signals:
private slots:
    void softReset();
    void defineSetResetPeriod();
    void disableInterrupt();
    void setConfiguration();

    void setMode(const int &x);
    void setOversampling(const int &x);
    void setRange(const int &x);
    void setSamplingRate(const int &x);

    int readRaw(int16_t *x, int16_t *y, int16_t *z);

    void calibrate();
    void setCalibrationOffsets(const int &c1, const int &c2, const int &c3,
                        const int &c4, const int &c5, const int &c6);
    void resetCalibrationOffsets();

    void setDeclination(const float &declination);
    void resetDeclination();

private:
    QI2C *i2c;

    const uint8_t i2c_bus;
    const uint8_t i2c_address;

    //QVector<QVector<float>> calibration;
    QVector<QVector<int>> calibration;
    float declination = 10.0;

    uint8_t mode;
    uint8_t rate;
    uint8_t range;
    uint8_t oversampling;
};

#endif // PIMAGNETOMETER_H
