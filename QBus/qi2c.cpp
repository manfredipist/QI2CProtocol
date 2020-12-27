#include "qi2c.h"

QI2C::QI2C(const uint8_t &i2c_bus, const uint8_t &i2c_address, QObject *parent) : QObject(parent), i2c_bus(i2c_bus), i2c_address(i2c_address)
{
    i2cInit();
}

QI2C::~QI2C(){
    i2cClose();
}

void QI2C::i2cInit()
{
    char filename[32];
    sprintf(filename,"/dev/i2c-%d",i2c_bus);
    if ((fd = open(filename, O_RDWR)) < 0){
        qDebug()<<"Couldn't open device";
        fd = 0;
    }
    if (ioctl(fd, I2C_SLAVE, i2c_address) < 0)
    {
        qDebug()<<"Couldn't recognize driver";
        fd = 0;
    }
}

void QI2C::i2cClose()
{
    close(fd);
}

uint8_t QI2C::readBit8(uint8_t registerAddress, uint8_t bitNumber)
{
    uint8_t buffer[1] = {0};
    buffer[0] = registerAddress & 0x00FF;
    write(fd, buffer, 1);

    uint8_t result_buffer[1] = {0};
    if (read(fd, result_buffer, 1) != 1){
        qDebug()<<"I2C Transaction failed!";
        return 0;
    }
    return result_buffer[0] & (1 << bitNumber);
}

uint8_t QI2C::readBit16(uint16_t registerAddress, uint8_t bitNumber)
{
    uint8_t buffer[2] = {0};
    buffer[0] = registerAddress >> 8;
    buffer[1] = registerAddress & 0x00FF;
    write(fd, buffer, 2);

    uint8_t result_buffer[1] = {0};
    if (read(fd, result_buffer, 1) != 1){
        qDebug()<<"I2C Transaction failed!";
        return 0;
    }
    return result_buffer[0] & (1 << bitNumber);
}

uint8_t QI2C::readBits8(uint8_t registerAddress, uint8_t bitStart, uint8_t length) {
    // 01101001 read byte
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    //    010   masked
    //   -> 010 shifted

    uint8_t buffer[1] = {0};
    buffer[0] = registerAddress & 0x00FF;
    write(fd, buffer, 1);

    uint8_t result_buffer[1] = {0};
    if (read(fd, result_buffer, 1) != 1){
        qDebug()<<"I2C Transaction failed!";
        return 0;
    }

    uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
    result_buffer[0] &= mask;
    result_buffer[0] >>= (bitStart - length + 1);

    return result_buffer[0];
}

uint8_t QI2C::readBits16(uint16_t registerAddress, uint8_t bitStart, uint8_t length) {
    // 01101001 read byte
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    //    010   masked
    //   -> 010 shifted

    uint8_t buffer[2] = {0};
    buffer[0] = registerAddress >> 8;
    buffer[1] = registerAddress & 0x00FF;
    write(fd, buffer, 2);

    uint8_t result_buffer[1] = {0};
    if (read(fd, result_buffer, 1) != 1){
        qDebug()<<"I2C Transaction failed!";
        return 0;
    }

    uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
    result_buffer[0] &= mask;
    result_buffer[0] >>= (bitStart - length + 1);

    return result_buffer[0];
}

uint8_t QI2C::readByte8(uint8_t registerAddress)
{
    uint8_t buffer[1] = {0};
    buffer[0] = registerAddress & 0x00FF;
    write(fd, buffer, 1);

    uint8_t result_buffer[1] = {0};
    if (read(fd, result_buffer, 1) != 1){
        qDebug()<<"I2C Transaction failed!";
        return 0;
    }

    return result_buffer[0];
}

uint8_t QI2C::readByte16(uint16_t registerAddress)
{
    uint8_t buffer[2] = {0};
    buffer[0] = registerAddress >> 8;
    buffer[1] = registerAddress & 0x00FF;
    write(fd, buffer, 2);

    uint8_t result_buffer[1] = {0};
    if (read(fd, result_buffer, 1) != 1){
        qDebug()<<"I2C Transaction failed!";
        return 0;
    }

    return result_buffer[0];
}

