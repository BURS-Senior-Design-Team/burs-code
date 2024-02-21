#include "Arduino.h"
#include "led.h"

int off = 1;

ledClass::ledClass(){
}


//Function to setup connection to led pins
  void ledClass::SETUP(){
      pinMode(RTC_and_InternalTemp_LED_PIN, OUTPUT);
      pinMode(Altimeter_LED_PIN, OUTPUT);
      pinMode(UV_A_LED_PIN, OUTPUT);
}



//Function to turn all leds on
//
void ledClass::LED_Indicator(){

      RTC_and_InternalTemp_LED();
      Altimeter_LED();
      UV_A_LED();
      
}

//Function to turn on led for rtc if connected or flash if not connected
//
void ledClass::RTC_and_InternalTemp_LED(){

      //turn on solid if connected
      if(sensor.RTCandInternalTemp_connected == true){
          digitalWrite(RTC_and_InternalTemp_LED_PIN, HIGH);
      }
      
      //blink led if not connected
      else{
          if(rtcANDinternaltemp_led_on == true && sensor.trigger == true){
            blink_LED(RTC_and_InternalTemp_LED_PIN, LOW);
            rtcANDinternaltemp_led_on = false;
          }
          else if (rtcANDinternaltemp_led_on == false && sensor.trigger == true) {
            blink_LED(RTC_and_InternalTemp_LED_PIN, HIGH);
            rtcANDinternaltemp_led_on = true;
          }
          else{}
      } 
}




//Function to turn on led if altimeter is connected
//
void ledClass::Altimeter_LED(){
    
    if(sensor.Altimeter_connected == true){
          digitalWrite(Altimeter_LED_PIN, HIGH);
    }
    else{
      if(altimeter_led_on == true && sensor.trigger == true){
        blink_LED(Altimeter_LED_PIN, LOW);
        altimeter_led_on = false;
      }
      else if(altimeter_led_on == false && sensor.trigger == true){
        blink_LED(Altimeter_LED_PIN, HIGH);
        altimeter_led_on = true;
      }
      else{}
    }
    
}



//Function to turn on led if UV-A sensor is connected
//
void ledClass::UV_A_LED(){
    if(sensor.UV_A_connected == true){
        digitalWrite(UV_A_LED_PIN, HIGH);
    }
    
    else{
      if(uv_a_led_on == true && sensor.trigger == true){
        blink_LED(UV_A_LED_PIN, LOW);
        uv_a_led_on = false;
      }
      else if(uv_a_led_on == false && sensor.trigger == true){
        blink_LED(UV_A_LED_PIN, HIGH);
        uv_a_led_on = true;
      }
    }
}


//Function to blink led if there is an issue with connection to a sensor
//
void ledClass::blink_LED(const uint8_t PIN, int STATE){
        digitalWrite(PIN, STATE);
}



//Function to turn of all LEDs
void ledClass::OFF(){
  digitalWrite(RTC_and_InternalTemp_LED_PIN, LOW);
}
ledClass led = ledClass();
