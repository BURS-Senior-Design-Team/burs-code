#include <Wire.h>
#include "LTR390.h"
#include "guvbc31sm.h"
#include "AD_Converter.h"

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
GUVB sensor;

#define I2C_ADDRESS 0x53

/* There are several ways to create your LTR390 object:
 * LTR390 ltr390 = LTR390()                    -> uses Wire / I2C Address = 0x53
 * LTR390 ltr390 = LTR390(OTHER_ADDR)          -> uses Wire / I2C_ADDRESS
 * LTR390 ltr390 = LTR390(&wire2)              -> uses the TwoWire object wire2 / I2C_ADDRESS
 * LTR390 ltr390 = LTR390(&wire2, I2C_ADDRESS) -> all together
 * Successfully tested with two I2C busses on an ESP32
 */
LTR390 ltr390(I2C_ADDRESS);

void setup() {
   Serial.begin(115200);
  Wire.begin();
  if(!ltr390.init()){
    Serial.println("UVA not connected");
  }
  if(sensor.begin() == false){
    Serial.println("UVB not connected");
  }
  if (!ads.begin()) {
    Serial.println("UVC not connected");
  }
  
  ltr390.setMode(LTR390_MODE_UVS);
  ltr390.setGain(LTR390_GAIN_18);
  ltr390.setResolution(LTR390_RESOLUTION_20BIT);

   // The ADC input range (or gain) can be changed via the following
   // functions, but be careful never to exceed VDD +0.3V max, or to
   // exceed the upper and lower limits if you adjust the input range!
   // Setting these values incorrectly may destroy your ADC!
   //                                                                ADS1015  ADS1115
   //                                                                -------  -------
   //ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
   //ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
   //ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
   // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
   //ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
   ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

}

void loop() {
               
  float uva_i = ltr390.getUVI();
  float uva = uva_i * 2.5f;

  Serial.print("UVA irradiation: "); 
  Serial.print(uva);
  Serial.println(" uW/cm^2");
      
  float uvb_i = sensor.getUVIndex();
  float uvb = uvb_i * 2.5f;

  Serial.print("UVB irradiation: ");
  Serial.print(uvb);
  Serial.println(" uW/cm^2");

  int16_t results = ads.readADC_SingleEnded(0);
  Serial.print("UVC volts: "); 
  Serial.print(ads.computeVolts(results));
  Serial.println(" v");
  
  delay(1000);
}
