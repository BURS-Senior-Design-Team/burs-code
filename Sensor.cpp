#include "Arduino.h"
#include "sensor.h"


//Sensor Declarations
RTC_DS3231 rtc;       //Real Time Clock
Adafruit_DPS310 dps;  //Altimeter
Adafruit_LTR390 ltr = Adafruit_LTR390();  //UV-A Sensor

SensorClass::SensorClass(){
}

//======================================================================================================
//Function to set up and return if the RTC and Internal Thermistor sensors are connected
//
bool SensorClass::RTC_connected(){

  if (! rtc.begin()) {
    Serial.println("Failed to find RTC");
    
    return false;
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

    digitalWrite(2, HIGH);//turn on led to indicate connected
    return true;
  }
}


//===================================================================================================
//Function to set up and return if the Altimeter is connected
//
bool SensorClass::Altimeter_connected(){
      
  if (! dps.begin_I2C()) {             // Can pass in I2C address here
    Serial.println("Failed to find DPS");

    return false;
  }

  else{
    Serial.println("Altimeter Connected");
                
    // Setup highest precision
    dps.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
    dps.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);
    digitalWrite(6, HIGH);//turn on led to indicated connected
    return true;
  }
}


//====================================================================================================
//Function to set up and return UV_A Sensor is connected
//
bool SensorClass::UV_A_connected(){
  if ( ! ltr.begin() ) {
    Serial.println("Failed to find LTR sensor");

    return false;
  }
  
  else{
    Serial.println("UV_A Sensor Connected");
        
    ltr.setMode(LTR390_MODE_UVS); //set sensor to UV sensing mode
    ltr.setGain(LTR390_GAIN_3);   //Set gain to 3
    ltr.setResolution(LTR390_RESOLUTION_16BIT); //Set Resolution
    ltr.setThresholds(100, 1000);
    ltr.configInterrupt(true, LTR390_MODE_UVS);

    digitalWrite(3, HIGH);//turn on led to indicate connected
    return true;
  }
}


//========================================================================================================
//Function to setup and return if SD reader/writer is connected
//
bool SensorClass::SD_connected(){
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
      
    return false;
  }
  else{
    Serial.println("SD Module Connect");
    digitalWrite(7, HIGH);  //turn on led to indicate connected
    return true;
  }
}


//========================================================================================================
//function to house all calls to set up and check if sensors are connected
//
bool SensorClass::All_Sensors_connected(){
  
  
  bool uva = UV_A_connected();
  bool uvb = true;
  bool uvc = true;
  bool alt = Altimeter_connected();
  bool sd  = SD_connected();
  bool et = true;
  bool rtc = RTC_connected();
  
  //if all sensors are connected return true
  if(alt && uva && rtc && sd){
    return true;
  }

  //if any sensors are disconnected return false
  else{
    if(!rtc){
      blink_led(2);
    }
    if(!uva){
      blink_led(3);
    }
    if(!uvb){
      blink_led(4);
    }
    if(!uvc){
      blink_led(5);
    }
    if(!alt){
      blink_led(6);
    }
    if(!sd){
      blink_led(7);
    }
    if(!et){
      blink_led(8);
    }
    return false;
  }
}


//=========================================================================================================
//function to setup all pinouts for led indicator functionallity
//
void SensorClass::LED_setup(){
  pinMode(2, OUTPUT);         //rtc led indicator
  pinMode(3, OUTPUT);         //uv-a led indicator
  pinMode(4, OUTPUT);         //uv-b led indicator
  pinMode(5, OUTPUT);         //uv-c led indicator
  pinMode(6, OUTPUT);         //altimeter led indicator
  pinMode(7, OUTPUT);         //sd reader/writer led indicator
  pinMode(8, OUTPUT);         //external thermistor indicator
}



//=========================================================================================================
//function to blink leds
//
void SensorClass::blink_led(int PIN){
  if(Frequency_Trigger(5) && (led_on == true)){
    digitalWrite(PIN, LOW);
    led_on == false;
  }
  else if (Frequency_Trigger(5) && (led_on == false)){
    digitalWrite(PIN, HIGH);
  }
  else{
    //do nothing
  }
}



//========================================================================================================
//Function to create frequency of sampling
//=========================================================================================================
bool SensorClass::Frequency_Trigger(float sampling_frequency){
  float period = (1/sampling_frequency);
  float current_time = millis();

  if((current_time - period_timer) >= (period * 1000)){
    period_timer = current_time;
    return true;
  }

  else{
    return false;
  }
}



//=========================================================================================================
//Sets an alarm which will inidcate if it is logical the balloon is at the end of flight based on time
//=========================================================================================================
void SensorClass::set_alarm(float dd, float hr, float m, float sec){
  rtc.setAlarm1(rtc.now() + TimeSpan(dd, hr, m, sec), DS3231_A1_Second);
}



//=========================================================================================================
//checks if alarm has fired
//=========================================================================================================
bool SensorClass::check_alarm(){
  return(rtc.alarmFired(1));
}


    
//===========================================================================================================
//Function to get all Scientific data points, build a string of the data points, and save the datapoints to the sd card
//=================================================================================================================
void SensorClass:: get_Scientific(float seaLevelhPa){
  
  String dataString = "";
  float current_time = millis();
  dataString += String(current_time);
  dataString += ",";
  float altitude = dps.readAltitude(seaLevelhPa);
  dataString += String(altitude);
  dataString += ",";
  float uva = ltr.readUVS();
  dataString += String(uva);
  dataString += ",";

  File dataFile = SD.open("Scienctific.txt", FILE_WRITE);

  if(dataFile){
    Serial.println("writting");
    dataFile.println(dataString);
    dataFile.close();
    Serial.println("close");
  }
}



//===========================================================================================================
//Function to get all Housekeeping data points, build a string of the data points, and save the datapoints to the sd card
//=================================================================================================================
void SensorClass:: get_Housekeeping(){
  
  String dataString = "";
  float current_time = millis();
  dataString += String(current_time);
  dataString += ",";
  float internal_temp = rtc.getTemperature();
  dataString += String(internal_temp);


  File dataFile = SD.open("Housekeeping.txt", FILE_WRITE);

  if(dataFile){
    Serial.println("writting");
    dataFile.println(dataString);
    dataFile.close();
    Serial.println("close");
  }
}



//==========================================================================================================
//Function to get the ground altitude for reference in determining balloon position
//========================================================================================================
float SensorClass::get_Altitude(float seaLevelhPa){
  return dps.readAltitude(seaLevelhPa);
}

SensorClass sensor = SensorClass();
