
#include "RTClib.h"             //RTC and Internal Thermistor Library
#include <Adafruit_DPS310.h>
#include <SPI.h>
#include <SD.h>
#include <LTR390.h>
#include "guvbc31sm.h"

//Sensor Declarations
RTC_DS3231 rtc;       //Real Time Clock
Adafruit_DPS310 dps;  //Altimeter
LTR390 ltr = LTR390();//UV-A Sensor
GUVB UVB;             //UV-B Sensor

//State Machine Setup
enum State {ARM, GROUND, CLIMB, FLOAT, DESCENT, CHECK, OFF};  // Define the states
State state = ARM;  // Start in the ARM state

//Initialize Critical Variables Pre-Flight
float seaLevelhPa = 1016;//preasure in hectoPascals at sealevel !!!ENTER DURING SETUP!!!

//value for creating frequency of writing
float previous_time = 0;

//values to check if balloon is descending or floating lower base on time
float descent_timer = 5000; //5 seconds
float timer = 0;  //set at end of float state

//value to know the zero altitude where ballon starts from
float ground_altitude =0;
float start_time = 0;

//flags for determining connection
bool sd_connected = false;
bool rtc_connected = false;
bool uva_connected = false;
bool uvb_connected = false;
bool uvc_connected = false;
bool altimeter_connected = false;


void setup() {
  Serial.begin(115200);

  //LED Pin declarations
  pinMode(2, OUTPUT);         //rtc led indicator
  pinMode(3, OUTPUT);         //uv-a led indicator
  pinMode(4, OUTPUT);         //uv-b led indicator
  pinMode(5, OUTPUT);         //uv-c led indicator
  pinMode(6, OUTPUT);         //altimeter led indicator
  pinMode(7, OUTPUT);         //sd reader/writer led indicator
  
  //connect to sd writer
  pinMode(10, OUTPUT);//chipselect pin of 10
  if (!SD.begin(10)) {
    sd_connected = false;
  }
  else{
    sd_connected = true;
    digitalWrite(2,HIGH);
  }
  
  //setup rtc
  if(! rtc.begin()){
    rtc_connected = false;
  }
  else{      
    // Disable and clear both alarms
    rtc.disableAlarm(1);
    rtc.clearAlarm(1);
    digitalWrite(3, HIGH);//turn on led to indicate connected
    rtc_connected = true;
  }

  //setup uva sensor
  if ( ! ltr.init() ) {
    uva_connected = false;
  }
  else{     
    ltr.setMode(LTR390_MODE_UVS); //set sensor to UV sensing mode
    uva_connected = true;
    digitalWrite(4,HIGH);
  }

  //Connect uvb
  if (!UVB.begin()){
    uvb_connected = false;
  }
  else{
    uvb_connected = true;
    digitalWrite(5,HIGH);
  }
  
  //Connect uvc
  uvc_connected = true;
  digitalWrite(6,HIGH);
  
  //connect to altimeter
  if (! dps.begin_I2C()) {
    altimeter_connected = false;
  }
  else{
    ground_altitude = dps.readAltitude(seaLevelhPa);  //store altitude of ground level for state machine control
    altimeter_connected = true;
    digitalWrite(7, HIGH);
  }

  state = ARM;
  
}

