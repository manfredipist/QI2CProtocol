#include "qqmc5883l.h"

QQMC5883L::QQMC5883L(const uint8_t i2c_bus, const uint8_t i2c_address, QObject *parent) : QObject(parent), i2c_bus(i2c_bus), i2c_address(i2c_address)
{
    i2c = new QI2C(i2c_bus,i2c_address,this);
}

QQMC5883L::~QQMC5883L(){
    i2c->i2cClose();
}

void QQMC5883L::initialize() {
    softReset();
    disableInterrupt();
    defineSetResetPeriod();

    setMode(1);
    setOversampling(512);
    setRange(2);
    setSamplingRate(10);
    setConfiguration();

    //calibrate();
    //setCalibrationOffsets(-1012, 5245, -7887, 0, 0, 2592);
    setCalibrationOffsets(-3195, 2430, -6775, 0, 0, 7590);

    if(i2c->readByte8(QMC5883L_CHIP_ID) == 0xFF)
        qDebug()<<"QMC5883L Initialization: SUCCESS";
    else
        qDebug()<<"QMC5883L Initialization: FAILURE";
}

void QQMC5883L::softReset()
{
    i2c->writeByte8(QMC5883L_CONFIG2, QMC5883L_CONFIG2_SRST);
}

void QQMC5883L::disableInterrupt(){
    i2c->writeByte8(QMC5883L_CONFIG2, QMC5883L_CONFIG2_IENB);
}

void QQMC5883L::defineSetResetPeriod()
{
    i2c->writeByte8(QMC5883L_RESET, 0x01);
}

void QQMC5883L::setConfiguration()
{
    i2c->writeByte8(QMC5883L_CONFIG, mode|oversampling|range|rate);
}

void QQMC5883L::setMode(const int &x){
    switch(x) {
        case 0:
            mode = QMC5883L_CONFIG_STANDBY;
        break;
        case 1:
            mode = QMC5883L_CONFIG_CONT;
        break;
    }
}

void QQMC5883L::setOversampling(const int &x)
{
    switch(x) {
        case 512:
            oversampling = QMC5883L_CONFIG_OS512;
            break;
        case 256:
            oversampling = QMC5883L_CONFIG_OS256;
            break;
        case 128:
            oversampling = QMC5883L_CONFIG_OS128;
            break;
        case 64:
            oversampling = QMC5883L_CONFIG_OS64;
            break;
    }
}

void QQMC5883L::setRange(const int &x)
{
    switch(x) {
        case 2:
            range = QMC5883L_CONFIG_2GAUSS;
        break;
        case 8:
            range = QMC5883L_CONFIG_8GAUSS;
        break;
    }
}

void QQMC5883L::setSamplingRate(const int &x)
{
    switch(x) {
        case 10:
            rate = QMC5883L_CONFIG_10HZ;
            break;
        case 50:
            rate = QMC5883L_CONFIG_50HZ;
            break;
        case 100:
            rate = QMC5883L_CONFIG_100HZ;
            break;
        case 200:
            rate = QMC5883L_CONFIG_200HZ;
            break;
    }
}

int QQMC5883L::readRaw(int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t data[6];
    i2c->readBytes8(QMC5883L_X_LSB, 6, data);

    *x = (data[0] | (data[1] << 8));
    *y = (data[2] | (data[3] << 8));
    *z = (data[4] | (data[5] << 8));

    if(*x == 0 && *y == 0 && *z == 0)
        return 0;

    return 1;
}

void QQMC5883L::calibrate(){
    int16_t x, y, z;
    int calibrationData[3][2] ={{0}};

    for(int i=0;i<10000;i++){
        qDebug()<<"Remaining "<<10000-i<<" iterations!";
        readRaw(&x,&y,&z);

        if(x < calibrationData[0][0])
          calibrationData[0][0] = x;

        if(x > calibrationData[0][1])
          calibrationData[0][1] = x;

        if(y < calibrationData[1][0])
          calibrationData[1][0] = y;

        if(y > calibrationData[1][1])
          calibrationData[1][1] = y;

        if(z < calibrationData[2][0])
          calibrationData[2][0] = z;

        if(z > calibrationData[2][1])
          calibrationData[2][1] = z;

        qDebug()<< "Calibrating.. Keep moving your sensor on a flat surface";
    }
    qDebug()<< "Calibration finished, copy and paste the following line into your code..";
    QString output = QString("setCalibrationOffsets(%1, %2, %3, %4, %5, %6);").arg(calibrationData[0][0]).arg(calibrationData[0][1]).arg(calibrationData[1][0]).arg(calibrationData[1][1]).arg(calibrationData[2][0]).arg(calibrationData[2][1]);
    qDebug()<<output;
}

void QQMC5883L::setCalibrationOffsets(const int &c1, const int &c2, const int &c3,
                                    const int &c4, const int &c5, const int &c6) {
    QVector<int> vec;

    vec.push_back(c1);
    vec.push_back(c2);

    calibration.push_back(vec);
    vec.clear();

    vec.push_back(c3);
    vec.push_back(c4);

    calibration.push_back(vec);
    vec.clear();

    vec.push_back(c5);
    vec.push_back(c6);

    calibration.push_back(vec);
}

void QQMC5883L::resetCalibrationOffsets() {
    for(int i=0;i<3;i++){
        QVector<int> vec;
        for(int j=0;j<2;j++)
            vec.push_back(0.0);
        calibration.push_back(vec);
    }
}

void QQMC5883L::setDeclination(const float &declination) {
    this->declination = declination;
}

void QQMC5883L::resetDeclination() {
    declination = 0.0;
}


bool QQMC5883L::isDataReady()
{
    uint8_t status = i2c->readByte8(QMC5883L_STATUS);
    return status & QMC5883L_STATUS_DRDY;
}

int QQMC5883L::readHeading()
{
    int16_t x, y, z;

    if(!readRaw(&x, &y, &z))
        return 0;

    float calibrated_x = x * calibration[0][0] + y * calibration[0][1] + calibration[0][2];
    float calibrated_y = x * calibration[1][0] + y * calibration[1][1] + calibration[1][2];

    float heading = 180.0*atan2(calibrated_x,calibrated_y)/M_PI;

    if(heading<0)
        heading += 360;

    heading+=declination;

    if(heading<0.0)
        heading += 360.0;
    else if(heading>=360.0)
        heading -= 360.0;

    return heading;
}

int QQMC5883L::readCalibratedHeading()
{
    int16_t x, y, z;

    if(!readRaw(&x, &y, &z))
        return 0;

    int x_offset = (calibration[0][0] + calibration[0][1])/2;
    int y_offset = (calibration[1][0] + calibration[1][1])/2;
    int z_offset = (calibration[2][0] + calibration[2][1])/2;
    int x_avg_delta = (calibration[0][1] - calibration[0][0])/2;
    int y_avg_delta = (calibration[1][1] - calibration[1][0])/2;
    int z_avg_delta = (calibration[2][1] - calibration[2][0])/2;

    int avg_delta = (x_avg_delta + y_avg_delta + z_avg_delta) / 3;

    float x_scale = (float)avg_delta / x_avg_delta;
    float y_scale = (float)avg_delta / y_avg_delta;
    float z_scale = (float)avg_delta / z_avg_delta;

    int calibrated_x = (x - x_offset) * x_scale;
    int calibrated_y = (y - y_offset) * y_scale;
    int calibrated_z = (z - z_offset) * z_scale;

    int heading = 180*atan2(calibrated_x,calibrated_y)/M_PI;

    if(heading<0)
        heading += 360;

    return heading;
}
