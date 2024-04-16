#include "uvb.h"

UVB::UVB(){
    _wire = &Wire;
    i2cAddress = GUVB_ADDR;
}

bool UVB::init(){
    // Check Wire
    if (_wire == NULL)
        return false;

    // Check ID (and comms to IC)
    if (readRegister(GUVB_REG_CHIPID) != GUVB_CHIP_ID)
        return false;

    writeRegister(GUVB_REG_MODE, 0x20); //enable
    
    writeRegister(GUVB_REG_RES_UV, GUVB_INT_100MS); //Select Resolution
    
    writeRegister(GUVB_REG_RANGE_UVB, GUVB_RANGE_X1); //select gain
    
    // Read offset and scale
    writeRegister(GUVB_REG_NVM_CTL, 0x0A); // Select offset
    _offset = readRegister(GUVB_REG_NVM_MSB);

    writeRegister(GUVB_REG_NVM_CTL, 0x0C); // Select b_scale
    uint8_t msb = readRegister(GUVB_REG_NVM_MSB);
    uint8_t lsb = readRegister(GUVB_REG_NVM_LSB);
    _b_scale = ((unsigned short)msb << 8) | (unsigned short)lsb;

    return true;
}

uint16_t UVB::readUVB()
{
    byte buf[2];
    readRegisters(GUVB_REG_UVB_LSB,buf,2);
    return (unsigned short)buf[0] | ((unsigned short)buf[1] << 8);
}

float UVB::getUVIndex()
{
    unsigned short raw = readUVB();
    //return (raw-_offset)*(float)_b_scale;
    return raw;
}

// WRITE A SINGLE REGISTER
void UVB::writeRegister(uint8_t reg, uint8_t data)
{
    writeRegisters(reg, &data, 1);
}

// WRITE MULTIPLE REGISTERS
void UVB::writeRegisters(uint8_t reg, uint8_t *buffer, uint8_t len)
{
    _wire->beginTransmission(i2cAddress);
    _wire->write(reg);
    for (int x = 0; x < len; x++)
        _wire->write(buffer[x]);
    _wire->endTransmission();
}

// READ A SINGLE REGISTER
byte UVB::readRegister(uint8_t reg){
    _wire->beginTransmission(i2cAddress);
    _wire->write(reg);
    _wire->endTransmission();
    _wire->requestFrom(i2cAddress,1);
    if (_wire->available()){
        return _wire->read(); 
    }
        return 0;
}

// READ MULTIPLE REGISTERS
void UVB::readRegisters(uint8_t reg, uint8_t *buffer, uint8_t len)
{
    _wire->beginTransmission(i2cAddress);
    _wire->write(reg);
    _wire->endTransmission();
    _wire->requestFrom(i2cAddress, len);
    if (_wire->available() == len)
    {
        for (int x = 0; x < len; x++)
            buffer[x] = _wire->read();
    }
}
