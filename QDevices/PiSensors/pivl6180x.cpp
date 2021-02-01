#include "pivl6180x.h"

PiVL6180X::PiVL6180X(uint8_t i2c_bus, uint8_t i2c_address, QObject *parent) : QObject(parent), i2c_bus(i2c_bus), i2c_address(i2c_address)
{
    i2c = new QI2C(i2c_bus,i2c_address,this);
}

PiVL6180X::~PiVL6180X(){
    i2c->i2cClose();
}

void PiVL6180X::initialize(){
    if(i2c->readByte8(VL6180X_REG_IDENTIFICATION_MODEL_ID) != 0xB4)
        qDebug()<<"VL6180X Initialization: FAILURE";
    else{
        loadDefaultConfiguration();
        i2c->writeByte8(VL6180X_REG_SYSTEM_FRESH_OUT_OF_RESET, 0x00);
    }

}

double PiVL6180X::getDistance()
{
    return getRangeMeasurement()/10.0;
}

uint8_t PiVL6180X::getRangeMeasurement(){

    uint8_t range = 0x00;
    // Read the range of an object in front of sensor and return it in mm
    //wait for device to be ready for range measurement
    while(!(i2c->readByte8(VL6180X_REG_RESULT_RANGE_STATUS) & 0x01));

    //Start a range measurement
    i2c->writeByte16(VL6180X_REG_SYSRANGE_START, 0x01);

    //Poll until bit 2 is set
    while (!(i2c->readByte8(VL6180X_REG_RESULT_INTERRUPT_STATUS_GPIO) & 0x04))

    //read range in mm
    range = i2c->readByte8(VL6180X_REG_RESULT_RANGE_VAL);
    //clear interrupt
    i2c->writeByte16(VL6180X_REG_SYSTEM_INTERRUPT_CLEAR, 0x07);

    return range;
}

uint16_t PiVL6180X::getLuxMeasurement(uint8_t gain){
    /* Read the lux (light value) from the sensor and return it.Must specify the gain value to use for the lux reading:
    - ALS_GAIN_1 = 1x
    - ALS_GAIN_1_25 = 1.25x
    - ALS_GAIN_1_67 = 1.67x
    - ALS_GAIN_2_5 = 2.5x
    - ALS_GAIN_5 = 5x
    - ALS_GAIN_10 = 10x
    - ALS_GAIN_20 = 20x
    - ALS_GAIN_40 = 40x
    */

    uint16_t lux;
    uint8_t reg = i2c->readByte8(VL6180X_REG_SYSTEM_INTERRUPT_CONFIG);
    reg &= ~0x38;
    reg |= 0x4 << 3;  // IRQ on ALS ready
    i2c->writeByte16(VL6180X_REG_SYSTEM_INTERRUPT_CONFIG, reg);

    //100 ms integration period
    i2c->writeByte16(VL6180X_REG_SYSALS_INTEGRATION_PERIOD_HI, 0);
    i2c->writeByte16(VL6180X_REG_SYSALS_INTEGRATION_PERIOD_LO, 100);

    //analog gain
    if (gain > ALS_GAIN_40)
        gain = ALS_GAIN_40;

    i2c->writeByte16(VL6180X_REG_SYSALS_ANALOGUE_GAIN, 0x40 | gain);

    //start ALS
    i2c->writeByte16(VL6180X_REG_SYSALS_START, 0x1);

    //Poll until "New Sample Ready threshold event" is set
    while (((i2c->readByte8(VL6180X_REG_RESULT_INTERRUPT_STATUS_GPIO) >> 3) & 0x7) != 4);

    //read lux!
    lux = i2c->readByte16(VL6180X_REG_RESULT_ALS_VAL);

    //clear interrupt
    i2c->writeByte16(VL6180X_REG_SYSTEM_INTERRUPT_CLEAR, 0x07);
    lux *= 0.32; // calibrated count/lux
    if (gain == ALS_GAIN_1);
    else if (gain == ALS_GAIN_1_25)
        lux /= 1.25;
    else if (gain == ALS_GAIN_1_67)
        lux /= 1.76;
    else if (gain == ALS_GAIN_2_5)
        lux /= 2.5;
    else if (gain == ALS_GAIN_5)
        lux /= 5;
    else if (gain == ALS_GAIN_10)
        lux /= 10;
    else if (gain == ALS_GAIN_20)
        lux /= 20;
    else if (gain == ALS_GAIN_40)
        lux /= 20;

    lux *= 100;
    lux /= 100;  //integration time in ms

    return lux;
}

