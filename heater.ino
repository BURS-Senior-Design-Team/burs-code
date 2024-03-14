#include <RTClib.h>

RTC_DS3231 rtc;


void setup () {
  Serial.begin(9600);

  //I connected the voltage switch to pin 9 in testing, feel free to change
  pinMode(9, OUTPUT);
  //SET PIN TO LOW THIS TURNS HEATER ***OFF***
  digitalWrite(9, LOW);
  

  // SETUP RTC MODULE
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1);
  }
}

void loop () {
  

  //prints temp for testing, feel free to remove
  Serial.println(rtc.getTemperature(), DEC);

  //if temp is less then threshhold temp
  if( rtc.getTemperature() < 25)
  {
    //heater turns on
    digitalWrite(9, HIGH);
  }

//if temp is greater then threshhold
if( rtc.getTemperature() > 25)
  {
    //heater turns off
    digitalWrite(9, LOW);
  }
  

}
