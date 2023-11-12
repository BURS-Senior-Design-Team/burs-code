#include "sensor.h"
#include "sd_card.h"

enum State {ARM, UV, FLOAT, DES, OFF};  // Define the states
State state = ARM;  // Start in the OFF state

long testAltitude[25];  //Array to populate altitudes
long demo_uv_a[25];
long demo_uv_b[25];
long demo_uv_c[25];
long demo_int_temp[25];
long demo_ext_temp[25];
int test_it = 0;  //iterator for testing state machine

//Recorded Data Points
unsigned long start_time;
unsigned long current_time;
unsigned long timer;
const unsigned long timer_check = 4000; //used for checking period of descent time set to 4 seconds for now
const unsigned long flight_time = 10000; //used for ensuring balloon has been in flight for more enough time to have reached the 10,000 foot descent altitude for turning the system off 12hrs = 43,200,000
double altitude=0;
double uv_a;
double uv_b;
double uv_c;
double SystemTemp;
double ExternalTemp;
double heat_cont;

int sample_rate = 1;//hz

void setup() {

Serial.begin(115200);
start_time = millis();

//pinhigh for sensor package transistor gate
//initialize led function

//creating arrays for testing purposes
for(int i=0; i<=2; i++){
  testAltitude[i]=0;
  demo_uv_a[i] = 1000;
  demo_uv_b[i] = 10;
  demo_uv_c[i] = 0;
  demo_int_temp[i] = 65;
  demo_ext_temp[i] = 65;
}
for(int i=3; i<=10; i++){
  testAltitude[i]=i*10000;
  demo_uv_a[i] = 1000;
  demo_uv_b[i] = 10;
  demo_uv_c[i] = 0;
  demo_int_temp[i] = (65 - (i*9));
  demo_ext_temp[i] = (65 - (i*9));
  }
for(int i=10; i<=15; i++){
  testAltitude[i]=100000;
  demo_uv_a[i] = 1000;
  demo_uv_b[i] = 10;
  demo_uv_c[i] = 0;
  demo_int_temp[i] = -25;
  demo_ext_temp[i] = -25;
  }
for(int i=16; i<=20; i++){
  testAltitude[i]= (100000 - (i*1000));
  demo_uv_a[i] = 1000;
  demo_uv_b[i] = 10;
  demo_uv_c[i] = 0;
  demo_int_temp[i] = (-25 + (i * 4.5));
  demo_ext_temp[i] = (-25 + (i *4.5));
  }
for(int i=20; i<=25; i++){
  testAltitude[i]=5000;
  demo_uv_a[i] = 1000;
  demo_uv_b[i] = 10;
  demo_uv_c[i] = 0;
  demo_int_temp[i] = 65;
  demo_ext_temp[i] = 65;
  }
Serial.print ('In Arm State');
}

