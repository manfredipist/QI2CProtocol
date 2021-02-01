#ifndef PIVL6180X_H
#define PIVL6180X_H

#include <QObject>
#include <QDateTime>
#include <QDebug>
#include <QtMath>

#include "PiBus/qi2c.h"

#define VL6180X_DEFAULT_I2C_ADDR                   0x29
#define VL6180X_REG_IDENTIFICATION_MODEL_ID        0x000
#define VL6180X_REG_SYSTEM_INTERRUPT_CONFIG        0x014
#define VL6180X_REG_SYSTEM_INTERRUPT_CLEAR         0x015
#define VL6180X_REG_SYSTEM_FRESH_OUT_OF_RESET      0x016
#define VL6180X_REG_SYSRANGE_START                 0x018
#define VL6180X_REG_SYSALS_START                   0x038
#define VL6180X_REG_SYSALS_ANALOGUE_GAIN           0x03F
#define VL6180X_REG_SYSALS_INTEGRATION_PERIOD_HI   0x040
#define VL6180X_REG_SYSALS_INTEGRATION_PERIOD_LO   0x041
#define VL6180X_REG_RESULT_ALS_VAL                 0x050
#define VL6180X_REG_RESULT_RANGE_VAL               0x062
#define VL6180X_REG_RESULT_RANGE_STATUS            0x04D
#define VL6180X_REG_RESULT_INTERRUPT_STATUS_GPIO   0x04F

#define ALS_GAIN_1           0x06
#define ALS_GAIN_1_25        0x05
#define ALS_GAIN_1_67        0x04
#define ALS_GAIN_2_5         0x03
#define ALS_GAIN_5           0x02
#define ALS_GAIN_10          0x01
#define ALS_GAIN_20          0x00
#define ALS_GAIN_40          0x07

#define ERROR_NONE           0
#define ERROR_SYSERR_1       1
#define ERROR_SYSERR_5       5
#define ERROR_ECEFAIL        6
#define ERROR_NOCONVERGE     7
#define ERROR_RANGEIGNORE    8
#define ERROR_SNR            11
#define ERROR_RAWUFLOW       12
#define ERROR_RAWOFLOW       13
#define ERROR_RANGEUFLOW     14
#define ERROR_RANGEOFLOW     15

class PiVL6180X : public QObject
{
    Q_OBJECT
public:
    explicit PiVL6180X(uint8_t i2c_bus, uint8_t i2c_address, QObject *parent = nullptr);
    ~PiVL6180X();
public slots:
    void initialize();
    double getDistance();
signals:
private slots:
    uint8_t getRangeMeasurement();
    uint16_t getLuxMeasurement(uint8_t gain);
    int rangeStatus();
    void loadDefaultConfiguration();


private:
    QI2C *i2c;

    const uint8_t i2c_bus;
    const uint8_t i2c_address;

};

#endif // PIVL6180X_H
