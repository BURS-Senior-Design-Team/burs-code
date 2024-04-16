#ifndef uvb_h
#define uvb_h

#define GUVB_ADDR       0x39
#define GUVB_REG_CHIPID   0x00
#define GUVB_REG_MODE     0x01
#define GUVB_REG_RES_UV   0x04
#define GUVB_REG_RANGE_UVB  0x07
#define GUVB_REG_MODE_CTL   0x0A
#define GUVB_REG_SOFT_RESET 0x0B
#define GUVB_REG_UVB_LSB  0x17
#define GUVB_REG_UVB_MSB  0x18
#define GUVB_REG_NVM_CTL  0x30
#define GUVB_REG_NVM_MSB  0x31
#define GUVB_REG_NVM_LSB  0x32

#define GUVB_CHIP_ID    0x62

#define GUVB_INT_800MS    0x00
#define GUVB_INT_400MS    0x01
#define GUVB_INT_200MS    0x02
#define GUVB_INT_100MS    0x03

#define GUVB_RANGE_X1   0x00
#define GUVB_RANGE_X2   0x01
#define GUVB_RANGE_X4   0x02
#define GUVB_RANGE_X8   0x03
#define GUVB_RANGE_X16    0x04
#define GUVB_RANGE_X32    0x05
#define GUVB_RANGE_X64    0x06
#define GUVB_RANGE_X128   0x07

#include <Arduino.h>
#include <Wire.h>

class UVB{
  
  public:
    UVB();
    bool init();

    void writeRegister(uint8_t, uint8_t);
    void writeRegisters(uint8_t, uint8_t*, uint8_t);
    byte readRegister(uint8_t);
    void readRegisters(uint8_t, uint8_t*, uint8_t);

    float getUVIndex();
    uint16_t readUVB();

  private:
    TwoWire *_wire;
    int i2cAddress;
    uint8_t _offset = 0;
    uint8_t _b_scale = 0;
};

#endif