void QI2C::readBytes8(uint8_t registerAddress, uint8_t length, uint8_t *data){
    uint8_t buffer[1] = {0};
    buffer[0] = registerAddress & 0x00FF;
    write(fd, buffer, 1);

    if (read(fd, data, length) != length){
        qDebug()<<"I2C Transaction failed!";
        return;
    }
}

void QI2C::readBytes16(uint16_t registerAddress, uint8_t length, uint8_t *data){
    uint8_t buffer[2] = {0};
    buffer[0] = registerAddress >> 8;
    buffer[1] = registerAddress & 0x00FF;
    write(fd, buffer, 2);

    if (read(fd, data, length) != length){
        qDebug()<<"I2C Transaction failed!";
        return;
    }
}

uint16_t QI2C::readWord8(uint8_t registerAddress)
{
    uint8_t buffer[1] = {0};
    buffer[0] = registerAddress & 0x00FF;
    write(fd, buffer, 1);

    uint16_t result;
    uint8_t result_buffer[2] = {0};
    if (read(fd, result_buffer, 2) != 2){
        qDebug()<<"I2C Transaction failed!";
        return (0);
    }
    else
       result = result_buffer[0] << 8 | result_buffer[1];

   return result;
}

uint16_t QI2C::readWord16(uint16_t registerAddress)
{
    uint8_t buffer[2] = {0};
    buffer[0] = registerAddress >> 8;
    buffer[1] = registerAddress & 0x00FF;
    write(fd, buffer, 2);

    uint16_t result;
    uint8_t result_buffer[2] = {0};
    if (read(fd, result_buffer, 2) != 2){
        qDebug()<<"I2C Transaction failed!";
        return (0);
    }
    else
       result = result_buffer[0] << 8 | result_buffer[1];

   return result;
}

uint16_t QI2C::readWord8_2c(uint8_t registerAddress)
{
    uint16_t word = readWord16(registerAddress);
    if (word >= 0x8000)         // 32768
        return word - 0x10000;  // 65536
    else
       return word;
}

uint16_t QI2C::readWord16_2c(uint16_t registerAddress)
{
    uint16_t word = readWord16(registerAddress);
    if (word >= 0x8000)         // 32768
        return word - 0x10000;  // 65536
    else
       return word;
}

uint32_t QI2C::readDoubleWord16(uint16_t registerAddress)
{
    uint8_t buffer[2] = {0};
    buffer[0] = registerAddress >> 8;
    buffer[1] = registerAddress & 0x00FF;
    write(fd, buffer, 2);

    uint32_t result;
    uint8_t result_buffer[4] = {0};
    if (read(fd, result_buffer, 4) != 4){
        qDebug()<<"I2C Transaction failed!";
        return (0);
    }
    else
       result = result_buffer[0] << 24 | result_buffer[1] << 16 | result_buffer[2] << 8 | result_buffer[1] ;

   return result;
}

bool QI2C::writeBit8(uint8_t registerAddress, uint8_t bitNumber, uint8_t data)
{
    uint8_t buffer[2] = {0};
    buffer[0] = registerAddress & 0x00FF;
    buffer[1] = (data & 0x1) << bitNumber;

    if (write(fd, buffer, 2) != 2){
        qDebug()<<"I2C Transaction failed!";
        return false;
    }
    return true;
}

bool QI2C::writeBit16(uint16_t registerAddress, uint8_t bitNumber, uint8_t data)
{
    uint8_t buffer[3] = {0};
    buffer[0] = registerAddress >> 8;
    buffer[1] = registerAddress & 0x00FF;
    buffer[2] = (data & 0x1) << bitNumber;

    if (write(fd, buffer, 3) != 3){
        qDebug()<<"I2C Transaction failed!";
        return false;
    }
    return true;
}

