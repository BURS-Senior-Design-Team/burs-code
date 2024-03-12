
#include <Adafruit_DPS310.h>

//Sensor Declarations
Adafruit_DPS310 dps;  //Altimeter


//State Machine Setup
enum State {ARM, GROUND, CLIMB, FLOAT, DESCENT, CHECK, OFF};  // Define the states
State state = ARM;  // Start in the ARM state

//Initialize Critical Variables Pre-Flight
float seaLevelhPa = 1016;//preasure in hectoPascals at sealevel !!!ENTER DURING SETUP!!!

//value for creating frequency of writing
float previous_time = 0;

//values to check if balloon is descending or floating lower base on time
float descent_timer = 2000; //5 seconds
float timer;  //set at end of float state
float projected_flight_time = 120000; //2 minutes for testing
float start_time;
//value to know the zero altitude where ballon starts from
float ground_altitude;
float altitude;
float current_time;
//flags for determining connection
bool altimeter_connected = false;


void setup() {
  Serial.begin(115200);
 
  //connect to altimeter
  if (! dps.begin_I2C()) {
    altimeter_connected = false;
    Serial.println("Altitmeter not connected");
  }
  else{
    ground_altitude = dps.readAltitude(seaLevelhPa);  //store altitude of ground level for state machine control
    altimeter_connected = true;
    Serial.println("Altimeter Connected");
    Serial.print("Ground Altitude = ");
    Serial.print(ground_altitude);
    Serial.println(" m");
  }

  state = ARM;
  Serial.println("In Arm State");
  
}

void loop() {
switch (state) {

//==========================================================================================================================================================================================
//In ARM state the sensor package is not sampling
//this is inteneded to be a loop checking if all sensors are active and an sd card has been input
//if there is a sensor not communicating check connections or that and sd card has been inserted
    case ARM:

  //All Sensors are Connected
  if (altimeter_connected) { 
    Serial.println("All Sensors Connected");
    Serial.println("Going to GROUND");
    state = GROUND; //go to ground state
    break;
  }

  //Altimeter is not connected
  else if (!altimeter_connected){
      //connect to altimeter
    if (! dps.begin_I2C()) {
      Serial.println("Altimeter Not Connected");
      altimeter_connected = false;
    }
    else{
      ground_altitude = dps.readAltitude(seaLevelhPa);  //store altitude of ground level for state machine control
      altimeter_connected = true;
      Serial.println("Altimeter Connected");
      Serial.print("Ground Altitude = ");
      Serial.print(ground_altitude);
      Serial.println(" m");
    }
    break;    
  }
  
  else{
    break;
  }

//==========================================================================================================================================================================================
//In GROUND state sample rate = 1hz
//will change to UV state when altitude reaches 4 meters above ground level

    case GROUND:
      if ((millis() - previous_time) >= 1000){//1 hz sampling rate
                  
        //read all sensors
        altitude = dps.readAltitude(seaLevelhPa);
        current_time = millis();
        
        //set time to check for next sample 
        previous_time = current_time;

        //Balloon is 4 meters above the ground
        if(altitude >= (ground_altitude + 4)){
          start_time = current_time;  //set time refference for expected flight time
          Serial.print("Going to CLIMB at: ");
          Serial.print(altitude);
          Serial.println(" m");
          state = CLIMB;  //go to climb state
        }
    }
break;



//=========================================================================================================================================================================================
//In CLIMB state sample rate = 4hz, recording UV data, altitude, time, external temp, internal temp, and heater control voltage
//will change to Float state when reaching ceiling of 16 meters above the ground

    case CLIMB:
      if ((millis() - previous_time) >= 250){//4hz sampling rate
        
          
        //read all sensors
        altitude = dps.readAltitude(seaLevelhPa);
        current_time = millis();
        
        //reset time to check for next sample 
        previous_time = current_time;

        //Balloon is 16 meters above the ground
        if(altitude >= (ground_altitude + 16)){
          Serial.print("Goint to FLOAT at: ");
          Serial.print(altitude);
          Serial.println(" m");
          state = FLOAT;//go to float state
        }
    }
break;



//=========================================================================================================================================================================================
//In Float state sample rate = 1hz
//recording UVdata, altitude, time, external temp, internal temp and heater control voltage
//will change to CHECK state once we are below 12 meters above the ground
    case FLOAT:
     
      if ((millis() - previous_time) >= 1000){//sample rate is 1hz
        
          
        //read all sensors
        altitude = dps.readAltitude(seaLevelhPa);
        current_time = millis();
        
        //set time to check for next sample 
        previous_time = current_time;
        
        //Balloon is 12 meters above the ground
        if(altitude <= (ground_altitude + 12)){
          Serial.print("Going to CHECK at: ");
          Serial.print(altitude);
          Serial.println(" m");
          timer = millis();   //start timer for checking if balloon is descending
          state = CHECK;  //go to check state
        }
    }
break;


//=========================================================================================================================================================================================
//In CHECK state no sample rate
//will change to DESCEND state once we are below 12 meters above the ground and has been below 12 meters above the ground for 20 seconds  
      case CHECK:

        //read all sensors
        altitude = dps.readAltitude(seaLevelhPa);
        current_time = millis();

     
        //Balloon is below 12 meters above the ground
        if(altitude <= (ground_altitude + 12)){
          //balloon has been below 12 meters long enough to consider it as descending
          if((current_time - timer) >= descent_timer){
            previous_time = current_time;
            Serial.print("Going to DESCENT at: ");
            Serial.print(altitude);
            Serial.println(" m");
            state = DESCENT;  //go to descent state
            break;
          }
          //balloon has not remained below 12 meters for long enough
          else{
            Serial.println("Staying in CHECK");
            break;
          }
        }
       
        //balloon is back above 12 meters indicating it is floating
        else if(altitude >= (ground_altitude + 12)){
          previous_time = current_time;
          Serial.print("Going back to FLOAT at: ");
          Serial.print(altitude);
          Serial.println(" m");
          state = FLOAT;    //go back to the FLOAT state
          break;
        }
        else{
          break;
        }
        

//=========================================================================================================================================================================================
//In DESCENT state sample rate = 16Hz, recording UVdata, altitude, time, external temp, internal temp and heater control voltage
//will change to OFF state once we are below 5 meters above the ground

      case DESCENT:
      if ((millis() - previous_time) >= 62.5){//16hz sampling rate
        
        //read all sensors
        altitude = dps.readAltitude(seaLevelhPa);
        current_time = millis();
        
        //set time to check for next sample 
        previous_time = current_time;
        
        //Ballon is below 4 meters above the ground
        if(altitude <= (ground_altitude + 4)){
          Serial.print("Going to OFF at: ");
          Serial.print(altitude);
          Serial.println(" m");
          state = OFF;  //go to off state
        }
    }
break;



//=========================================================================================================================================================================================
//In OFF state sampling has been turned off and will end the flight if the time has been long enough
      
      case OFF:
        current_time = millis();
        //Check if balloon has logically been in flight long enough to be at this state
        if ((current_time - start_time) >= projected_flight_time) {
          Serial.println("Staying in Off State");
            //Turn off sensing
            //stop all heating commands
            //stop sensing by sending no commands
        }
        else {  /*if enough time has not passed to make sense for the balloon to be in the last part of 
                descent it will go back to the descent state and continue taking data*/
            Serial.println("Going back to DESCENT");
            state = DESCENT;
        }
      break;
  }
  
}
