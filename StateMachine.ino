#include "sensor.h"
#include "sd_card.h"
#include "led.h"
#include <Adafruit_DPS310.h>
#include "RTClib.h"

//Sensor Declaration
Adafruit_DPS310 dps;  //Altimeter
RTC_DS3231 rtc;       //Real Time Clock

enum State {ARM, CLIMB, FLOAT, DESCENT, CHECK, OFF};  // Define the states
State state = ARM;  // Start in the OFF state

//!!!ENSURE PROPER DATA LOADED PREFLIGHT!!!
float seaLevelhPa = 1013.65;               //preasure in hectoPascals at sealevel !!!ENTER DURING SETUP!!!
const unsigned long flight_time = 25;   //hours; used for ensuring balloon has been in flight for more enough 
                                          //time to have reached the 10,000 foot descent altitude for 
                                          //turning the system off 12hrs = 43,200,000*/
float climbing_altitude = 655;            //altitude to start ascent
float floating_altitude= 657;             //altitude to slow recording during floating
float descending_altitude= 656;           //altitude to increase recording rate during descent
float off_altitude = 655;                 //altitude to turn of sensing package
float End_day = 0;
float End_hour = 0;
float End_minute = 0;
float End_second = 45;
const unsigned long timer_check = 3000; //used for checking period of descent time set to 3 seconds for now

//Recorded Data Points
unsigned long period_timer;
unsigned long current_time;
unsigned long timer;
float altitude = 0;
long uv_a;
long uv_b;
long uv_c;
float SystemTemp;
long ExternalTemp;
long heat_cont;
bool End_of_flight = false;

double sample_rate = 1;//hz
double period;

void setup() {

  Serial.begin(115200);

//Real Time Clock Setup used only to reference for the shutdown protocall
    if (! rtc.begin()) {
      Serial.println("Couldn't find RTC");
      Serial.flush();
      abort();
    }
    Serial.println("RTC Connected");
    //Resets time to current time
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
    // Disable and clear both alarms
    rtc.disableAlarm(1);
    rtc.clearAlarm(1);
         
    DateTime now = rtc.now(); // Get current time

    // Print flight date and time at startup
    char buff[] = "Start time is hh:mm:ss DDD, DD MMM YYYY";
    Serial.println(now.toString(buff));

//led setup
    led.SETUP();//set up for LED blinking

//Altimeter Setup
    if (! dps.begin_I2C()) {             // Can pass in I2C address here
      Serial.println("Failed to find DPS");
      abort();
    }
    Serial.println("Altimeter Connected");
  
    // Setup highest precision
    dps.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
    dps.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);

  
  Serial.print ("ARM STATE, ");
    
}

