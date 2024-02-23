#ifndef sensor_h
#define sensor_h

//Included Librarys
#include "RTClib.h"             //RTC and Internal Thermistor Library
#include <Adafruit_DPS310.h>    //Altimeter Library
#include "Adafruit_LTR390.h"    //UV-A Library
#include <SPI.h>                //used in data transfer for SD card
#include <SD.h>                 //SD module Library

class SensorClass
{
  public:
    SensorClass();
    
    //Setup Sensor Funcions
    bool RTC_connected();
    bool Altimeter_connected();
    bool UV_A_connected();
    bool SD_connected();
    bool All_Sensors_connected();

    //led functions
    void LED_setup();
    void blink_led(int PIN);
    bool led_on = false;
    
    //Functions to read data and write to sd card
    void get_Scientific(float seaLevelhPa);
    void get_Housekeeping();
    float get_Altitude(float seaLevelhPa);
    int i = 0;
    //Sets an alarm which will inidcate if it is logical the balloon is at the end of flight based on time
    void set_alarm(float dd, float hr, float m, float sec);
    bool check_alarm();

    
    //Function to create frequency of sampling rates and led blinking
    float period_timer = 0;
    bool Frequency_Trigger(float sampling_frequency);

  private:
    const int chipSelect = 10;
    
};

extern SensorClass sensor;


#endif
