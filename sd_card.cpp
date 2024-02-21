#include "Arduino.h"
#include "sd_card.h"


sd_cardClass::sd_cardClass(){
}

void sd_cardClass::writeTime(){
  Serial.print(sensor.current_time);
  Serial.print(" msec, ");
}

void sd_cardClass::writeAltitude(){
  Serial.print(sensor.altitude);
  Serial.print(" m, ");
}

void sd_cardClass::writeUVA(){
  Serial.print(sensor.uv_a);
  Serial.print(" w/m^2, ");
}

void sd_cardClass::writeUVB(){
  Serial.print (sensor.uv_b);
  Serial.print(" w/m^2, ");
}

void sd_cardClass::writeUVC(){
  Serial.print (sensor.uv_c);
  Serial.print(" w/m^2, ");
}

void sd_cardClass::writeExternalTemp(){
  Serial.print (sensor.external_temp);
  Serial.print (" degrees C,");
}

void sd_cardClass::writeInternalTemp(){
  Serial.print (sensor.internal_temp);
  Serial.print (" degrees C, ");
}
void sd_cardClass::writeScientific(){
  Serial.print("Scientific Data: ");
  writeTime();
  writeAltitude();
  writeUVA();
  Serial.println();
}

void sd_cardClass::writeHouseKeeping(){
  Serial.print("HouseKeeping Data: ");
  writeTime();
  writeInternalTemp();
  Serial.println();
}
sd_cardClass sd = sd_cardClass();
