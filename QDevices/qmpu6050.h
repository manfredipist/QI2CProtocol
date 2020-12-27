#ifndef PIGYROACCELEROMETER_H
#define PIGYROACCELEROMETER_H

#include <QObject>
#include <QDebug>
#include <QtMath>
#include <QThread>

#include "QBus/qi2c.h"

// WHO_AM_I registers
#define MPU6050_RA_WHO_AM_I                0x75
#define MPU6050_WHO_AM_I_BIT               6
#define MPU6050_WHO_AM_I_LENGTH            6

//GENERAL_CONFIG registers
#define MPU6050_RA_CONFIG                  0x1A
#define MPU6050_RA_SMPLRT_DIV              0x19

// CLOCK registers
#define MPU6050_CLOCK_INTERNAL          0x00
#define MPU6050_CLOCK_PLL_XGYRO         0x01
#define MPU6050_CLOCK_PLL_YGYRO         0x02
#define MPU6050_CLOCK_PLL_ZGYRO         0x03
#define MPU6050_CLOCK_PLL_EXT32K        0x04
#define MPU6050_CLOCK_PLL_EXT19M        0x05
#define MPU6050_CLOCK_KEEP_RESET        0x07

// ACCEL_CONFIG registers
#define MPU6050_RA_ACCEL_CONFIG            0x1C
#define MPU6050_ACONFIG_AFS_SEL_BIT        4
#define MPU6050_ACONFIG_AFS_SEL_LENGTH     2
#define MPU6050_ACCEL_FS_2                 0x00
#define MPU6050_ACCEL_FS_4                 0x01
#define MPU6050_ACCEL_FS_8                 0x02
#define MPU6050_ACCEL_FS_16                0x03

// ACCEL_*OUT_* registers
#define MPU6050_RA_ACCEL_XOUT_H            0x3B
#define MPU6050_RA_ACCEL_XOUT_L            0x3C
#define MPU6050_RA_ACCEL_YOUT_H            0x3D
#define MPU6050_RA_ACCEL_YOUT_L            0x3E
#define MPU6050_RA_ACCEL_ZOUT_H            0x3F
#define MPU6050_RA_ACCEL_ZOUT_L            0x40

// GYRO_CONFIG registers
#define MPU6050_RA_GYRO_CONFIG             0x1B
#define MPU6050_GCONFIG_FS_SEL_BIT         4
#define MPU6050_GCONFIG_FS_SEL_LENGTH      2
#define MPU6050_GYRO_FS_250                0x00
#define MPU6050_GYRO_FS_500                0x01
#define MPU6050_GYRO_FS_1000               0x02
#define MPU6050_GYRO_FS_2000               0x03

// GYRO_*OUT_* registers
#define MPU6050_RA_GYRO_XOUT_H             0x43
#define MPU6050_RA_GYRO_XOUT_L             0x44
#define MPU6050_RA_GYRO_YOUT_H             0x45
#define MPU6050_RA_GYRO_YOUT_L             0x46
#define MPU6050_RA_GYRO_ZOUT_H             0x47
#define MPU6050_RA_GYRO_ZOUT_L             0x48

// PWR_MGMT_1 registers
#define MPU6050_RA_PWR_MGMT_1              0x6B
#define MPU6050_RA_PWR_MGMT_2              0x6C
#define MPU6050_PWR1_DEVICE_RESET_BIT =    7
#define MPU6050_PWR1_SLEEP_BIT             6
#define MPU6050_PWR1_CYCLE_BIT             5
#define MPU6050_PWR1_TEMP_DIS_BIT          3
#define MPU6050_PWR1_CLKSEL_BIT            2
#define MPU6050_PWR1_CLKSEL_LENGTH         3

// TEMP_*OUT_* register
#define MPU6050_RA_TEMP_OUT                0x41


#define GRAVITIY_MS2                        9.80665
//Scale Modifiers
#define ACCEL_SCALE_MODIFIER_2G             16384.0
#define ACCEL_SCALE_MODIFIER_4G             8192.0
#define ACCEL_SCALE_MODIFIER_8G             4096.0
#define ACCEL_SCALE_MODIFIER_16G            2048.0

#define GYRO_SCALE_MODIFIER_250DEG          131.0
#define GYRO_SCALE_MODIFIER_500DEG          65.5
#define GYRO_SCALE_MODIFIER_1000DEG         32.8
#define GYRO_SCALE_MODIFIER_2000DEG         16.4

// Pre-defined ranges
#define ACCEL_RANGE_2G 0x00
#define ACCEL_RANGE_4G 0x08
#define ACCEL_RANGE_8G 0x10
#define ACCEL_RANGE_16G 0x18

#define GYRO_RANGE_250DEG 0x00
#define GYRO_RANGE_500DEG 0x08
#define GYRO_RANGE_1000DEG 0x10
#define GYRO_RANGE_2000DEG 0x18

#define CONSIDER_GRAVITY 0
#define TAU 0.05 //Complementary filter percentage
#define RAD_T_DEG 57.29577951308 //Radians to degrees (180/PI)

class QMPU6050 : public QObject
{
    Q_OBJECT
public:
    explicit QMPU6050(const uint8_t i2c_bus, const uint8_t i2c_address, QObject *parent = nullptr);
    ~QMPU6050();
public slots:
    void initialize();    

    double getTemperature();

    void getAccelerationRaw(float* x, float* y, float* z);
    void getAcceleration(float *x, float *y, float *z);
    int16_t getAccelerationX();
    int16_t getAccelerationY();
    int16_t getAccelerationZ();

    void getRotationRaw(float* x, float* y, float* z);
    void getRotation(float* x, float* y, float* z);
    int16_t getRotationX();
    int16_t getRotationY();
    int16_t getRotationZ();

    void getMotion6(float *ax, float *ay, float *az, float *gx, float *gy, float *gz);
    void getFilteredMotion();
    float getYaw();
signals:
private slots:
    uint8_t getDeviceID();
    bool testConnection();

    void setSleepEnabled(uint8_t enabled);
    void setDLPF(uint8_t hz);
    void setSampleRateDivider(uint8_t rate);
    void setClockSource(uint8_t source);

    uint16_t getGyroRange(const bool &raw);
    void setGyroRange(uint16_t range);
    uint16_t getAccelRange(const bool &raw);
    void setAccelRange(uint8_t range);

    void calibrate();
    void setCalibrationOffsets(const float &c1, const float &c2, const float &c3,
                               const float &c4, const float &c5, const float &c6);
    void resetCalibrationOffsets();

private:
    QI2C *i2c;

    const uint8_t i2c_bus;
    const uint8_t i2c_address;

    QVector<QVector<float>> calibration;

    //Store all angles (accel roll, accel pitch, accel yaw, gyro roll, gyro pitch, gyro yaw, comb roll, comb pitch comb yaw)
    float accel_angle[3];
    float gyro_angle[3];
    float angle[3];

    bool first_run = true;
    bool calc_yaw = false;

    float dt; //Loop time (recalculated with each loop)
    struct timespec start,end; //Create a time structure
};

#endif // PIGYROACCELEROMETER_H