int PiVL6180X::rangeStatus() {
    /* Retrieve the status/error from a previous range read.  This will return a constant value such as:
    - ERROR_NONE - No error
    - ERROR_SYSERR_1 - System error 1 (see datasheet)
    - ERROR_SYSERR_5 - System error 5 (see datasheet)
    - ERROR_ECEFAIL - ECE failure
    - ERROR_NOCONVERGE - No convergence
    - ERROR_RANGEIGNORE - Outside range ignored
    - ERROR_SNR - Too much noise
    - ERROR_RAWUFLOW - Raw value underflow
    - ERROR_RAWOFLOW - Raw value overflow
    - ERROR_RANGEUFLOW - Range underflow
    - ERROR_RANGEOFLOW - Range overflow
    */
    return i2c->readByte8(VL6180X_REG_RESULT_RANGE_STATUS) >> 4;
}

void PiVL6180X::loadDefaultConfiguration() {
    i2c->writeByte16(0x0207, 0x01);
    i2c->writeByte16(0x0208, 0x01);
    i2c->writeByte16(0x0096, 0x00);
    i2c->writeByte16(0x0097, 0xFD);
    i2c->writeByte16(0x00E3, 0x00);
    i2c->writeByte16(0x00E4, 0x04);
    i2c->writeByte16(0x00E5, 0x02);
    i2c->writeByte16(0x00E6, 0x01);
    i2c->writeByte16(0x00E7, 0x03);
    i2c->writeByte16(0x00F5, 0x02);
    i2c->writeByte16(0x00D9, 0x05);
    i2c->writeByte16(0x00DB, 0xCE);
    i2c->writeByte16(0x00DC, 0x03);
    i2c->writeByte16(0x00DD, 0xF8);
    i2c->writeByte16(0x009F, 0x00);
    i2c->writeByte16(0x00A3, 0x3C);
    i2c->writeByte16(0x00B7, 0x00);
    i2c->writeByte16(0x00BB, 0x3C);
    i2c->writeByte16(0x00B2, 0x09);
    i2c->writeByte16(0x00CA, 0x09);
    i2c->writeByte16(0x0198, 0x01);
    i2c->writeByte16(0x01B0, 0x17);
    i2c->writeByte16(0x01AD, 0x00);
    i2c->writeByte16(0x00FF, 0x05);
    i2c->writeByte16(0x0100, 0x05);
    i2c->writeByte16(0x0199, 0x05);
    i2c->writeByte16(0x01A6, 0x1B);
    i2c->writeByte16(0x01AC, 0x3E);
    i2c->writeByte16(0x01A7, 0x1F);
    i2c->writeByte16(0x0030, 0x00);
    //Recommended : Public registers - See data sheet for more detail
    i2c->writeByte16(0x0011, 0x10);  // Enables polling for 'New Sample ready' when measurement completes
    i2c->writeByte16(0x010A, 0x30);  // Set the averaging sample period (compromise between lower noise and increased execution time)
    i2c->writeByte16(0x003F, 0x46);  // Sets the light and dark gain (uppermnibble). Dark gain should not be changed.
    i2c->writeByte16(0x0031, 0xFF);  // sets the # of range measurements after which auto calibration of system is performed
    i2c->writeByte16(0x0040, 0x63);  // Set ALS integration time to 100ms
    i2c->writeByte16(0x002E, 0x01);  // perform a single temperature calibration of the ranging sensor

    // Optional: Public registers - See data sheet for more detail
    i2c->writeByte16(0x001B, 0x09);  //Set default ranging inter-measurement period to 100ms
    i2c->writeByte16(0x003E, 0x31);  // Set default ALS inter-measurement period to 500ms
    i2c->writeByte16(0x0014, 0x24);  // Configures interrupt on 'New Sample Ready threshold event'
}
