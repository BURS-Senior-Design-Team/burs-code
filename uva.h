#ifndef uva_h
#define uva_h

#include "Arduino.h"
#include <Wire.h>

#define UV_SENSITIVITY  1400

#define WFAC            1

#define LTR390_ADDRESS 0x53         ///< I2C address
#define LTR390_MAIN_CTRL 0x00       ///< Main control register
#define LTR390_MEAS_RATE 0x04       ///< Resolution and data rate
#define LTR390_GAIN 0x05            ///< ALS and UVS gain range
#define LTR390_PART_ID 0x06         ///< Part id/revision register
#define LTR390_MAIN_STATUS 0x07     ///< Main status register
#define LTR390_UVSDATA_LSB 0x10     ///< UVS data lowest byte
#define LTR390_UVSDATA_MSB 0x11     ///< UVS data middle byte
#define LTR390_UVSDATA_HSB 0x12     ///< UVS data highest byte

#define  MODE_UVS 1

//Gain settings for UVA sensor
//most accurate using x18
//#define  UVA_GAIN 0 // x1
//#define  UVA_GAIN 1 // x3
//#define  UVA_GAIN 2 // x6
//#define  UVA_GAIN 3 // x9
#define  UVA_GAIN 4 // x18  most accurate gain for uva measurements

//Resolution settings for UVA sensor
//most accurate using 20bit
#define  UVA_RESOLUTION 0  //20 bit = 400ms conversion time
//#define  UVA_RESOLUTION 1  //19 bit = 200ms conversion time
//#define  UVA_RESOLUTION 2  //18 bit = 100ms conversion time
//#define  UVA_RESOLUTION 3  //17 bit = 50ms conversion time
//#define  UVA_RESOLUTION 4  //16 bit = 25ms conversion time
//#define  UVA_RESOLUTION 5  //13 bit = 12.5ms conversion time

/*!    @brief  Sensor gain for UV or ALS  */
typedef enum {
  LTR390_GAIN_1 = 0,
  LTR390_GAIN_3,
  LTR390_GAIN_6,
  LTR390_GAIN_9,
  LTR390_GAIN_18,
} ltr390_gain_t;

/*!    @brief Measurement resolution (higher res means slower reads!)  */
typedef enum {
  LTR390_RESOLUTION_20BIT,
  LTR390_RESOLUTION_19BIT,
  LTR390_RESOLUTION_18BIT,
  LTR390_RESOLUTION_17BIT,
  LTR390_RESOLUTION_16BIT,
  LTR390_RESOLUTION_13BIT,
} ltr390_resolution_t;


class UVA{
public:
  UVA();
  
  bool init();

  bool reset(void);

  void enable(bool en);
  bool enabled(void);

  void setGain(ltr390_gain_t gain);
  ltr390_gain_t getGain(void);

  void setResolution(ltr390_resolution_t res);
  ltr390_resolution_t getResolution(void);

  uint32_t readUVA(void);
  float getUVI();

  uint8_t writeRegister(uint8_t reg, uint8_t val);
  uint8_t readRegister(uint8_t reg);

private:
  TwoWire *_wire;
  int i2cAddress;
  float gain_factor[5] = {1, 3, 6, 9, 18};
  float res_factor[6] = {4, 2, 1, 0.5, 0.25, 0.03125};
};

#endif