void loop() {
switch (state) {

//==========================================================================================================================================================================================
//In ARM state the sensor package is not sampling
//this is inteneded to be a loop checking if all sensors are active and an sd card has been input
//if there is a sensor not communicating check connections or that and sd card has been inserted
    case ARM:

  //if all sensors are connected and sd card inserted will move to ground and begin sampling data
  if (sd_connected && rtc_connected && uva_connected && uvb_connected && uvc_connected && altimeter_connected) {
    
    // Print flight date and time at startup
    DateTime now = rtc.now(); // Get current time  
    char buff[] = "Start time is hh:mm:ss DDD, DD MMM YYYY";
    File time_stamp = SD.open("time.txt", FILE_WRITE);
    time_stamp.println(now.toString(buff));  
    time_stamp.close(); 
    start_time = millis();
    state = GROUND;
  }

  //try to connect to sd card again
  else if (!sd_connected){
    if (!SD.begin(10)){
      sd_connected = false;
      digitalWrite(2,LOW);
    }
    else{
      sd_connected = true;
      digitalWrite(2,HIGH);
    }
  }

  //try to connect to rtc again
  else if(!rtc_connected){
    if(! rtc.begin()){
      rtc_connected = false;
      digitalWrite(3,LOW);
    }
    else{
      // Disable and clear both alarms
      rtc.disableAlarm(1);
      rtc.clearAlarm(1);
      rtc_connected = true;
      digitalWrite(3, HIGH);//turn on led to indicate connected 
    }
  }

  //try to connect to uva again
  else if(!uva_connected){
    if ( ! ltr.init() ) {
      uva_connected = false;
      digitalWrite(4,LOW);
    }
    else{
      Serial.println("UV_A Sensor Connected");        
      ltr.setMode(LTR390_MODE_UVS); //set sensor to UV sensing mode
      uva_connected = true;
      digitalWrite(4,HIGH);
    }
  }

  //try to connect uvb again
  else if (!uvb_connected){
    if (!UVB.begin()){
      uvb_connected = false;
    }
    else{
      uvb_connected = true;
      digitalWrite(5,HIGH);
    }
  }
  
  //try to connect to altimeter again
  else if (!altimeter_connected){
      //connect to altimeter
    if (! dps.begin_I2C()) {
      altimeter_connected = false;
      digitalWrite(7,LOW);
    }
    else{
      ground_altitude = dps.readAltitude(seaLevelhPa);  //store altitude of ground level for state machine control
      altimeter_connected = true;
      digitalWrite(7, HIGH);
    }    
  }

break;
//==========================================================================================================================================================================================
//In GROUND state sample rate = 1hz
//will change to UV state when altitude reaches 30 meters above ground level

    case GROUND:
      if ((millis() - previous_time) >= 1000){
                  
        //read all sensors
        float altitude = dps.readAltitude(seaLevelhPa);
        float current_time = millis();
        float uva =ltr.readUVS();
        float uvb = UVB.getUV();
        float int_temp = rtc.getTemperature();
        
        //set time to check for next sample 
        previous_time = current_time;

        //open science data file
        File dataFile = SD.open("sci.csv", FILE_WRITE);
        
        //write values to science data file
        dataFile.print(current_time);   //Milliseconds
        dataFile.print(", ");
        dataFile.print(altitude);       //Meters
        dataFile.print(", ");
        dataFile.print(uva);            //UV Count
        dataFile.print(",");
        dataFile.println(uvb);          //uW/cm^2
        
        //close science data file
        dataFile.close();

        //open housekeeping data file
        File houseFile = SD.open("house.csv", FILE_WRITE);

        //write values to housekeeping file
        houseFile.print(current_time);
        houseFile.print(",");
        houseFile.println(int_temp);
        
        //close housekeeping file
        houseFile.close();
        
        //after being on for 10 minutes the leds will turn off
        if((current_time-start_time) >= 5000){                  //set to 5 seconds for testing
          digitalWrite(2, LOW);
          digitalWrite(3, LOW);
          digitalWrite(4, LOW);
          digitalWrite(5, LOW);
          digitalWrite(6, LOW);
          digitalWrite(7, LOW);
          digitalWrite(8, LOW);
          
        }
        //When balloon is 30 meters above the ground move to climb state and set alarm for determining
        //if balloon has been in flight long enough to stop reading files
        if(altitude >= (ground_altitude + 30)){
          rtc.setAlarm1(rtc.now() + TimeSpan(0,0,2,30), DS3231_A1_Second);//set to expected flight time
          state = CLIMB;
        }
    }
break;



//=========================================================================================================================================================================================
//In CLIMB state sample rate = 4hz, recording UV data, altitude, time, external temp, internal temp, and heater control voltage
//will change to Float state when reaching ceiling of 26kilometers

    case CLIMB:
      if ((millis() - previous_time) >= 250){
        
          
        //read all sensors
        float altitude = dps.readAltitude(seaLevelhPa);
        float current_time = millis();
        float uva =ltr.readUVS();
        float uvb = UVB.getUV();
        float int_temp = rtc.getTemperature();
        
        //set time to check for next sample 
        previous_time = current_time;

        //open science data file
        File dataFile = SD.open("sci.csv", FILE_WRITE);
        
        //write values to science data file
        dataFile.print(current_time);   //Milliseconds
        dataFile.print(", ");
        dataFile.print(altitude);       //Meters
        dataFile.print(", ");
        dataFile.print(uva);            //UV Count
        dataFile.print(",");
        dataFile.println(uvb);          //uW/cm^2
        
        //close science data file
        dataFile.close();

        //open housekeeping data file
        File houseFile = SD.open("house.csv", FILE_WRITE);

        //write values to housekeeping file
        houseFile.print(current_time);
        houseFile.print(",");
        houseFile.println(int_temp);
        
        //close housekeeping file
        houseFile.close();

        //When balloon is above 26 kilometers go to FLOAT state
        if(altitude >= 26000){
          state = FLOAT;
        }
    }
break;



//=========================================================================================================================================================================================
//In Float state sample rate = 1hz
//recording UVdata, altitude, time, external temp, internal temp and heater control voltage
//will change to CHECK state once we are below 25kilometers
    case FLOAT:
     
      if ((millis() - previous_time) >= 1000){
        
          
        //read all sensors
        float altitude = dps.readAltitude(seaLevelhPa);
        float current_time = millis();
        float uva =ltr.readUVS();
        float uvb = UVB.getUV();
        float int_temp = rtc.getTemperature();
        
        //set time to check for next sample 
        previous_time = current_time;

        //open science data file
        File dataFile = SD.open("sci.csv", FILE_WRITE);
        
        //write values to science data file
        dataFile.print(current_time);   //Milliseconds
        dataFile.print(", ");
        dataFile.print(altitude);       //Meters
        dataFile.print(", ");
        dataFile.print(uva);            //UV Count
        dataFile.print(",");
        dataFile.println(uvb);          //uW/cm^2
        
        //close science data file
        dataFile.close();

        //open housekeeping data file
        File houseFile = SD.open("house.csv", FILE_WRITE);

        //write values to housekeeping file
        houseFile.print(current_time);
        houseFile.print(",");
        houseFile.println(int_temp);
        
        //close housekeeping file
        houseFile.close();

        //When balloon is below 25 kilometers go to check state
        if(altitude <= 25000){
          timer = millis();
          state = CHECK;
        }
    }
break;


//=========================================================================================================================================================================================
//In CHECK state sample rate = 1hz
//recording UVdata, altitude, time, external temp, internal temp and heater control voltage
//will change to DESCEND state once we are below 25 kilometers and has been below 25 kilometers for 5 seconds  
      case CHECK:
      
      if ((millis() - previous_time) >= 1000){
        
          
        //read all sensors
        float altitude = dps.readAltitude(seaLevelhPa);
        float current_time = millis();
        float uva =ltr.readUVS();
        float uvb = UVB.getUV();
        float int_temp = rtc.getTemperature();
        
        //set time to check for next sample 
        previous_time = current_time;

        //open science data file
        File dataFile = SD.open("sci.csv", FILE_WRITE);
        
        //write values to science data file
        dataFile.print(current_time);   //Milliseconds
        dataFile.print(", ");
        dataFile.print(altitude);       //Meters
        dataFile.print(", ");
        dataFile.print(uva);            //UV Count
        dataFile.print(",");
        dataFile.println(uvb);          //uW/cm^2
        
        //close science data file
        dataFile.close();

        //open housekeeping data file
        File houseFile = SD.open("house.csv", FILE_WRITE);

        //write values to housekeeping file
        houseFile.print(current_time);
        houseFile.print(",");
        houseFile.println(int_temp);
        
        //close housekeeping file
        houseFile.close();

        //the balloon is below 25 km and has been for more than 5 seconds meaning the balloon is descending
        if(((millis() - timer) >= descent_timer) && (altitude <= 25000)){
          Serial.println("to descent");
          state = DESCENT;    //change the state to the descent state
          break;          //exits the loop and will be in descent state next loop
        }

        //balloon is still below 25 km but has not been for more than 5 seconds meaning balloon may not be descending
        else if(((millis() - timer) <= descent_timer) && (altitude <= 25000)){ 
          Serial.println("stay in check");
          break;//will remain in the while loop and continue to check the time and altitude
        }
        
        //the altitude moves back up above 25 kMm (balloon is still in float)  
        else{
          Serial.println("back to float");
          state = FLOAT;    //change the state to the FLOAT state
          break;
        }
    }

//=========================================================================================================================================================================================
//In DESCENT state sample rate = 16Hz, recording UVdata, altitude, time, external temp, internal temp and heater control voltage
//will change to OFF state once we are below 1 kilometer

      case DESCENT:

      if ((millis() - previous_time) >= 62.5){
        
          
        //read all sensors
        float altitude = dps.readAltitude(seaLevelhPa);
        float current_time = millis();
        float uva =ltr.readUVS();
        float uvb = UVB.getUV();
        float int_temp = rtc.getTemperature();
        
        //set time to check for next sample 
        previous_time = current_time;

        //open science data file
        File dataFile = SD.open("sci.csv", FILE_WRITE);
        
        //write values to science data file
        dataFile.print(current_time);   //Milliseconds
        dataFile.print(", ");
        dataFile.print(altitude);       //Meters
        dataFile.print(", ");
        dataFile.print(uva);            //UV Count
        dataFile.print(",");
        dataFile.println(uvb);          //uW/cm^2
        
        //close science data file
        dataFile.close();

        //open housekeeping data file
        File houseFile = SD.open("house.csv", FILE_WRITE);

        //write values to housekeeping file
        houseFile.print(current_time);
        houseFile.print(",");
        houseFile.println(int_temp);
        
        //close housekeeping file
        houseFile.close();

        //When balloon is below 1 kilometer from the ground go to Off state
        if(altitude <= (ground_altitude + 1000)){
          Serial.println("OFF");
          state = OFF;
        }
    }
break;



//=========================================================================================================================================================================================
//In OFF state sampling has been turned off and will end the flight if the time has been long enough
      
      case OFF:

        //check if the alarm has gone off
        if (rtc.alarmFired(1)) {
            //Turn off sensing
            //stop all heating commands
            //stop sensing by sending no commands
        }
        else {  /*if enough time has not passed to make sense for the balloon to be in the last part of 
                descent it will go back to the descent state and continue taking data*/
            state = DESCENT;
        }
      break;
  }
}
