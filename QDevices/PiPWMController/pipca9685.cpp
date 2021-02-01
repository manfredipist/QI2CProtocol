#include "pipca9685.h"

PiPCA9685::PiPCA9685(uint8_t i2c_bus, uint8_t i2c_address, QObject *parent) : QObject(parent), i2c_bus(i2c_bus), i2c_address(i2c_address)
{
    i2c = new QI2C(i2c_bus,i2c_address,this);
}

PiPCA9685::~PiPCA9685(){
    i2c->i2cClose();
}

void PiPCA9685::initialize(){
    i2c->writeByte8(MODE2, OUTDRV);
    i2c->writeByte8(MODE1, ALLCALL);
    //time.sleep(0.005)  # wait for oscillator
    usleep(5000);
    uint8_t mode1 = i2c->readByte8(MODE1);
    mode1 = mode1 & ~SLEEP;  // wake up (reset sleep)
    i2c->writeByte8(MODE1, mode1);
    //time.sleep(0.005)  # wait for oscillator
    usleep(5000);
    setPwmFrequency(50);
    qInfo()<<"PCA9685 Initialization: SUCCESS";
}

void PiPCA9685::setPwmFrequency(uint16_t frequency){
    float prescaleval = 25000000.0; //25MHz
    prescaleval /= 4096.0;         //12-bit
    prescaleval /= float(frequency);
    prescaleval -= 1.0;
    qInfo()<<"Setting PWM frequency to "<<frequency<<" hz";
    qInfo()<<"Estimated pre-scale: "<<prescaleval;
    int prescale = (int)floor(prescaleval + 0.5);
    qInfo()<<"Final pre-scale: "<<prescale;
    uint8_t oldmode = i2c->readByte8(MODE1);
    uint8_t newmode = (oldmode & 0x7F) | 0x10; // sleep
    i2c->writeByte8(MODE1, newmode);      //go to sleep
    i2c->writeByte8(PRESCALE, prescale);
    i2c->writeByte8(MODE1, oldmode);
    //time.sleep(0.005)  # wait
    usleep(5000);
    i2c->writeByte8(MODE1, oldmode | 0x80);
}

void PiPCA9685::setPwm(uint8_t channel, uint16_t on, uint16_t off){
    i2c->writeByte8(LED0_ON_L+4*channel, on & 0xFF);
    i2c->writeByte8(LED0_ON_H+4*channel, on >> 8);
    i2c->writeByte8(LED0_OFF_L+4*channel, off & 0xFF);
    i2c->writeByte8(LED0_OFF_H+4*channel, off >> 8);
}

void PiPCA9685::setAllPwm(uint16_t on, uint16_t off){
    i2c->writeByte8(ALL_LED_ON_L, on & 0xFF);
    i2c->writeByte8(ALL_LED_ON_H, on >> 8);
    i2c->writeByte8(ALL_LED_OFF_L, off & 0xFF);
    i2c->writeByte8(ALL_LED_OFF_H, off >> 8);
}

void PiPCA9685::move(uint8_t channel, int deg){
    float pwm = 570.0 + ((deg/180.0) * 1700.0);
    pwm = (4096.0/20000.0) * pwm;
    setPwm(channel, 0, (int) pwm);
}