void loop() {
  
  DateTime now = rtc.now(); // Get current time
  
  switch (state) {

    //In ARM state sample rate = 1hz
    //will change to UV state when altitude reaches 100ft
    case ARM:

      sample_rate = 1;//hz
      period = 1/sample_rate;
      current_time = Sensor.get_time(current_time);

      // Check if alarm by polling by using alarmFired
      if (rtc.alarmFired(1) == true){
        End_of_flight = true;
        Serial.println("EoF");
        rtc.clearAlarm(1);
      }
      
      //handle scientific data
      if((current_time - period_timer) >= (period*1000)){
        led.BLINK();
        period_timer = current_time;
        altitude = dps.readAltitude(seaLevelhPa);
        SystemTemp = rtc.getTemperature();
        //ExternalTemp = demo_ext_temp[test_it];
        //uv_a = demo_uv_a[test_it];
        //uv_b = demo_uv_b[test_it];
        //uv_c = demo_uv_c[test_it];
        sd.writeTime(current_time);
        //sd.writeExternalTemp(ExternalTemp);
        sd.writeAltitude(altitude);
        sd.writeSystemTemp(SystemTemp);
        //sd.writeUVA(uv_a);
        //sd.writeUVB(uv_b);
        //sd.writeUVC(uv_c);
        
        if (altitude >= climbing_altitude) {
        Serial.print ("Clibing State, ");
        
        // Start alarm timing for predicting end of flight time
        rtc.setAlarm1(now + TimeSpan(End_day, End_hour, End_minute, End_second), DS3231_A1_Second); // In 10 seconds time

        state = CLIMB;
        }
        
        else{
          break;
        }
      }
      
      break;


    //In CLIMB state sample rate = 4hz, recording UV data, altitude, time, external temp, internal temp, and heater control voltage
    //will change to Float state when reaching ceiling of 90kft
    case CLIMB:

      sample_rate = 4;//hz
      period = 1/sample_rate;
    
      current_time = Sensor.get_time(current_time);

      // Check if alarm by polling by using alarmFired
      if (rtc.alarmFired(1) == true){
        End_of_flight = true;
        rtc.clearAlarm(1);
      }
      
      //handle scientific data
      if((current_time - period_timer) >= (period*1000)){
        led.BLINK();
        period_timer = current_time;
        altitude = dps.readAltitude(seaLevelhPa);
        //ExternalTemp = demo_ext_temp[test_it];
        //uv_a = demo_uv_a[test_it];
        //uv_b = demo_uv_b[test_it];
        //uv_c = demo_uv_c[test_it];
        sd.writeTime(current_time);
        //sd.writeExternalTemp(ExternalTemp);
        sd.writeAltitude(altitude);
        //sd.writeUVA(uv_a);
        //sd.writeUVB(uv_b);
        //sd.writeUVC(uv_c);
        
        if (altitude >= floating_altitude) {
          delay(1000);
          Serial.print("FLOAT STATE, ");
          state = FLOAT;
        }

        else{
          break;
        }
      }

      //record internal temp
      //record external temp
      //record heater voltage

      break;


    //In Float state sample rate = 1hz
    //recording UVdata, altitude, time, external temp, internal temp and heater control voltage
    //will change to CHECK state once we are below 90kft
    case FLOAT:

      sample_rate = 1;//hz
      period = 1/sample_rate;
      
      current_time = Sensor.get_time(current_time);

      // Check if alarm by polling by using alarmFired
      if (rtc.alarmFired(1) == true){
        End_of_flight = true;
        Serial.println("Eof");
        rtc.clearAlarm(1);
      }
      
      //handle scientific data during normal floating operation
      if((current_time - period_timer) >= (period*1000)){
        led.BLINK();
        period_timer = current_time;
        altitude = dps.readAltitude(seaLevelhPa);
        //ExternalTemp = demo_ext_temp[test_it];
        //uv_a = demo_uv_a[test_it];
        //uv_b = demo_uv_b[test_it];
        //uv_c = demo_uv_c[test_it];
        sd.writeTime(current_time);
        //sd.writeExternalTemp(ExternalTemp);
        sd.writeAltitude(altitude);
        //sd.writeUVA(uv_a);
        //sd.writeUVB(uv_b);
        //sd.writeUVC(uv_c);
      }
      
      if (altitude < descending_altitude){
        timer = millis();//start a timer for checking if we are actually descending
        Serial.print("BELOW 90,000 FEET, ");//for demo
        state = CHECK; //change state to check if the balloon is actually descending  
      }
      
      break;
      
      //In CHECK state sample rate = 1hz
      //recording UVdata, altitude, time, external temp, internal temp and heater control voltage
      //will change to DESCEND state once we are below 90kft and been there for long enough
      case CHECK:

        sample_rate = 1;
        period = 1/sample_rate;

        current_time = Sensor.get_time(current_time);

        // Check if alarm by polling by using alarmFired
        if (rtc.alarmFired(1) == true){
          End_of_flight = true;
          Serial.println("Eof");
          rtc.clearAlarm(1);
        }
        
        //handle scientific data
        if((current_time - period_timer) >= (period*1000)){
          led.BLINK();
          period_timer = current_time;
          altitude = dps.readAltitude(seaLevelhPa);
          //ExternalTemp = demo_ext_temp[test_it];
          //uv_a = demo_uv_a[test_it];
          //uv_b = demo_uv_b[test_it];
          //uv_c = demo_uv_c[test_it];
          sd.writeTime(current_time);
          //sd.writeExternalTemp(ExternalTemp);
          sd.writeAltitude(altitude);
          //sd.writeUVA(uv_a);
          //sd.writeUVB(uv_b);
          //sd.writeUVC(uv_c);
        }
        if(((current_time - timer) >= timer_check) && (altitude < descending_altitude)){  //the balloon is below 90000 feet and has been for more than 5 seconds
          Serial.print("DESCENT STATE, ");//for demo
          state = DESCENT;    //change the state to the descent state
          break;          //exits the loop and will be in descent state next loop
        }
          
        else if(((current_time - timer) <= timer_check) && (altitude < descending_altitude)){ //balloon is still below 90000 feet but has not been for more than 5 seconds
          break;//will remain in the while loop and continue to check the time and altitude
        }
          
        else{           //the altitude moves back up above 90000 feet(balloon is still in float)
          Serial.print("FLOAT STATE, ");//for demo
          state = FLOAT;    //change the state to the FLOAT state
          break;
        }
      
      //In DES state sample rate = 16Hz, recording UVdata, altitude, time, external temp, internal temp and heater control voltage
      //will change to OFF state once we are below 10kft
      case DESCENT:

      sample_rate=16; //increase the sampling rate for the descent portion
      period = 1/sample_rate;

      current_time = Sensor.get_time(current_time);

      // Check if alarm by polling by using alarmFired
      if (rtc.alarmFired(1) == true){
        End_of_flight = true;
        Serial.println("Eof");
        rtc.clearAlarm(1);
      }
      
      //handle scientific data
      if((current_time - period_timer) >= (period*1000)){
        led.BLINK();
        period_timer = current_time;
        altitude = dps.readAltitude(seaLevelhPa);
        //ExternalTemp = demo_ext_temp[test_it];
        //uv_a = demo_uv_a[test_it];
        //uv_b = demo_uv_b[test_it];
        //uv_c = demo_uv_c[test_it];
        sd.writeTime(current_time);
        //sd.writeExternalTemp(ExternalTemp);
        sd.writeAltitude(altitude);
        //sd.writeUVA(uv_a);
        //sd.writeUVB(uv_b);
        //sd.writeUVC(uv_c);

        if (altitude <= off_altitude) {
        //turn off all sampling
        //turn off all recording
        Serial.print("OFF STATE, ");//for demo
        state = OFF;
        break;
        }
        else{
        }
      }

      break;

      /*In OFF state sampling has been turned off and will end the flight if the time has been
        long enough*/
      case OFF:

      current_time = Sensor.get_time(current_time);
      
      if (End_of_flight == true) {
        led.OFF();
        //power down transistor and shuts power off;
      }
      else {  /*if enough time has not passed to make sense for the balloon to be in the last part of 
                descent it will go back to the descent state and continue taking data*/
        Serial.print("DESCENT STATE, ");//for demo
        state = DESCENT;
        //turn sensing transistor back on
      }
      break;
  }
}
