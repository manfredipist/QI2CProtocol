#include "qmpu6050.h"

QMPU6050::QMPU6050(const uint8_t i2c_bus, const uint8_t i2c_address, QObject *parent) : QObject(parent), i2c_bus(i2c_bus), i2c_address(i2c_address)
{
    i2c = new QI2C(i2c_bus,i2c_address,this);
}

QMPU6050::~QMPU6050(){
    i2c->i2cClose();
}

void QMPU6050::initialize(){
    setSleepEnabled(0);
    setGyroRange(250);
    setAccelRange(2);

    if(testConnection())
        qDebug()<<"MPU6050 Initialization: SUCCESS";
    else
        qDebug()<<"MPU6050 Initialization: FAILURE";

    //calibrate();
    //setCalibrationOffsets(494.299, -170.241, 2763.41, -311.272, 256.172, 76.2543);
    setCalibrationOffsets(332.575, -239.116, 2581.87, -341.182, 232.903, -13.7107);
    clock_gettime(CLOCK_REALTIME, &start); //Read current time into start variable
}

uint8_t QMPU6050::getDeviceID() {
    uint8_t buffer = i2c->readBits8(MPU6050_RA_WHO_AM_I, MPU6050_WHO_AM_I_BIT, MPU6050_WHO_AM_I_LENGTH);
    return buffer;
}

bool QMPU6050::testConnection() {
    return getDeviceID() == 0x34;
}

void QMPU6050::setSleepEnabled(uint8_t enabled) {
    i2c->writeBit8(MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_SLEEP_BIT, enabled);
}

void QMPU6050::setDLPF(uint8_t hz) {
    i2c->writeByte8(MPU6050_RA_CONFIG, hz);
}

void QMPU6050::setSampleRateDivider(uint8_t rate) {
    i2c->writeByte8(MPU6050_RA_SMPLRT_DIV, rate);
}

void QMPU6050::setClockSource(uint8_t source) {
    i2c->writeBits8(MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_CLKSEL_BIT, MPU6050_PWR1_CLKSEL_LENGTH, source);
}

uint16_t QMPU6050::getGyroRange(const bool &raw){
    uint8_t raw_data = i2c->readByte8(MPU6050_RA_GYRO_CONFIG);

    if(raw)
        return raw_data;
    else{
        if(raw_data == GYRO_RANGE_250DEG)
            return 250;
        else if(raw_data == GYRO_RANGE_500DEG)
            return 500;
        else if(raw_data == GYRO_RANGE_1000DEG)
            return 1000;
        else if(raw_data == GYRO_RANGE_2000DEG)
            return 2000;
        else
            return -1;
    }
}

void QMPU6050::setGyroRange(uint16_t range){
    if(range == 250)
        i2c->writeBits8(MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, GYRO_RANGE_250DEG);
    else if(range == 500)
        i2c->writeBits8(MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, GYRO_RANGE_500DEG);
    else if(range == 1000)
        i2c->writeBits8(MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, GYRO_RANGE_1000DEG);
    else if(range == 2000)
        i2c->writeBits8(MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, GYRO_RANGE_2000DEG);
}

uint16_t QMPU6050::getAccelRange(const bool &raw){
    uint8_t raw_data = i2c->readByte8(MPU6050_RA_ACCEL_CONFIG);

    if(raw)
        return raw_data;
    else{
        if(raw_data == ACCEL_RANGE_2G)
            return 2;
        else if(raw_data == ACCEL_RANGE_4G)
            return 4;
        else if(raw_data == ACCEL_RANGE_8G)
            return 8;
        else if(raw_data == ACCEL_RANGE_16G)
            return 16;
        else
            return -1;
    }
}

void QMPU6050::setAccelRange(uint8_t range) {
    if(range == 2)
        i2c->writeBits8(MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, ACCEL_RANGE_2G);
    else if(range == 4)
        i2c->writeBits8(MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, ACCEL_RANGE_4G);
    else if(range == 8)
        i2c->writeBits8(MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, ACCEL_RANGE_8G);
    else if(range == 16)
        i2c->writeBits8(MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, ACCEL_RANGE_16G);
}

double QMPU6050::getTemperature() {
    int16_t buffer = i2c->readWord8(MPU6050_RA_TEMP_OUT);
    double temp = buffer/340.0+36.5;
    return (int)(temp*100+0.5)/100.0;
}

void QMPU6050::getAccelerationRaw(float* x, float* y, float* z) {
    *x = getAccelerationX();
    *y = getAccelerationY();
    *z = getAccelerationZ();
}

