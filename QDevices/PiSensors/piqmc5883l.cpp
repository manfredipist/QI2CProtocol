#include "piqmc5883l.h"

PiQMC5883L::PiQMC5883L(const uint8_t i2c_bus, const uint8_t i2c_address, QObject *parent) : QObject(parent), i2c_bus(i2c_bus), i2c_address(i2c_address)
{
    i2c = new QI2C(i2c_bus,i2c_address,this);
}

PiQMC5883L::~PiQMC5883L(){
    i2c->i2cClose();
}

void PiQMC5883L::initialize() {
    softReset();
    disableInterrupt();
    defineSetResetPeriod();

    setMode(1);
    setOversampling(512);
    setRange(2);
    setSamplingRate(10);
    setConfiguration();

    //calibrate();
    setCalibrationOffsets(-2817, 4392, -2430, 9775, -4200, 5997);
    //setCalibrationOffsets(1.0000912557105424, 0.003915423209277943, 418.0882138871126, 0.003915423209277943, 1.1679953924705968, -923.1265637992069, 0.0, 0.0, 1.0);

    if(i2c->readByte8(QMC5883L_CHIP_ID) == 0xFF)
        qDebug()<<"QMC5883L Initialization: SUCCESS";
    else
        qDebug()<<"QMC5883L Initialization: FAILURE";
}

void PiQMC5883L::softReset()
{
    i2c->writeByte8(QMC5883L_CONFIG2, QMC5883L_CONFIG2_SRST);
}

void PiQMC5883L::disableInterrupt(){
    i2c->writeByte8(QMC5883L_CONFIG2, QMC5883L_CONFIG2_IENB);
}

void PiQMC5883L::defineSetResetPeriod()
{
    i2c->writeByte8(QMC5883L_RESET, 0x01);
}

void PiQMC5883L::setConfiguration()
{
    i2c->writeByte8(QMC5883L_CONFIG, mode|oversampling|range|rate);
}

void PiQMC5883L::setMode(const int &x){
    switch(x) {
        case 0:
            mode = QMC5883L_CONFIG_STANDBY;
        break;
        case 1:
            mode = QMC5883L_CONFIG_CONT;
        break;
    }
}

void PiQMC5883L::setOversampling(const int &x)
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

void PiQMC5883L::setRange(const int &x)
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

void PiQMC5883L::setSamplingRate(const int &x)
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

bool PiQMC5883L::readRaw(int16_t *x, int16_t *y, int16_t *z)
{
    if(!isDataReady())
        return false;

    uint8_t data[6];
    i2c->readBytes8(QMC5883L_X_LSB, 6, data);
    *x = (data[0] | (data[1] << 8));
    *y = (data[2] | (data[3] << 8));
    *z = (data[4] | (data[5] << 8));

    return true;
}

void PiQMC5883L::calibrate(){
    int16_t x, y, z;
    int calibrationData[3][2] ={{0}};

    for(int i=0;i<10000;i++){
        qDebug()<<"Remaining "<<10000-i<<" iterations!";
        if(!readRaw(&x,&y,&z)){
            i--;
            continue;
        }
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

void PiQMC5883L::setCalibrationOffsets(const int &c1, const int &c2, const int &c3,
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

void PiQMC5883L::setCalibrationOffsets(const float &c1, const float &c2, const float &c3,
                                           const float &c4, const float &c5, const float &c6,
                                           const float &c7, const float &c8, const float &c9) {
    QVector<float> vec;

    vec.push_back(c1);
    vec.push_back(c2);
    vec.push_back(c3);

    calibration2.push_back(vec);
    vec.clear();

    vec.push_back(c4);
    vec.push_back(c5);
    vec.push_back(c6);

    calibration2.push_back(vec);
    vec.clear();

    vec.push_back(c7);
    vec.push_back(c8);
    vec.push_back(c9);

    calibration2.push_back(vec);
}

void PiQMC5883L::resetCalibrationOffsets() {
    for(int i=0;i<3;i++){
        QVector<int> vec;
        for(int j=0;j<3;j++)
            vec.push_back(0.0);
        calibration.push_back(vec);
    }
}

void PiQMC5883L::setDeclination(const float &declination) {
    this->declination = declination;
}

void PiQMC5883L::resetDeclination() {
    declination = 0.0;
}


bool PiQMC5883L::isDataReady()
{
    uint8_t status = i2c->readByte8(QMC5883L_STATUS);
    return status & QMC5883L_STATUS_DRDY;
}

int PiQMC5883L::readCalibratedHeading()
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

    int heading = 180*atan2(calibrated_y,calibrated_x)/M_PI;

    if(heading<0)
        heading += 360;

    return heading;
}


int PiQMC5883L::readCalibratedHeading2()
{
    int16_t x, y, z;
    if(!readRaw(&x, &y, &z))
        return -1;

    float calibrated_x = x * calibration2[0][0] + y * calibration2[0][1] + calibration2[0][2];
    float calibrated_y = x * calibration2[1][0] + y * calibration2[1][1] + calibration2[1][2];

    float heading = 180.0*atan2(calibrated_y,calibrated_x)/M_PI;

    if(heading<0)
        heading += 360;

    heading+=declination;

    if(heading<0.0)
        heading += 360.0;
    else if(heading>=360.0)
        heading -= 360.0;

    return heading;
}

int PiQMC5883L::readCalibratedHeading3(){
    /* Update the observed boundaries of the measurements */
    int16_t x, y, z;
    if(!readRaw(&x, &y, &z))
        return -1;

    if(x<xlow) xlow = x;
    if(x>xhigh) xhigh = x;
    if(y<ylow) ylow = y;
    if(y>yhigh) yhigh = y;

    /* Bail out if not enough data is available. */

    if( xlow==xhigh || ylow==yhigh ) return 0;

    /* Recenter the measurement by subtracting the average */

    x -= (xhigh+xlow)/2;
    y -= (yhigh+ylow)/2;

    /* Rescale the measurement to the range observed. */

    float fx = (float)x/(xhigh-xlow);
    float fy = (float)y/(yhigh-ylow);

    int heading = 180.0*atan2(fy,fx)/M_PI;
    if(heading<=0) heading += 360;

    return heading;
}
