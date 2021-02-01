#include "pivl53l1x.h"

PiVL53L1X::PiVL53L1X(uint8_t i2c_bus, uint8_t i2c_address, QObject *parent) : QObject(parent), i2c_bus(i2c_bus), i2c_address(i2c_address)
{
    i2c = new QI2C(i2c_bus,i2c_address,this);
}

PiVL53L1X::~PiVL53L1X(){
    i2c->i2cClose();
}

void PiVL53L1X::initialize(){
    sensorInit();

    setDistanceMode(VL53L1X_DISTANCE_MODE_LONG);

    //qDebug()<< "DistanceMode: "<< getDistanceMode();
    //qDebug()<<"TimingBudgetInMs :" << getTimingBudgetInMs();
    //qDebug()<<"InterMeasurementInMs :" << getInterMeasurementInMs();

    if(bootState() == 3)
        qDebug()<<"VL53L1X Initialization: SUCCESS";
    else
        qDebug()<<"VL53L1X Initialization: FAILURE";

    startRanging();
}

void PiVL53L1X::sensorInit() {
    for (uint8_t reg = 0x2D; reg <= 0x87; reg++)
       i2c->writeByte16(reg, VL51L1X_DEFAULT_CONFIGURATION[reg - 0x2D]);

    startRanging();
    while(!isDataReady())
        usleep(10);
    clearInterrupt();
    stopRanging();

    i2c->writeByte16(VL53L1_VHV_CONFIG__TIMEOUT_MACROP_LOOP_BOUND, 0x09); /* two bounds VHV */
    i2c->writeByte16(0x0B, 0); /* start VHV from the previous temperature */
}

uint16_t PiVL53L1X::getSensorId() {
    return i2c->readWord16(VL53L1_IDENTIFICATION__MODEL_ID);
}

bool PiVL53L1X::startRanging() {
    return i2c->writeByte16(SYSTEM__MODE_START, 0x40);	/* Enable VL53L1X */
}

uint8_t PiVL53L1X::getInterruptPolarity() {
    uint8_t temp = i2c->readByte16(GPIO_HV_MUX__CTRL);
    temp = temp & 0x10;
    return (temp>>4) ? 0 : 1;
}

bool PiVL53L1X::isDataReady() {
    uint8_t intPol = getInterruptPolarity();
    uint8_t temp = i2c->readByte16(GPIO__TIO_HV_STATUS);
    if ((temp & 1) == intPol) {
        return true;
    }
    return false;
}

bool PiVL53L1X::clearInterrupt() {
    return i2c->writeByte16(SYSTEM__INTERRUPT_CLEAR, 0x01);
}

bool PiVL53L1X::stopRanging() {
    return i2c->writeByte16(SYSTEM__MODE_START, 0x00);	/* Disable VL53L1X */
}

uint16_t PiVL53L1X::getUIntDistance() {
    return i2c->readWord16(VL53L1_RESULT__FINAL_CROSSTALK_CORRECTED_RANGE_MM_SD0);
}

double PiVL53L1X::getDistance(){
    if(isDataReady())
        return getUIntDistance()/10.0;
    else
        return -1;
}

uint8_t PiVL53L1X::bootState() {
    return i2c->readByte16(VL53L1_FIRMWARE__SYSTEM_STATUS);
}

int PiVL53L1X::getTimingBudgetInMs() {
    uint16_t temp = i2c->readWord16(RANGE_CONFIG__TIMEOUT_MACROP_A_HI);
    int timingBudget;
    switch (temp) {
        case 0x001D :
            timingBudget = 15;
            break;
        case 0x0051 :
        case 0x001E :
            timingBudget = 20;
            break;
        case 0x00D6 :
        case 0x0060 :
            timingBudget = 33;
            break;
        case 0x1AE :
        case 0x00AD :
            timingBudget = 50;
            break;
        case 0x02E1 :
        case 0x01CC :
            timingBudget = 100;
            break;
        case 0x03E1 :
        case 0x02D9 :
            timingBudget = 200;
            break;
        case 0x0591 :
        case 0x048F :
            timingBudget = 500;
            break;
        default:
            qDebug()<<"Unexpected timing budget "<<temp;
            timingBudget = 0;
    }
    return timingBudget;
}

int PiVL53L1X::getDistanceMode() {
    uint8_t tempDM = i2c->readByte16(PHASECAL_CONFIG__TIMEOUT_MACROP);
    int distanceMode = 0;
    if (tempDM == 0x14) {
        distanceMode = 1;
    } else if (tempDM == 0x0A) {
        distanceMode = 2;
    }
    return distanceMode;
}