bool QI2C::writeBits8(uint8_t registerAddress, uint8_t bitStart, uint8_t length, uint8_t data) {
    //      010 value to write
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    // 00011100 mask byte
    // 10101111 original value (sample)
    // 10100011 original & ~mask
    // 10101011 masked | value

    uint8_t buffer[2] = {0};
    buffer[0] = registerAddress & 0x00FF;

    uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
    data <<= (bitStart - length + 1); // shift data into correct position
    data &= mask; // zero all non-important bits in data
    buffer[1] &= ~(mask); // zero all important bits in existing byte
    buffer[1] |= data; // combine data with existing byte

    if (write(fd, buffer, 2) != 2){
        qDebug()<<"I2C Transaction failed!";
        return false;
    }
    return true;
}

bool QI2C::writeBits16(uint16_t registerAddress, uint8_t bitStart, uint8_t length, uint8_t data) {
    //      010 value to write
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    // 00011100 mask byte
    // 10101111 original value (sample)
    // 10100011 original & ~mask
    // 10101011 masked | value

    uint8_t buffer[3] = {0};
    buffer[0] = registerAddress >> 8;
    buffer[1] = registerAddress & 0x00FF;

    uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
    data <<= (bitStart - length + 1); // shift data into correct position
    data &= mask; // zero all non-important bits in data
    buffer[2] &= ~(mask); // zero all important bits in existing byte
    buffer[2] |= data; // combine data with existing byte

    if (write(fd, buffer, 3) != 3){
        qDebug()<<"I2C Transaction failed!";
        return false;
    }
    return true;
}

bool QI2C::writeByte8(uint8_t registerAddress, uint8_t data)
{
    uint8_t buffer[2] = {0};
    buffer[0] = registerAddress & 0x00FF;
    buffer[1] = data;

    if (write(fd, buffer, 2) != 2){
        qDebug()<<"I2C Transaction failed!";
        return false;
    }
    return true;
}

bool QI2C::writeByte16(uint16_t registerAddress, uint8_t data)
{
    uint8_t buffer[3] = {0};
    buffer[0] = registerAddress >> 8;
    buffer[1] = registerAddress & 0x00FF;
    buffer[2] = data;

    if (write(fd, buffer, 3) != 3){
        qDebug()<<"I2C Transaction failed!";
        return false;
    }
    return true;
}

bool QI2C::writeBytes8(uint8_t registerAddress, uint8_t lenght, uint8_t *data){
    for(int i=0;i<lenght;i++){
        if(!writeByte8(registerAddress+i, data[i]))
            return false;
    }
    return true;
}

bool QI2C::writeBytes16(uint16_t registerAddress, uint8_t lenght, uint8_t *data){
    for(int i=0;i<lenght;i++){
        if(!writeByte16(registerAddress+i, data[i]))
            return false;
    }
    return true;
}

bool QI2C::writeWord8(uint8_t registerAddress, uint16_t data)
{
    uint8_t buffer[4] = {0};
    buffer[0] = registerAddress & 0x00FF;
    buffer[1] = data >> 8;
    buffer[2] = data & 0x00FF;

    if (write(fd, buffer, 3) != 3){
        qDebug()<<"I2C Transaction failed!";
        return false;
    }
    return true;
}

bool QI2C::writeWord16(uint16_t registerAddress, uint16_t data)
{
    uint8_t buffer[4] = {0};
    buffer[0] = registerAddress >> 8;
    buffer[1] = registerAddress & 0x00FF;
    buffer[2] = data >> 8;
    buffer[3] = data & 0x00FF;

    if (write(fd, buffer, 4) != 4){
        qDebug()<<"I2C Transaction failed!";
        return false;
    }
    return true;
}

bool QI2C::writeDoubleWord(uint16_t registerAddress, uint32_t data)
{
    uint8_t buffer[6] = {0};
    buffer[0] = registerAddress >> 8;
    buffer[1] = registerAddress & 0x00FF;
    buffer[2] = (data >> 24) & 0x00FF;
    buffer[3] = (data >> 16) & 0x00FF;
    buffer[4] = (data >> 8) & 0x00FF;
    buffer[5] = data & 0x00FF;

    if (write(fd, buffer, 6) != 6){
        qDebug()<<"I2C Transaction failed!";
        return false;
    }
    return true;
}