void QMPU6050::getAcceleration(float* x, float* y, float* z) {
    float _x = getAccelerationX();
    float _y = getAccelerationY();
    float _z = getAccelerationZ();

    float accel_scale_modifier;
    uint8_t accel_range = getAccelRange(true);

    if(accel_range == ACCEL_RANGE_2G)
        accel_scale_modifier = ACCEL_SCALE_MODIFIER_2G;
    else if(accel_range == ACCEL_RANGE_4G)
        accel_scale_modifier = ACCEL_SCALE_MODIFIER_4G;
    else if(accel_range == ACCEL_RANGE_8G)
        accel_scale_modifier = ACCEL_SCALE_MODIFIER_8G;
    else if(accel_range == ACCEL_RANGE_16G)
        accel_scale_modifier = ACCEL_SCALE_MODIFIER_16G;
    else
        accel_scale_modifier = ACCEL_SCALE_MODIFIER_2G;

    *x = qRound((_x - calibration[0][0]) * 1000.0 / accel_scale_modifier) / 1000.0;
    *y = qRound((_y - calibration[0][1]) * 1000.0 / accel_scale_modifier) / 1000.0;
    *z = qRound((_z - calibration[0][2]) * 1000.0 / accel_scale_modifier) / 1000.0;

    if(CONSIDER_GRAVITY){
        *x *= GRAVITIY_MS2;
        *y *= GRAVITIY_MS2;
        *z *= GRAVITIY_MS2;
    }
}

int16_t QMPU6050::getAccelerationX() {
    int16_t buffer = i2c->readWord8(MPU6050_RA_ACCEL_XOUT_H);
    return buffer;
}

int16_t QMPU6050::getAccelerationY() {
    int16_t buffer = i2c->readWord8(MPU6050_RA_ACCEL_YOUT_H);
    return buffer;
}

int16_t QMPU6050::getAccelerationZ() {
    int16_t buffer = i2c->readWord8(MPU6050_RA_ACCEL_ZOUT_H);
    return buffer;
}

void QMPU6050::getRotationRaw(float *x, float *y, float *z) {
    *x = getRotationX();
    *y = getRotationY();
    *z = getRotationZ();
}

void QMPU6050::getRotation(float *x, float *y, float *z) {
    float _x = getRotationX();
    float _y = getRotationY();
    float _z = getRotationZ();

    float gyro_scale_modifier;
    uint8_t gyro_range = getGyroRange(true);

    if(gyro_range == GYRO_RANGE_250DEG)
        gyro_scale_modifier = GYRO_SCALE_MODIFIER_250DEG;
    else if(gyro_range == GYRO_RANGE_500DEG)
        gyro_scale_modifier = GYRO_SCALE_MODIFIER_500DEG;
    else if(gyro_range == GYRO_RANGE_1000DEG)
        gyro_scale_modifier = GYRO_SCALE_MODIFIER_1000DEG;
    else if(gyro_range == GYRO_RANGE_2000DEG)
        gyro_scale_modifier = GYRO_SCALE_MODIFIER_2000DEG;
    else
        gyro_scale_modifier = GYRO_SCALE_MODIFIER_250DEG;

    *x = qRound((_x - calibration[1][0]) * 1000.0 / gyro_scale_modifier) / 1000.0;
    *y = qRound((_y - calibration[1][1]) * 1000.0 / gyro_scale_modifier) / 1000.0;
    *z = qRound((_z - calibration[1][2]) * 1000.0 / gyro_scale_modifier) / 1000.0;
}

int16_t QMPU6050::getRotationX() {
    int16_t buffer = i2c->readWord8(MPU6050_RA_GYRO_XOUT_H);
    return buffer;
}

int16_t QMPU6050::getRotationY() {
    int16_t buffer = i2c->readWord8(MPU6050_RA_GYRO_YOUT_H);
    return buffer;
}

int16_t QMPU6050::getRotationZ() {
    int16_t buffer = i2c->readWord8(MPU6050_RA_GYRO_ZOUT_H);
    return buffer;
}

void QMPU6050::getMotion6(float* ax, float* ay, float* az, float* gx, float* gy, float* gz) {
    getAcceleration(ax,ay,az);
    getRotation(gx,gy,gz);
}

