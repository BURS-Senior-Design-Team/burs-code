#include "Arduino.h"
#include "sensor.h"

SensorClass::SensorClass(){
}

//function to get the uva measurement
long SensorClass::get_UVA(int test_it, long demo_uv_a[], long uv_a){
  uv_a = demo_uv_a[test_it];
  return uv_a;
}

//function to get the uvb measurement
long SensorClass::get_UVB(int test_it, long demo_uv_b[], long uv_b){
  uv_b = demo_uv_b[test_it];
  return uv_b;
}

//function to get the uvc measurement
long SensorClass::get_UVC(int test_it, long demo_uv_c[], long uv_c){
  uv_c = demo_uv_c[test_it];
  return uv_c;
}

//function to get the altitude
long SensorClass::get_Altitude(long testAltitude){
  long altitude = testAltitude;
  Serial.print(altitude);
  Serial.print(", ");
  return altitude;
}

//function to get the internal temperature
long SensorClass::get_int_temp(int test_it, long demo_int_temp[], long int_temp){
  int_temp = demo_int_temp[test_it];
  return int_temp;
}

//function to get the external temperature
long SensorClass::get_ext_temp(int test_it, long demo_ext_temp[], long ext_temp){
  ext_temp = demo_ext_temp[test_it];
  return ext_temp;
}

//function to get the current time
unsigned long SensorClass::get_time(unsigned long long current_time){
  current_time = millis();
  return current_time;
}

SensorClass Sensor = SensorClass();
