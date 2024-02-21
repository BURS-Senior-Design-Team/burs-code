#ifndef sensor_h
#define sensor_h

//Included Librarys
#include "RTClib.h"             //RTC and Internal Thermistor Library
#include <Adafruit_DPS310.h>    //Altimeter Library
#include "Adafruit_LTR390.h"    //UV-A Library
#include "led.h"

class SensorClass
{
  public:
    SensorClass();
    
    //Setup Sensor Funcions
    void Setup_Sensors();
    void Setup_RTCandInternalTemp();
    void Setup_Altimeter();
    void Setup_UV_A();
    
    //Sensor Setup Variables
    bool All_Sensors_Connected = false;
    bool RTCandInternalTemp_connected = false;
    bool Altimeter_connected = false;
    bool UV_A_connected = false;

    //Sample Frequencies
    float Arm_Frequency = 5; //hz
    float Ground_Frequency = 1; //hz
    float Climb_Frequency = 4; //hz
    float Float_Frequency = 1; //hz
    float Check_Frequency = 1; //hz
    float Descent_Frequency = 16; //hz

    //Data Variables
    float uv_a;
    float uv_b;
    float uv_c;
    float altitude;
    float current_time;
    float internal_temp;
    float external_temp;

    //Sampling Frequency Variables
    float period_timer = 0;
    bool trigger = false;
    
    //Sampling Functions
    void get_Scientific(float seaLevelhPa);
    void get_HouseKeeping();
    void get_UVA();
    long get_UVB(int test_it, long demo_uv_b[], long uv_b);
    long get_UVC(int test_it, long demo_uv_c[], long uv_c);
    void get_altitude(float seaLevelhPa);
    void get_internal_temp();
    long get_ext_temp(int test_it, long demo_ext_temp[], long ext_temp);
    void get_time();
    void Frequency(float sampling_frequency);
    
};

extern SensorClass sensor;


#endif
