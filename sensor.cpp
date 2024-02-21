#include "Arduino.h"
#include "sensor.h"


//Sensor Declarations
RTC_DS3231 rtc;       //Real Time Clock
Adafruit_DPS310 dps;  //Altimeter
Adafruit_LTR390 ltr = Adafruit_LTR390();  //UV-A Sensor

SensorClass::SensorClass(){
}



//Function to set up all sensors
//
void SensorClass::Setup_Sensors(){
    
    if(RTCandInternalTemp_connected == false){
      Setup_RTCandInternalTemp();
    }
    
    if(UV_A_connected == false){
      Setup_UV_A();
    }
    
    if(Altimeter_connected == false){
      Setup_Altimeter();
    }

    if((RTCandInternalTemp_connected == true)&&
       (Altimeter_connected == true)&&
       (UV_A_connected == true)){
        
        All_Sensors_Connected = true;
    }

}


  
//Function to set up the RTC and Internal Thermistor sensors
//
void SensorClass::Setup_RTCandInternalTemp(){

      //Real Time Clock Setup used only to reference for the shutdown protocall
      if (! rtc.begin()) {
          Serial.println("Failed to find RTC");
          RTCandInternalTemp_connected = false;
      }
    
      else{
          Serial.println("RTC Connected");
          DateTime now = rtc.now(); // Get current time
          
          //Resets time to current time
          rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

          // Print flight date and time at startup
          char buff[] = "Start time is hh:mm:ss DDD, DD MMM YYYY";
          Serial.println(now.toString(buff));
      
          // Disable and clear both alarms
          rtc.disableAlarm(1);
          rtc.clearAlarm(1);
      
          RTCandInternalTemp_connected = true;
      }
}


//Function to set up the Altimeter
//
  void SensorClass::Setup_Altimeter(){
      
        if (! dps.begin_I2C()) {             // Can pass in I2C address here
          Serial.println("Failed to find DPS");

          Altimeter_connected = false;
        }

        else{
          Serial.println("Altimeter Connected");

          Altimeter_connected = true;
          
          // Setup highest precision
            dps.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
            dps.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);
        }
}



//Function to set up UV_A Sensor
//
  void SensorClass::Setup_UV_A(){
      if ( ! ltr.begin() ) {
        Serial.println("Failed to find LTR sensor");

        UV_A_connected = false;
      }
  
      else{
        Serial.println("Found LTR sensor!");
        
        UV_A_connected = true;
        ltr.setMode(LTR390_MODE_UVS); //set sensor to UV sensing mode
        ltr.setGain(LTR390_GAIN_3);   //Set gain to 3
        ltr.setResolution(LTR390_RESOLUTION_16BIT); //Set Resolution
        ltr.setThresholds(100, 1000);
        ltr.configInterrupt(true, LTR390_MODE_UVS);
      }
}



//Function to read all scientific data
//
  void SensorClass::get_Scientific(float seaLevelhPa){
      get_time();
      get_altitude(seaLevelhPa);
      get_UVA();
}



//Function to get all housekeeping data
//
void SensorClass::get_HouseKeeping(){
  get_internal_temp();
}


//function to get the uva measurement
//
void SensorClass::get_UVA(){
        sensor.uv_a = ltr.readUVS();
}



//function to get the uvb measurement
long SensorClass::get_UVB(int test_it, long demo_uv_b[], long uv_b){
  uv_b = demo_uv_b[test_it];
  return uv_b;
}

//function to get the uvc measurement
long SensorClass::get_UVC(int test_it, long demo_uv_c[], long uv_c){
  uv_c = demo_uv_c[test_it];
  return uv_c;
}



//function to read altitude from altimeter
//
void SensorClass::get_altitude(float seaLevelhPa){
      sensor.altitude = dps.readAltitude(seaLevelhPa);
}



//function to get the internal temperature
//
void SensorClass::get_internal_temp(){
      sensor.internal_temp = rtc.getTemperature();
}

//function to get the external temperature
long SensorClass::get_ext_temp(int test_it, long demo_ext_temp[], long ext_temp){
  ext_temp = demo_ext_temp[test_it];
  return ext_temp;
}

//function to get the current time
void SensorClass::get_time(){
  current_time = millis();
}



//Function to Control Sampling Frequency
//
void SensorClass::Frequency(float sampling_frequency){
  float period = 1/sampling_frequency;
  float c_time = millis();
  
  if((c_time - period_timer) >= (period*1000)){
    period_timer = c_time;
    trigger = true;
  }
  else{
    trigger = false;
  }

}
SensorClass sensor = SensorClass();
