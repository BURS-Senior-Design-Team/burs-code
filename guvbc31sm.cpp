/******************************************************************************
https://github.com/sichel94/GUVB-C31SM 

Distributed as-is; no warranty is given.
******************************************************************************/
#include "guvbc31sm.h"
#include <Arduino.h>
#include <Wire.h>

GUVB::GUVB()
{
    _i2cPort = &Wire;
}

GUVB::GUVB(TwoWire * w)
{
    _i2cPort = w;
}

bool GUVB::begin()
{
    // Check Wire
    if (_i2cPort == NULL)
        return false;

    _i2cPort->begin();

    // Check ID (and comms to IC)
    if (readRegister(GUVB_REG_CHIPID) != GUVB_CHIP_ID)
        return false;

    enable();
    setIntegrationTime(GUVB_INT_100MS);
    setRange(GUVB_RANGE_X1);
    // Read offset and scale
    writeRegister(GUVB_REG_NVM_CTL, 0x0A); // Select offset
    _offset = readRegister(GUVB_REG_NVM_MSB);

    writeRegister(GUVB_REG_NVM_CTL, 0x0C); // Select b_scale
    byte msb = readRegister(GUVB_REG_NVM_MSB);
    byte lsb = readRegister(GUVB_REG_NVM_LSB);
    _b_scale = ((unsigned short)msb << 8) | (unsigned short)lsb;

    return true;
}

unsigned short GUVB::readUVB()
{
    byte buf[2];
    readRegisters(GUVB_REG_UVB_LSB,buf,2);
    return (unsigned short)buf[0] | ((unsigned short)buf[1] << 8);
}

float GUVB::getUVIndex()
{
    unsigned short raw = readUVB();
    return (raw-_offset)*(float)_b_scale;
}

// Returns UVB in µW/cm^2
float GUVB::getUV()
{
    return getUVIndex()*2.5f;
}

void GUVB::reset()
{
    enable();
    writeRegister(GUVB_REG_SOFT_RESET, 0xA5);
}

void GUVB::enable()
{
    writeRegister(GUVB_REG_MODE, 0x20);
}

void GUVB::shutdown()
{
    writeRegister(GUVB_REG_MODE, 0x03);
}

void GUVB::setIntegrationTime(byte res)
{
    writeRegister(GUVB_REG_RES_UV, res);
}

void GUVB::setRange(byte res)
{
    writeRegister(GUVB_REG_RANGE_UVB, res);
}

// WRITE A SINGLE REGISTER
void GUVB::writeRegister(byte reg, byte data)
{
    writeRegisters(reg, &data, 1);
}

// WRITE MULTIPLE REGISTERS
void GUVB::writeRegisters(byte reg, byte *buffer, byte len)
{
    _i2cPort->beginTransmission(_deviceAddress);
    _i2cPort->write(reg);
    for (int x = 0; x < len; x++)
        _i2cPort->write(buffer[x]);
    _i2cPort->endTransmission();
}

// READ A SINGLE REGISTER
byte GUVB::readRegister(byte reg)
{
    _i2cPort->beginTransmission(_deviceAddress);
    _i2cPort->write(reg);
    _i2cPort->endTransmission(false);

    _i2cPort->requestFrom(_deviceAddress, (byte)1);

    if (_i2cPort->available())
    {
        return _i2cPort->read(); 
    }
    else
    {
        return 0;
    }
}

// READ MULTIPLE REGISTERS
void GUVB::readRegisters(byte reg, byte *buffer, byte len)
{
    _i2cPort->beginTransmission(_deviceAddress);
    _i2cPort->write(reg);
    _i2cPort->endTransmission(false);
    _i2cPort->requestFrom(_deviceAddress, len);

    if (_i2cPort->available() == len)
    {
        for (int x = 0; x < len; x++)
            buffer[x] = _i2cPort->read();
    }
}