void PiVL53L1X::setTimingBudgetInMs(const int &timingBudgetInMs) {
    const int distanceMode = getDistanceMode();
    if (distanceMode == 0) {
        qDebug()<<"Unexpected distanceMode";
    } else if (distanceMode == 1) {    /* Short DistanceMode */
        switch (timingBudgetInMs) {
            case 15: /* only available in short distance mode */
                i2c->writeWord16(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x01D);
                i2c->writeWord16(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x0027);
                break;
            case 20:
                i2c->writeWord16(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x0051);
                i2c->writeWord16(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x006E);
                break;
            case 33:
                i2c->writeWord16(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x00D6);
                i2c->writeWord16(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x006E);
                break;
            case 50:
                i2c->writeWord16(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x1AE);
                i2c->writeWord16(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x01E8);
                break;
            case 100:
                i2c->writeWord16(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x02E1);
                i2c->writeWord16(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x0388);
                break;
            case 200:
                i2c->writeWord16(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x03E1);
                i2c->writeWord16(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x0496);
                break;
            case 500:
                i2c->writeWord16(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x0591);
                i2c->writeWord16(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x05C1);
                break;
            default:
                break;
        }
    } else {
        switch (timingBudgetInMs) {
            case 20:
                i2c->writeWord16(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x001E);
                i2c->writeWord16(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x0022);
                break;
            case 33:
                i2c->writeWord16(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x0060);
                i2c->writeWord16(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x006E);
                break;
            case 50:
                i2c->writeWord16(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x00AD);
                i2c->writeWord16(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x00C6);
                break;
            case 100:
                i2c->writeWord16(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x01CC);
                i2c->writeWord16(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x01EA);
                break;
            case 200:
                i2c->writeWord16(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x02D9);
                i2c->writeWord16(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x02F8);
                break;
            case 500:
                i2c->writeWord16(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x048F);
                i2c->writeWord16(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x04A4);
                break;
            default:
                qDebug()<<"Unexpected timing budget "<<timingBudgetInMs;
                break;
        }
    }
}

void PiVL53L1X::setDistanceMode(const int &distanceMode) {
    const int timingBudget = getTimingBudgetInMs();

    if (distanceMode == 1) {
        i2c->writeByte16(PHASECAL_CONFIG__TIMEOUT_MACROP, 0x14);
        i2c->writeByte16(RANGE_CONFIG__VCSEL_PERIOD_A, 0x07);
        i2c->writeByte16(RANGE_CONFIG__VCSEL_PERIOD_B, 0x05);
        i2c->writeByte16(RANGE_CONFIG__VALID_PHASE_HIGH, 0x38);
        i2c->writeWord16(SD_CONFIG__WOI_SD0, 0x0705);
        i2c->writeWord16(SD_CONFIG__INITIAL_PHASE_SD0, 0x0606);
    } else if (distanceMode == 2) {
        i2c->writeByte16(PHASECAL_CONFIG__TIMEOUT_MACROP, 0x0A);
        i2c->writeByte16(RANGE_CONFIG__VCSEL_PERIOD_A, 0x0F);
        i2c->writeByte16(RANGE_CONFIG__VCSEL_PERIOD_B, 0x0D);
        i2c->writeByte16(RANGE_CONFIG__VALID_PHASE_HIGH, 0xB8);
        i2c->writeWord16(SD_CONFIG__WOI_SD0, 0x0F0D);
        i2c->writeWord16(SD_CONFIG__INITIAL_PHASE_SD0, 0x0E0E);
    } else
        qDebug()<<"Unexpected distanceMode "<<distanceMode;

    setTimingBudgetInMs(timingBudget);
}

uint32_t PiVL53L1X::getInterMeasurementInMs() {
    uint32_t pIM = i2c->readDoubleWord16(VL53L1_SYSTEM__INTERMEASUREMENT_PERIOD);
    uint16_t clockPLL = i2c->readWord16(VL53L1_RESULT__OSC_CALIBRATE_VAL);
    clockPLL = clockPLL & 0x03FF;
    pIM = round(pIM / (clockPLL * 1.065));
    return pIM;
}

void PiVL53L1X::setInterMeasurementInMs(const uint32_t &interMeasMs) {
    uint16_t clockPLL = i2c->readWord16(VL53L1_RESULT__OSC_CALIBRATE_VAL);
    clockPLL = clockPLL & 0x3FF;
    i2c->writeDoubleWord(VL53L1_SYSTEM__INTERMEASUREMENT_PERIOD, round(clockPLL * interMeasMs * 1.075));
}

/**
 * There are five range statuses: 0, 1, 2, 4, and 7.
 * When the range status is 0, there is no error.
 * Range status 1 and 2 are error warnings while
 * range status 4 and 7 are errors.
 *
 * When the range status is 1, there is a sigma failure. This means that the repeatability or standard
 * deviation of the measurement is bad due to a decreasing signal noise ratio. Increasing the timing budget can
 * improve the standard deviation and avoid a range status 1.
 *
 * When the range status is 2, there is a signal failure. This means that the return signal is too week to return
 * a good answer. The reason is because the target is too far, or the target is not reflective enough, or the
 * target is too small. Increasing the timing buget might help, but there may simply be no target available.
 *
 * When the range status is 4, the sensor is "out of bounds". This means that the sensor
 * is ranging in a “nonappropriated” zone and the measured result may be inconsistent. This status is considered
 * as a warning but, in general, it happens when a target is at the maximum distance possible from the sensor,
 * i.e. around 5 m. However, this is only for very bright targets.
 *
 * Range status 7 is called "wraparound". This situation may occur when the target is very reflective and
 * the distance to the target/sensor is longer than the physical limited distance measurable by the sensor.
 * Such distances include approximately 5 m when the senor is in Long distance mode and approximately 1.3 m
 * when the sensor is in Short distance mode.
 *
 * Example: a traffic sign located at 6 m can be seen by the sensor and returns a range of 1 m. This is due
 * to “radar aliasing”: if only an approximate distance is required, we may add 6 m to the distance returned.
 * However, that is a very approximate estimation.
 */

uint8_t PiVL53L1X::getRangeStatus() {
    uint8_t rangeStatus = i2c->readByte16(VL53L1_RESULT__RANGE_STATUS);
    rangeStatus = rangeStatus & 0x1F;
    if (rangeStatus < 24)
        rangeStatus = status_rtn[rangeStatus];
    return rangeStatus;
}


