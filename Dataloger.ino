#include "sensor.h"

enum State {ARM,GROUND, CLIMB, FLOAT, DESCENT, CHECK, OFF};  // Define the states
State state = ARM;  // Start in the OFF state

//!!!ENSURE PROPER DATA LOADED PREFLIGHT!!!
float seaLevelhPa = 1013.65;               //preasure in hectoPascals at sealevel !!!ENTER DURING SETUP!!!
float expected_flight_time_length = 25;      //hours; used for ensuring balloon has been in flight for more enough 
                                          //time to have reached the 10,000 foot descent altitude for 
                                          //turning the system off 12hrs = 43,200,000*/
float climbing_altitude = 30;            //meters altitude above ground level to start ascent sampling frequency
float floating_altitude= 26000;          //meters altitude above ground level to slow recording rate during floating
float descending_altitude= 25000;        //meters altitude above ground level to increase recording rate during descent
float off_altitude = 1000;               //meters altitude to stop communicating over sda and scl lines to sensing package
float ground_altitude = 0;
const unsigned long timer_check = 3000; //used for checking period of descent time set to 3 seconds for now
float timer = 0; //used for tracking ballon float vs. balloon descent



void setup() {
  Serial.begin(115200);
  
  // Setup Connection with Sensors  
  sensor.All_Sensors_connected();

  // Setup led connections
  sensor.LED_setup();
  Serial.println("ARM");
    
}




void loop() {
  
  switch (state) {
//=========================================================================================================================================================================================
//In ARM state there is no data handling
//Handles LED indication
//Changes to GROUND state when all sensors have been connected

    case ARM:

      //when all sensors are connect will move to next state
      if(sensor.All_Sensors_connected()){
            ground_altitude = sensor.get_Altitude(seaLevelhPa);//logs altitude at ground level of testing site for use in determining balloons location
            state = GROUND;
            Serial.println("GROUND");
      }
      else{
        //do nothing
      }
    break;


//==========================================================================================================================================================================================
//In GROUND state sample rate = 1hz
//will change to UV state when altitude reaches 100ft

    case GROUND:


      //read and save data with frequency of 1 hz
      if(sensor.Frequency_Trigger(1)){
        sensor.get_Scientific(seaLevelhPa);
        sensor.get_Housekeeping();
        
        //Will move to next state when balloon is 30 meters above the ground      
        if (sensor.get_Altitude(seaLevelhPa) >= (ground_altitude + climbing_altitude)) {
          Serial.println("CLIMB");
        
        // Start alarm timing for predicting end of flight time
        sensor.set_alarm(0, 0, 0, 10); // In 10 seconds time

            state = CLIMB;
        }
        else if(millis() >= 10000){
 state = CLIMB;
 Serial.println("CLIMB");
        }
        else{
            //do nothing
        }
      }
     
    break;


//=========================================================================================================================================================================================
//In CLIMB state sample rate = 4hz, recording UV data, altitude, time, external temp, internal temp, and heater control voltage
//will change to Float state when reaching ceiling of 90kft

    case CLIMB:

      //Read and save data with sampling rate of 4hz
      if(sensor.Frequency_Trigger(4)){
          sensor.get_Scientific(seaLevelhPa);
          sensor.get_Housekeeping();
          
        //Will move to next state when balloon is at 26 km compared to sea level
        if (sensor.get_Altitude(seaLevelhPa) >= floating_altitude) {
            Serial.println("FLOAT STATE, ");
            state = FLOAT;
        }
else if(millis() >= 20000){
  state = FLOAT;
  Serial.println("FLOAT STATE, ");
}
        else{
            //do nothing
        }
      }
      else{
        //do nothing
      }
    break;


//=========================================================================================================================================================================================
//In Float state sample rate = 1hz
//recording UVdata, altitude, time, external temp, internal temp and heater control voltage
//will change to CHECK state once we are below 90kft
    case FLOAT:
     
      //Read and save data with sampling rate of 1hz
      if(sensor.Frequency_Trigger(1)){
        sensor.get_Scientific(seaLevelhPa);
        sensor.get_Housekeeping();
        
        //Will move to next state when balloon is at 25 km compared to sea level
        if (sensor.get_Altitude(seaLevelhPa) < descending_altitude){
          timer = millis();//start a timer for checking if we are actually descending                 
          Serial.print("BELOW 90,000 FEET, ");//for demo
          state = CHECK; //change state to check if the balloon is actually descending  
        }
        else if (millis() >= 30000){
  state = DESCENT;
  Serial.print("BELOW 90,000 FEET, ");//for demo
}
     
        else{
          //do nothing
        }
      }
      else{
        //do nothing
      }

    break;


//=========================================================================================================================================================================================
//In CHECK state sample rate = 1hz
//recording UVdata, altitude, time, external temp, internal temp and heater control voltage
//will change to DESCEND state once we are below 90kft and been there for long enough     
      case CHECK:
          
        //Read and save data with sampling rate of 1hz
        if(sensor.Frequency_Trigger(1)){
          sensor.get_Scientific(seaLevelhPa);
          sensor.get_Housekeeping();
        
          if(((millis() - timer) >= timer_check) && (sensor.get_Altitude(seaLevelhPa) < descending_altitude)){  //the balloon is below 25 km and has been for more than 5 seconds meaning the balloon is descending
              Serial.print("DESCENT STATE, ");//for demo
              state = DESCENT;    //change the state to the descent state
              break;          //exits the loop and will be in descent state next loop
          }
          
          else if(((millis() - timer) <= timer_check) && (sensor.get_Altitude(seaLevelhPa) < descending_altitude)){ //balloon is still below 25 km but has not been for more than 5 seconds meaning balloon may not be descending
              break;//will remain in the while loop and continue to check the time and altitude
          }
          
          else{           //the altitude moves back up above 25 kMm (balloon is still in float)
              Serial.print("FLOAT STATE, ");//for demo
              state = FLOAT;    //change the state to the FLOAT state
              break;
          }
        }
        


      
//In DES state sample rate = 16Hz, recording UVdata, altitude, time, external temp, internal temp and heater control voltage
//will change to OFF state once we are below 10kft

      case DESCENT:
  
        //Read and save data with sampling rate of 16hz
        if(sensor.Frequency_Trigger(16)){
          sensor.get_Scientific(seaLevelhPa);
          sensor.get_Housekeeping();

          //Will move to next state when balloon is at 1km above the ground level
          if (sensor.get_Altitude(seaLevelhPa) <= (ground_altitude + off_altitude)) {
            Serial.print("OFF STATE, ");//for demo
            state = OFF;
            break;
          }
          
      else if(millis()>=40000){
        state = OFF;
        Serial.print("OFF STATE, ");//for demo
      }
 
          else{
            //do nothing
          }
        }
        else{
          //do nothing
        }
       
      break;



//In OFF state sampling has been turned off and will end the flight if the time has been long enough
      
      case OFF:
     
        if (sensor.check_alarm()) {
            //Turn off sensing
            //stop all heating commands
            //stop sensing by sending no commands
        }
        else {  /*if enough time has not passed to make sense for the balloon to be in the last part of 
                descent it will go back to the descent state and continue taking data*/
            Serial.print("DESCENT STATE, ");//for demo
            state = DESCENT;
        }
      break;
  }
}
