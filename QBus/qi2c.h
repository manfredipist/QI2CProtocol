#ifndef QI2C_H
#define QI2C_H

#include <QObject>
#include <QDebug>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/i2c-dev.h>

class QI2C : public QObject
{
    Q_OBJECT
public:
    explicit QI2C(const uint8_t &i2c_bus, const uint8_t &i2c_address, QObject *parent = nullptr);
    ~QI2C();
public slots:
    int getFileDescriptor(){return fd;}

    void i2cInit();
    void i2cClose();
    uint8_t readBit8(uint8_t registerAddress, uint8_t bitNumber);
    uint8_t readBit16(uint16_t registerAddress, uint8_t bitNumber);
    uint8_t readBits8(uint8_t registerAddress, uint8_t bitStart, uint8_t length);
    uint8_t readBits16(uint16_t registerAddress, uint8_t bitStart, uint8_t length);
    uint8_t readByte8(uint8_t registerAddress);
    uint8_t readByte16(uint16_t registerAddress);
    void readBytes8(uint8_t registerAddress, uint8_t length, uint8_t *data);
    void readBytes16(uint16_t registerAddress, uint8_t length, uint8_t *data);
    uint16_t readWord8(uint8_t registerAddress);
    uint16_t readWord16(uint16_t registerAddress);
    uint16_t readWord8_2c(uint8_t registerAddress);
    uint16_t readWord16_2c(uint16_t registerAddress);
    uint32_t readDoubleWord16(uint16_t registerAddress);

    bool writeBit8(uint8_t registerAddress, uint8_t bitNumber, uint8_t data);
    bool writeBit16(uint16_t registerAddress, uint8_t bitNumber, uint8_t data);
    bool writeBits8(uint8_t registerAddress, uint8_t bitStart, uint8_t lenght, uint8_t data);
    bool writeBits16(uint16_t registerAddress, uint8_t bitStart, uint8_t lenght, uint8_t data);
    bool writeByte8(uint8_t registerAddress, uint8_t data);
    bool writeByte16(uint16_t registerAddress, uint8_t data);
    bool writeBytes8(uint8_t registerAddress, uint8_t lenght, uint8_t *data);
    bool writeBytes16(uint16_t registerAddress, uint8_t lenght, uint8_t *data);
    bool writeWord8(uint8_t registerAddress, uint16_t data);
    bool writeWord16(uint16_t registerAddress, uint16_t data);
    bool writeDoubleWord(uint16_t registerAddress, uint32_t data);
private:
    const uint8_t i2c_bus = 0x1;
    const uint8_t i2c_address;

    int fd; //The generic connection to user's chosen I2C hardware
};

#endif // QI2C_H
