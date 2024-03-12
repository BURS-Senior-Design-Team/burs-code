#include "uva.h"

UVA::UVA(){
    _wire = &Wire;
    i2cAddress = LTR390_ADDRESS;   
}


bool UVA::init() {
  uint8_t part_id = readRegister(LTR390_PART_ID);

  if ((part_id  >> 4) != 0x0B) {
    return false;
  }

  if (!reset()) {
    return false;
  }

  enable(true);
  if (!enabled()) {
    return false;
  }

  //Set Mode to UVS
  uint8_t _r = readRegister(LTR390_MAIN_CTRL);
  _r &= 0xF7;
  _r |= ((uint8_t)MODE_UVS << 3);
  writeRegister(LTR390_MAIN_CTRL, (uint8_t)_r);
                   
  //Set Gain
  writeRegister(LTR390_GAIN, (uint8_t)LTR390_GAIN);  //best results for uva is x18
  
  //Set Resolution  
  _r |= (UVA_RESOLUTION << 4);                 //best results for uva is 20-bit
  writeRegister(LTR390_MEAS_RATE, (uint8_t)_r);
  
  return true;
}

/*!
 *  @brief  Perform a soft reset with 10ms delay.
 *  @returns True on success (reset bit was cleared post-write)
 */
bool UVA::reset(void) {
  uint8_t _r = readRegister(LTR390_MAIN_CTRL);
  _r |= B00010000;
  writeRegister(LTR390_MAIN_CTRL, _r);
  delay(10);
  _r = readRegister(LTR390_MAIN_CTRL);
  if (_r != 0) {
    return false;
  }

  return true;
}

/*!
 *  @brief  Read 3-bytes out of UV data register, does not check if data is new!
 *  @returns Up to 20 bits, right shifted into a 32 bit int
 */
uint32_t UVA::readUVA(void) {
  uint8_t _lsb = readRegister(LTR390_UVSDATA_LSB);
  uint8_t _msb = readRegister(LTR390_UVSDATA_MSB);
  uint8_t _hsb = readRegister(LTR390_UVSDATA_HSB);
  _hsb &= 0x0F;
  uint32_t _out = ((uint32_t)_hsb << 16) | ((uint16_t)_msb << 8) | _lsb;
  return _out;
}

/*!
 *  @brief  Enable or disable the light sensor
 *  @param  en True to enable, False to disable
 */
void UVA::enable(bool en) {
  uint8_t _r = readRegister(LTR390_MAIN_CTRL);
  _r |= (1 << 1);
  writeRegister(LTR390_MAIN_CTRL, (uint8_t)_r);
}

/*!
 *  @brief  Read the enabled-bit from the sensor
 *  @returns True if enabled
 */
bool UVA::enabled(void) {
  uint8_t _r = readRegister(LTR390_MAIN_CTRL);
  _r >>= 1;
  _r &= 1;
  return _r;
}

uint8_t UVA::writeRegister(uint8_t reg, uint8_t val){
  _wire->beginTransmission(i2cAddress);
  _wire->write(reg);
  _wire->write(val);
  return _wire->endTransmission();
}

uint8_t UVA::readRegister(uint8_t reg){
  uint8_t regValue = 0;
  _wire->beginTransmission(i2cAddress);
  _wire->write(reg);
  _wire->endTransmission();
  _wire->requestFrom(i2cAddress,1);
  if(_wire->available()){
    regValue = _wire->read();
  }
  return regValue;
}
