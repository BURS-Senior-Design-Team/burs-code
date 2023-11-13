#include "Arduino.h"
#include "led.h"

const uint8_t led_pin = 8;
int off = 1;

ledClass::ledClass(){
}

void ledClass::SETUP(){
  pinMode(led_pin, OUTPUT);
}

void ledClass::BLINK(){
  
  if(off == 1){
    digitalWrite(led_pin, HIGH);
    off = 0;
  }
  
  else{
    digitalWrite(led_pin, LOW);
    off = 1;
  }
}

void ledClass::OFF(){
  digitalWrite(led_pin, LOW);
}
