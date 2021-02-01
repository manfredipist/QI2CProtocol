#include "pitof10120.h"

PiTOF10120::PiTOF10120(uint8_t i2c_bus, uint8_t i2c_address, QObject *parent) : QObject(parent), i2c_bus(i2c_bus), i2c_address(i2c_address)
{
    i2c = new QI2C(i2c_bus,i2c_address,this);
}

PiTOF10120::~PiTOF10120(){
    i2c->i2cClose();
}

void PiTOF10120::initialize(){
    qInfo()<<"TOF10120 Initialization: SUCCESS";
}

uint16_t PiTOF10120::getUIntDistance(){
    uint8_t buffer[2];
    i2c->readBytes8(0x00,2,buffer);
    uint16_t result = buffer[0] << 8 | buffer[1];
    return result;
}

double PiTOF10120::getDistance(){
    return (double) getUIntDistance()/10.0;
}