void loop() {

  switch (state) {

    //In ARM state sample rate = 1hz, recording altitude and time to SD
    //will change to UV state when altitude reaches 100ft
    case ARM:

      sample_rate = 1;//hz
      //handle scientific data
      if(current_time % ((1/sample_rate)*1000)){
        get_time(current_time);
        get_UVA(test_it, demo_uv_a, uv_a);
        get_UVB(test_it, demo_uv_b, uv_b);
        get_UVC(test_it, demo_uv_c, uv_c);
        get_Altitude(test_it, testAltitude, altitude);
        writeTime(current_time);
        writeAltitude(altitude);
        writeUVA(uv_a);
        writeUVB(uv_b);
        writeUVC(uv_c);
        test_it++;
      }

        //turn on LED
        
      if (altitude >= 100) {
        Serial.print ('Going to UV State,');
        state = UV;
      }
      
      break;


    //In UV state sample rate = 4hz, recording UV data, altitude, time, external temp, internal temp, and heater control voltage
    //will change to Float state when reaching ceiling of 90kft
    case UV:

      sample_rate = 4;//hz
      
      //handle scientific data
      if(current_time % ((1/sample_rate)*1000)){
        get_time(current_time);
        get_UVA(test_it, demo_uv_a, uv_a);
        get_UVB(test_it, demo_uv_b, uv_b);
        get_UVC(test_it, demo_uv_c, uv_c);
        get_Altitude(test_it, testAltitude, altitude);
        writeTime(current_time);
        writeAltitude(altitude);
        writeUVA(uv_a);
        writeUVB(uv_b);
        writeUVC(uv_c);
        test_it++;
      }

      //record internal temp
      //record external temp
      //record heater voltage
      
      if (altitude >= 90000) {
        Serial.print('Going to Float State');
        state = FLOAT;
      }

      break;


    //In Float state sample rate = 1hz
    //recording UVdata, altitude, time, external temp, internal temp and heater control voltage
    //will change to DES state once we are below 90kft
    case FLOAT:

      sample_rate = 1;//hz
      
      //handle scientific data
      if(current_time % ((1/sample_rate)*1000)){
        get_time(current_time);
        get_UVA(test_it, demo_uv_a, uv_a);
        get_UVB(test_it, demo_uv_b, uv_b);
        get_UVC(test_it, demo_uv_c, uv_c);
        get_Altitude(test_it, testAltitude, altitude);
        writeTime(current_time);
        writeAltitude(altitude);
        writeUVA(uv_a);
        writeUVB(uv_b);
        writeUVC(uv_c);
        test_it++;
      }

      if (altitude < 90000){
        Serial.print('Checking if Descending,');//for demo
        current_time = millis();  //if the altitude is below 90000 feet check the time
        
        while(altitude < 90000){  //will stay in this loop until 2 of the 3 conditions are met
          //handle scientific data
          if(current_time % ((1/sample_rate)*1000)){
            get_time(current_time);
            get_UVA(test_it, demo_uv_a, uv_a);
            get_UVB(test_it, demo_uv_b, uv_b);
            get_UVC(test_it, demo_uv_c, uv_c);
            get_Altitude(test_it, testAltitude, altitude);
            writeTime(current_time);
            writeAltitude(altitude);
            writeUVA(uv_a);
            writeUVB(uv_b);
            writeUVC(uv_c);
            test_it++;
          }
          
          timer = millis();//start a timer for checking if we are actually descending
          if((timer - current_time >= timer_check) && (altitude < 90000)){  //the balloon is below 90000 feet and has been for more than 5 seconds
            state = DES;    //change the state to the descent state
            break;          //exits the loop and will be in descent state next loop
          }
          else if((timer - current_time <= timer_check) && (altitude < 90000)){ //balloon is still below 90000 feet but has not been for more than 5 seconds
            //will remain in the while loop and continue to check the time and altitude
          }
          else{           //the altitude moves back up above 90000 feet(balloon is still in float)
            test_it++;
            break;  //exits while loop and remains in float state
          }
            
        }
      }
      break;


      //In DES state sample rate = 16Hz, recording UVdata, altitude, time, external temp, internal temp and heater control voltage
      //will change to OFF state once we are below 10kft
      case DES:

      sample_rate=16; //increase the sampling rate for the descent portion

      //handle scientific data
      if(current_time % ((1/sample_rate)*1000)){
        get_time(current_time);
        get_UVA(test_it, demo_uv_a, uv_a);
        get_UVB(test_it, demo_uv_b, uv_b);
        get_UVC(test_it, demo_uv_c, uv_c);
        get_Altitude(test_it, testAltitude, altitude);
        writeTime(current_time);
        writeAltitude(altitude);
        writeUVA(uv_a);
        writeUVB(uv_b);
        writeUVC(uv_c);
        test_it++;
      }
      
      if (altitude <= 10,000) {
        //turn off all sampling
        //turn off all recording
        state = OFF;
      }
      break;

      //In OFF state sampling has been turned off and will end the flight if the time has been long enough
      case OFF:

      current_time = millis();
      if (current_time - start_time>=flight_time) {
        //power down transistor and shuts power off;
      }
      else {  //if enough time has not passed to make sense for the balloon to be in the last part of descent it will go back to the descent state and continue taking data
        state = DES;
        //turn sensing transistor back on
      }
      break;
  }
}
