#include "sensor.h"
#include "sd_card.h"
#include "led.h"

enum State {ARM, UV, FLOAT, DES, OFF};  // Define the states
State state = ARM;  // Start in the OFF state

long testAltitude[25];  //Array to populate altitudes
long demo_uv_a[25];
long demo_uv_b[25];
long demo_uv_c[25];
long demo_int_temp[25];
long demo_ext_temp[25];
int test_it;  //iterator for testing state machine

//Recorded Data Points
unsigned long start_time;
unsigned long period_timer;
unsigned long current_time;
unsigned long timer;
const unsigned long timer_check = 3000; //used for checking period of descent time set to 4 seconds for now
const unsigned long flight_time = 5000; //used for ensuring balloon has been in flight for more enough time to have reached the 10,000 foot descent altitude for turning the system off 12hrs = 43,200,000
long altitude;
long uv_a;
long uv_b;
long uv_c;
long SystemTemp;
long ExternalTemp;
long heat_cont;

double sample_rate = 1;//hz
double period;

void setup() {

Serial.begin(115200);
start_time = Sensor.get_time(current_time);
period_timer = start_time;
led.SETUP();//set up for LED blinking

//creating arrays for testing purposes
for(int i=0; i<=2; i++){  //on the ground
  testAltitude[i]=0;
  demo_uv_a[i] = 800;
  demo_uv_b[i] = 10;
  demo_uv_c[i] = 0;
  demo_int_temp[i] = 65;
  demo_ext_temp[i] = 65;
}
for(int i=3; i<=10; i++){ //ascending
  testAltitude[i]=i*10000;
  demo_uv_a[i] = 800 + (i*10);
  demo_uv_b[i] = 10 + (i*10);
  if(i >=6){
    demo_uv_c[i] = 0 + (i*10);
  }
  else{
    demo_uv_c[i] = 0;
  }
  demo_int_temp[i] = (65 - (i*9));
  demo_ext_temp[i] = (65 - (i*9));
  }
for(int i=11; i<=15; i++){ //floating
  testAltitude[i]=100000;
  demo_uv_a[i] = demo_uv_a[10];
  demo_uv_b[i] = demo_uv_b[10];
  demo_uv_c[i] = demo_uv_c[10];
  demo_int_temp[i] = -25;
  demo_ext_temp[i] = -25;
  }
for(int i=16; i<=20; i++){ //descending
  testAltitude[i]= (100000 - (i*1000));
  demo_uv_a[i] = demo_uv_a[10]-((i-10)*10);
  demo_uv_b[i] = demo_uv_b[10]-((i-10)*10);
  demo_uv_c[i] = demo_uv_c[10]-((i-10)*10);
  demo_int_temp[i] = (-25 + (i * 4.5));
  demo_ext_temp[i] = (-25 + (i *4.5));
  }
for(int i=21; i<=25; i++){ //below 10000 feet
  testAltitude[i]=5000;
  demo_uv_a[i] = 800;
  demo_uv_b[i] = 10;
  demo_uv_c[i] = 0;
  demo_int_temp[i] = 65;
  demo_ext_temp[i] = 65;
  }
Serial.print ("ARM STATE, ");
test_it = 0;
altitude = 0;
}

