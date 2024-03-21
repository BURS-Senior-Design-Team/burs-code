#include "LTR390.h"



LTR390::LTR390(){
    _wire = &Wire;
    i2cAddress = LTR390_ADDRESS;   
}

bool LTR390::init() {
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

  setGain(LTR390_GAIN_18);
  setResolution(LTR390_RESOLUTION_20BIT);

  return true;
}

/*!
 *  @brief  Perform a soft reset with 10ms delay.
 *  @returns True on success (reset bit was cleared post-write)
 */
bool LTR390::reset(void) {
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
uint32_t LTR390::readUVS(void) {
  uint8_t _lsb = readRegister(LTR390_UVSDATA_LSB);
  uint8_t _msb = readRegister(LTR390_UVSDATA_MSB);
  uint8_t _hsb = readRegister(LTR390_UVSDATA_HSB);
  _hsb &= 0x0F;
  uint32_t _out = ((uint32_t)_hsb << 16) | ((uint16_t)_msb << 8) | _lsb;
  return _out;
}

float LTR390::getUVI(){
  uint32_t raw = readUVS();
  uint8_t _gain = (uint8_t)(LTR390_GAIN_18);
  uint8_t _resolution = (uint8_t)(LTR390_RESOLUTION_20BIT);
  float uvi = (float)(raw) / ((gain_factor[_gain] / gain_factor[LTR390_GAIN_18]) * (res_factor[_resolution] / res_factor[LTR390_RESOLUTION_20BIT]) * (float)(UV_SENSITIVITY)) * (float)(WFAC);
  return uvi;
}

/*!
 *  @brief  Enable or disable the light sensor
 *  @param  en True to enable, False to disable
 */
void LTR390::enable(bool en) {
  uint8_t _r = readRegister(LTR390_MAIN_CTRL);
  _r |= (1 << 1);
  writeRegister(LTR390_MAIN_CTRL, (uint8_t)_r);
}

/*!
 *  @brief  Read the enabled-bit from the sensor
 *  @returns True if enabled
 */
bool LTR390::enabled(void) {
  uint8_t _r = readRegister(LTR390_MAIN_CTRL);
  _r >>= 1;
  _r &= 1;
  return _r;
}

/*!
 *  @brief  Set the sensor mode to EITHER ambient (LTR390_MODE_ALS) or UV
 * (LTR390_MODE_UVS)
 *  @param  mode The desired mode - LTR390_MODE_UVS or LTR390_MODE_ALS
 */
void LTR390::setMode(ltr390_mode_t mode) {
  uint8_t _r = readRegister(LTR390_MAIN_CTRL);
  _r &= 0xF7;
  _r |= ((uint8_t)mode << 3);
  writeRegister(LTR390_MAIN_CTRL, (uint8_t)_r);
}

/*!
 *  @brief  Set the sensor gain
 *  @param  gain The desired gain: LTR390_GAIN_1, LTR390_GAIN_3, LTR390_GAIN_6
 *  LTR390_GAIN_9 or LTR390_GAIN_18
 */
void LTR390::setGain(ltr390_gain_t gain) {
  writeRegister(LTR390_GAIN, (uint8_t)gain);
}

/*!
 *  @brief  Set the sensor resolution. Higher resolutions take longer to read!
 *  @param  res The desired resolution: LTR390_RESOLUTION_13BIT,
 *  LTR390_RESOLUTION_16BIT, LTR390_RESOLUTION_17BIT, LTR390_RESOLUTION_18BIT,
 *  LTR390_RESOLUTION_19BIT or LTR390_RESOLUTION_20BIT
 */
void LTR390::setResolution(ltr390_resolution_t res) {
  uint8_t _r = 0;
  _r |= (res << 4);
  writeRegister(LTR390_MEAS_RATE, (uint8_t)_r);
}

uint8_t LTR390::writeRegister(uint8_t reg, uint8_t val){
  _wire->beginTransmission(i2cAddress);
  _wire->write(reg);
  _wire->write(val);
  return _wire->endTransmission();
}

uint8_t LTR390::readRegister(uint8_t reg){
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

