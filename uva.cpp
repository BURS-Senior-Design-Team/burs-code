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

float UVA::getUVI(){
  uint32_t raw = readUVA();
  uint8_t _gain = (uint8_t)(getGain());
  uint8_t _resolution = (uint8_t)(getResolution());
  float uvi = (float)(raw) / ((gain_factor[_gain] / gain_factor[LTR390_GAIN_18]) * (res_factor[_resolution] / res_factor[LTR390_RESOLUTION_20BIT]) * (float)(UV_SENSITIVITY)) * (float)(WFAC);
  return uvi * 250;
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

/*!
 *  @brief  Set the sensor gain
 *  @param  gain The desired gain: LTR390_GAIN_1, LTR390_GAIN_3, LTR390_GAIN_6
 *  LTR390_GAIN_9 or LTR390_GAIN_18
 */
void UVA::setGain(ltr390_gain_t gain) {
  writeRegister(LTR390_GAIN, (uint8_t)gain);
}

/*!
 *  @brief  Get the sensor's gain
 *  @returns gain The current gain: LTR390_GAIN_1, LTR390_GAIN_3, LTR390_GAIN_6
 *  LTR390_GAIN_9 or LTR390_GAIN_18
 */
ltr390_gain_t UVA::getGain(void) {
  uint8_t _r = readRegister(LTR390_GAIN);
  _r &= 7;
  return (ltr390_gain_t)_r;
}

/*!
 *  @brief  Set the sensor resolution. Higher resolutions take longer to read!
 *  @param  res The desired resolution: LTR390_RESOLUTION_13BIT,
 *  LTR390_RESOLUTION_16BIT, LTR390_RESOLUTION_17BIT, LTR390_RESOLUTION_18BIT,
 *  LTR390_RESOLUTION_19BIT or LTR390_RESOLUTION_20BIT
 */
void UVA::setResolution(ltr390_resolution_t res) {
  uint8_t _r = 0;
  _r |= (res << 4);
  writeRegister(LTR390_MEAS_RATE, (uint8_t)_r);
}

/*!
 *  @brief  Get the sensor's resolution
 *  @returns The current resolution: LTR390_RESOLUTION_13BIT,
 *  LTR390_RESOLUTION_16BIT, LTR390_RESOLUTION_17BIT, LTR390_RESOLUTION_18BIT,
 *  LTR390_RESOLUTION_19BIT or LTR390_RESOLUTION_20BIT
 */
ltr390_resolution_t UVA::getResolution(void) {
  uint8_t _r = readRegister(LTR390_MEAS_RATE);
  _r &= 0x70;
  _r = 7 & (_r >> 4);
  return (ltr390_resolution_t)_r;
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