void loop() {

  switch (state) {

    //In ARM state sample rate = 1hz
    //will change to UV state when altitude reaches 100ft
    case ARM:

      sample_rate = 1;//hz
      period = 1/sample_rate;
      current_time = Sensor.get_time(current_time);

      //handle scientific data
      if((current_time - period_timer) >= (period*1000)){
        led.BLINK();
        period_timer = current_time;
        altitude = testAltitude[test_it];
        ExternalTemp = demo_ext_temp[test_it];
        uv_a = demo_uv_a[test_it];
        uv_b = demo_uv_b[test_it];
        uv_c = demo_uv_c[test_it];
        sd.writeTime(current_time);
        sd.writeExternalTemp(ExternalTemp);
        sd.writeAltitude(altitude);
        sd.writeUVA(uv_a);
        sd.writeUVB(uv_b);
        sd.writeUVC(uv_c);
        
        if (altitude >= 100) {
        delay(1000);
        Serial.print ("UV STATE, ");
        state = UV;
        }
        
        else{
          test_it = test_it + 1;
        }
      }
      
      break;


    //In UV state sample rate = 4hz, recording UV data, altitude, time, external temp, internal temp, and heater control voltage
    //will change to Float state when reaching ceiling of 90kft
    case UV:

      sample_rate = 4;//hz
      period = 1/sample_rate;
    
      current_time = Sensor.get_time(current_time);
      
      //handle scientific data
      if((current_time - period_timer) >= (period*1000)){
        led.BLINK();
        period_timer = current_time;       
        altitude = testAltitude[test_it];
        ExternalTemp = demo_ext_temp[test_it];
        uv_a = demo_uv_a[test_it];
        uv_b = demo_uv_b[test_it];
        uv_c = demo_uv_c[test_it];
        sd.writeTime(current_time);
        sd.writeExternalTemp(ExternalTemp);
        sd.writeAltitude(altitude);
        sd.writeUVA(uv_a);
        sd.writeUVB(uv_b);
        sd.writeUVC(uv_c);
        
        if (altitude >= 90000) {
          delay(1000);
          Serial.print("FLOAT STATE, ");
          test_it = test_it + 1;
          state = FLOAT;
        }

        else{
          test_it = test_it + 1;
        }
      }

      //record internal temp
      //record external temp
      //record heater voltage

      break;


    //In Float state sample rate = 1hz
    //recording UVdata, altitude, time, external temp, internal temp and heater control voltage
    //will change to DES state once we are below 90kft
    case FLOAT:

      sample_rate = 1;//hz
      period = 1/sample_rate;
      
      current_time = Sensor.get_time(current_time);
      
      //handle scientific data during normal floating operation
      if((current_time - period_timer) >= (period*1000)){
        led.BLINK();
        period_timer = current_time;       
        altitude = testAltitude[test_it];
        ExternalTemp = demo_ext_temp[test_it];
        uv_a = demo_uv_a[test_it];
        uv_b = demo_uv_b[test_it];
        uv_c = demo_uv_c[test_it];
        sd.writeTime(current_time);
        sd.writeExternalTemp(ExternalTemp);
        sd.writeAltitude(altitude);
        sd.writeUVA(uv_a);
        sd.writeUVB(uv_b);
        sd.writeUVC(uv_c);
        test_it = test_it + 1;
      }
      
      if (altitude < 90000){
        current_time = Sensor.get_time(current_time);  //if the altitude is below 90000 feet check the time
        timer = millis();//start a timer for checking if we are actually descending
        Serial.print("BELOW 90,000 FEET, ");//for demo
        
        while(altitude < 90000){  //will stay in this loop until 2 of the 3 conditions are met
            
          current_time = Sensor.get_time(current_time);

          //handle scientific data
          if((current_time - period_timer) >= (period*1000)){
            led.BLINK();
            period_timer = current_time;       
            altitude = testAltitude[test_it];
            ExternalTemp = demo_ext_temp[test_it];
        uv_a = demo_uv_a[test_it];
        uv_b = demo_uv_b[test_it];
        uv_c = demo_uv_c[test_it];
        sd.writeTime(current_time);
        sd.writeExternalTemp(ExternalTemp);
            sd.writeAltitude(altitude);
            sd.writeUVA(uv_a);
            sd.writeUVB(uv_b);
            sd.writeUVC(uv_c);
            test_it = test_it + 1;
          }
          
          if(((current_time - timer) >= timer_check) && (altitude < 90000)){  //the balloon is below 90000 feet and has been for more than 5 seconds
            delay(1000);
            Serial.print("DESCENT STATE, ");//for demo
            state = DES;    //change the state to the descent state
            break;          //exits the loop and will be in descent state next loop
          }
          
          else if(((current_time - timer) <= timer_check) && (altitude < 90000)){ //balloon is still below 90000 feet but has not been for more than 5 seconds
            //will remain in the while loop and continue to check the time and altitude
          }
          
          else{           //the altitude moves back up above 90000 feet(balloon is still in float)
            test_it = test_it + 1;
            break;  //exits while loop and remains in float state
          }
            
        }
      }
      break;


      //In DES state sample rate = 16Hz, recording UVdata, altitude, time, external temp, internal temp and heater control voltage
      //will change to OFF state once we are below 10kft
      case DES:

      sample_rate=16; //increase the sampling rate for the descent portion
      period = 1/sample_rate;

      current_time = Sensor.get_time(current_time);
      
      //handle scientific data
      if((current_time - period_timer) >= (period*1000)){
        led.BLINK();
        period_timer = current_time;       
        altitude = testAltitude[test_it];
        ExternalTemp = demo_ext_temp[test_it];
        uv_a = demo_uv_a[test_it];
        uv_b = demo_uv_b[test_it];
        uv_c = demo_uv_c[test_it];
        sd.writeTime(current_time);
        sd.writeExternalTemp(ExternalTemp);
        sd.writeAltitude(altitude);
        sd.writeUVA(uv_a);
        sd.writeUVB(uv_b);
        sd.writeUVC(uv_c);

        if (altitude <= 10000) {
        //turn off all sampling
        //turn off all recording
        delay(1000);
        Serial.print("OFF STATE, ");//for demo
        state = OFF;
        break;
        }
        else{
          test_it = test_it + 1;
        }
      }

      break;

      //In OFF state sampling has been turned off and will end the flight if the time has been long enough
      case OFF:

      current_time = Sensor.get_time(current_time);
      
      if (current_time - start_time>=flight_time) {
        delay(1000);
        Serial.print("FLIGHT COMPLETE");//for demo
        //power down transistor and shuts power off;
      }
      else {  //if enough time has not passed to make sense for the balloon to be in the last part of descent it will go back to the descent state and continue taking data
        delay(1000);
        Serial.print("DESCENT STATE, ");//for demo
        state = DES;
        //turn sensing transistor back on
      }
      break;
  }
}