void QMPU6050::getFilteredMotion() {
    float ax, ay, az, gr, gp, gy; //Temporary storage variables used in _update()

    getRotation(&gr, &gp, &gy); //Get the data from the sensors
    getAcceleration(&ax, &ay, &az);

    //X (roll) axis
    accel_angle[0] = atan2(az, ay) * RAD_T_DEG - 90.0; //Calculate the angle with z and y convert to degrees and subtract 90 degrees to rotate
    gyro_angle[0] = angle[0] + gr*dt; //Use roll axis (X axis)

    //Y (pitch) axis
    accel_angle[1] = atan2(az, ax) * RAD_T_DEG - 90.0; //Calculate the angle with z and x convert to degrees and subtract 90 degrees to rotate
    gyro_angle[1] = angle[1] + gp*dt; //Use pitch axis (Y axis)

    //Z (yaw) axis
    if (calc_yaw)
        gyro_angle[2] = angle[2] + gy*dt; //Use yaw axis (Z axis)


    if (first_run) { //Set the gyroscope angle reference point if this is the first function run
        for (int i = 0; i <= 1; i++)
            gyro_angle[i] = accel_angle[i]; //Start off with angle from accelerometer (absolute angle since gyroscope is relative)

        gyro_angle[2] = 0; //Set the yaw axis to zero (because the angle cannot be calculated with the accelerometer when vertical)
        first_run = false;
    }

    float asum = qAbs(ax) + qAbs(ay) + qAbs(az); //Calculate the sum of the accelerations
    float gsum = qAbs(gr) + qAbs(gp) + qAbs(gy); //Calculate the sum of the gyro readings

    for (int i = 0; i <= 1; i++) { //Loop through roll and pitch axes
        if (qAbs(gyro_angle[i] - accel_angle[i]) > 5)  //Correct for very large drift (or incorrect measurment of gyroscope by longer loop time)
        gyro_angle[i] = accel_angle[i];


        //Create result from either complementary filter or directly from gyroscope or accelerometer depending on conditions
        if (asum > 0.1 && asum < 3 && gsum > 0.3)  //Check that th movement is not very high (therefore providing inacurate angles)
            angle[i] = (1 - TAU)*(gyro_angle[i]) + (TAU)*(accel_angle[i]); //Calculate the angle using a complementary filter
        else if (gsum > 0.3)  //Use the gyroscope angle if the acceleration is high
            angle[i] = gyro_angle[i];
        else if (gsum <= 0.3)  //Use accelerometer angle if not much movement
            angle[i] = accel_angle[i];
    }

    //The yaw axis will not work with the accelerometer angle, so only use gyroscope angle
    if (calc_yaw) //Only calculate the angle when we want it to prevent large drift
        angle[2] = gyro_angle[2];
    else {
        angle[2] = 0;
        gyro_angle[2] = 0;
    }

    clock_gettime(CLOCK_REALTIME, &end); //Save time to end clock
    dt = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9; //Calculate new dt
    clock_gettime(CLOCK_REALTIME, &start); //Save time to start clock
}

float QMPU6050::getYaw(){
    calc_yaw=true;
    getFilteredMotion();
    calc_yaw=false;
    return angle[2];
}

void QMPU6050::calibrate() {
    float ax_off = 0.0, ay_off = 0.0, az_off = 0.0;
    float gr_off = 0.0, gp_off = 0.0, gy_off = 0.0;

    float gyro_off[3]; //Temporary storage
    float accel_off[3];

    for (int i = 0; i < 10000; i++) { //Use loop to average offsets
        qDebug()<<"Remaining "<<10000-i<<" iterations!";
        getRotationRaw(&gyro_off[0], &gyro_off[1], &gyro_off[2]); //Raw gyroscope values
        gr_off = gr_off + gyro_off[0], gp_off = gp_off + gyro_off[1], gy_off = gy_off + gyro_off[2]; //Add to sum

        getAccelerationRaw(&accel_off[0], &accel_off[1], &accel_off[2]); //Raw accelerometer values
        ax_off = ax_off + accel_off[0], ay_off = ay_off + accel_off[1], az_off = az_off + accel_off[2]; //Add to sum

        qDebug()<< "Calibrating.. Keep your sensor still on a flat surface";
    }


    gr_off = gr_off / 10000, gp_off = gp_off / 10000, gy_off = gy_off / 10000; //Divide by number of loops (to average)
    ax_off = ax_off / 10000, ay_off = ay_off / 10000, az_off = az_off / 10000;

    az_off = az_off - ACCEL_SCALE_MODIFIER_2G; //Remove 1g from the value calculated to compensate for gravity)

    qDebug()<< "Calibration finished, copy and paste the following line into your code..";
    QString output = QString("setCalibrationOffsets(%1, %2, %3, %4, %5, %6);").arg(ax_off).arg(ay_off).arg(az_off).arg(gr_off).arg(gp_off).arg(gy_off);
    qDebug()<<output;
}

void QMPU6050::setCalibrationOffsets(const float &c1, const float &c2, const float &c3,
                                    const float &c4, const float &c5, const float &c6) {
    QVector<float> vec;

    vec.push_back(c1);
    vec.push_back(c2);
    vec.push_back(c3);

    calibration.push_back(vec);
    vec.clear();

    vec.push_back(c4);
    vec.push_back(c5);
    vec.push_back(c6);

    calibration.push_back(vec);
}

void QMPU6050::resetCalibrationOffsets() {
    for(int i=0;i<2;i++){
        QVector<float> vec;
        for(int j=0;j<3;j++)
            vec.push_back(0.0);
        calibration.push_back(vec);
    }
}
