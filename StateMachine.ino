#include "sensor.h"
#include "sd_card.h"
#include "led.h"

enum State {ARM,GROUND, CLIMB, FLOAT, DESCENT, CHECK, OFF};  // Define the states
State state = ARM;  // Start in the OFF state

//!!!ENSURE PROPER DATA LOADED PREFLIGHT!!!
float seaLevelhPa = 1013.65;               //preasure in hectoPascals at sealevel !!!ENTER DURING SETUP!!!
const unsigned long flight_time = 25;   //hours; used for ensuring balloon has been in flight for more enough 
                                          //time to have reached the 10,000 foot descent altitude for 
                                          //turning the system off 12hrs = 43,200,000*/
float climbing_altitude = 30;            //meters altitude above ground level to start ascent sampling frequency
float floating_altitude= 26000;          //meters altitude above ground level to slow recording rate during floating
float descending_altitude= 25000;        //meters altitude above ground level to increase recording rate during descent
float off_altitude = 1000;               //meters altitude to stop communicating over sda and scl lines to sensing package
float End_day = 0;
float End_hour = 0;
float End_minute = 0;
float End_second = 45;
const unsigned long timer_check = 3000; //used for checking period of descent time set to 3 seconds for now
float ground_altitude = 0;
unsigned long timer;

bool End_of_flight = false;



void setup() {
  Serial.begin(115200);
  
  // Setup Connection with Sensors  
  sensor.Setup_Sensors();

  // Setup led connections
  led.SETUP();//set up for LED blinking
  Serial.print ("GROUND STATE, ");
    
}

void loop() {
  
  switch (state) {

//In ARM state there is no data handling
//Handles LED indication
//Changes to GROUND state when all sensors have been connected

    case ARM:

      //determine if trigger has changed
      sensor.Frequency(sensor.Arm_Frequency);
      
      led.LED_Indicator();

      //when all sensors are connect will move to next state
      if(sensor.All_Sensors_Connected == true){
            sensor.get_altitude(seaLevelhPa);
            ground_altitude = sensor.altitude;
            state = GROUND;
            Serial.print ("ARM STATE, ");
      }

      //if not all sensors are connected will try to reconnect
      else{
        sensor.Setup_Sensors();
      }

    break;

//In GROUND state sample rate = 1hz
//will change to UV state when altitude reaches 100ft

    case GROUND:

      //determine if trigger has changed
      sensor.Frequency(sensor.Ground_Frequency);
      
      // Check if alarm by polling by using alarmFired
/*      if (rtc.alarmFired(1) == true){
        End_of_flight = true;
        Serial.println("EoF");
        rtc.clearAlarm(1);
      }
*/      
      //read and save data
      if(sensor.trigger ==true){
          sensor.get_Scientific(seaLevelhPa);
          sensor.get_HouseKeeping();
          sd.writeScientific();
          sd.writeHouseKeeping();

        //Will move to next state when balloon is 30 meters above the ground      
        if (sensor.altitude >= (ground_altitude + climbing_altitude)) {
            Serial.print ("Clibing State, ");
        
        // Start alarm timing for predicting end of flight time
//        rtc.setAlarm1(now + TimeSpan(End_day, End_hour, End_minute, End_second), DS3231_A1_Second); // In 10 seconds time

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

      //determine if trigger has changed
      sensor.Frequency(sensor.Climb_Frequency);
      
      // Check if alarm by polling by using alarmFired
/*      if (rtc.alarmFired(1) == true){
        End_of_flight = true;
        rtc.clearAlarm(1);
      }
*/      
      //Read and save data
      if(sensor.trigger == true){
          sensor.get_Scientific(seaLevelhPa);
          sensor.get_HouseKeeping();
          sd.writeScientific();
          sd.writeHouseKeeping();

        //Will move to next state when balloon is at 26 km compared to sea level
        if (sensor.altitude >= floating_altitude) {
            Serial.print("FLOAT STATE, ");
            state = FLOAT;
        }

        else{
            break;
        }
      }
      break;



    //In Float state sample rate = 1hz
    //recording UVdata, altitude, time, external temp, internal temp and heater control voltage
    //will change to CHECK state once we are below 90kft
    case FLOAT:

      //determine if trigger has changed
      sensor.Frequency(sensor.Float_Frequency);
      
      // Check if alarm by polling by using alarmFired
/*      if (rtc.alarmFired(1) == true){
        End_of_flight = true;
        Serial.println("Eof");
        rtc.clearAlarm(1);
      }
*/      
      //handle scientific data during normal floating operation
      if(sensor.trigger == true){
          sensor.get_Scientific(seaLevelhPa);
          sensor.get_HouseKeeping();
          sd.writeScientific();
          sd.writeHouseKeeping();
      
      }
      //Will move to next state when balloon is at 25 km compared to sea level
      if (sensor.altitude < descending_altitude){
          timer = millis();//start a timer for checking if we are actually descending
          Serial.print("BELOW 90,000 FEET, ");//for demo
          state = CHECK; //change state to check if the balloon is actually descending  
      }
      break;


      
//In CHECK state sample rate = 1hz
//recording UVdata, altitude, time, external temp, internal temp and heater control voltage
//will change to DESCEND state once we are below 90kft and been there for long enough     
      case CHECK:
      
        //determine if trigger has changed
        sensor.Frequency(sensor.Check_Frequency);
        
        // Check if alarm by polling by using alarmFired
/*        if (rtc.alarmFired(1) == true){
          End_of_flight = true;
          Serial.println("Eof");
          rtc.clearAlarm(1);
        }
*/        
        //handle scientific data
        if(sensor.trigger == true){
          sensor.get_Scientific(seaLevelhPa);
          sensor.get_HouseKeeping();
          sd.writeScientific();
          sd.writeHouseKeeping();
          
          if(((sensor.current_time - timer) >= timer_check) && (sensor.altitude < descending_altitude)){  //the balloon is below 25 km and has been for more than 5 seconds meaning the balloon is descending
              Serial.print("DESCENT STATE, ");//for demo
              state = DESCENT;    //change the state to the descent state
              break;          //exits the loop and will be in descent state next loop
          }
          
          else if(((sensor.current_time - timer) <= timer_check) && (sensor.altitude < descending_altitude)){ //balloon is still below 25 km but has not been for more than 5 seconds meaning balloon may not be descending
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
      
        //determine if trigger has changed
        sensor.Frequency(sensor.Descent_Frequency);
        
        // Check if alarm by polling by using alarmFired
/*      if (rtc.alarmFired(1) == true){
          End_of_flight = true;
          Serial.println("Eof");
          rtc.clearAlarm(1);
        }
*/      
        //handle scientific data
        if(sensor.trigger == true){
            sensor.get_Scientific(seaLevelhPa);
            sensor.get_HouseKeeping();
            sd.writeScientific();
            sd.writeHouseKeeping();

            //Will move to next state when balloon is at 1km above the ground level
            if (sensor.altitude <= (ground_altitude + off_altitude)) {
                Serial.print("OFF STATE, ");//for demo
                state = OFF;
                break;
            }

        }
       
      break;



//In OFF state sampling has been turned off and will end the flight if the time has been long enough
      
      case OFF:
     
        if (End_of_flight == true) {
            led.OFF();
            //Turn off sensing
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
