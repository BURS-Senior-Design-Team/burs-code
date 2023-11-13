#include "Arduino.h"
#include "sd_card.h"


sd_cardClass::sd_cardClass(){
}

void sd_cardClass::writeTime(unsigned long current_time){
  Serial.print(current_time);
  Serial.print(" msec, ");
}

void sd_cardClass::writeAltitude(long altitude){
  Serial.print(altitude);
  Serial.print(" ft, ");
}

void sd_cardClass::writeUVA(long UVA){
  Serial.print(UVA);
  Serial.print(" w/cm^2, ");
}

void sd_cardClass::writeUVB(long UVB){
  Serial.print (UVB);
  Serial.print(" w/cm^2, ");
}

void sd_cardClass::writeUVC(long UVC){
  Serial.print (UVC);
  Serial.print(" w/cm^2, ");
}

void sd_cardClass::writeExternalTemp(long ExternalTemp){
  Serial.print (ExternalTemp);
  Serial.print (" degrees F,");
}

void sd_cardClass::writeSystemTemp(long SystemTemp){
  Serial.print (SystemTemp);
  Serial.print (" degrees F, ");
}
void writeScientific(unsigned long current_time, long altitude, long UVA, long UVB, long UVC){
  Serial.print(current_time);
  Serial.print(" msec, ");
  Serial.print(altitude);
  Serial.print(" ft, ");
  Serial.print(UVA);
  Serial.print(" w/cm^2, ");
  Serial.print (UVB);
  Serial.print(" w/cm^2, ");
  Serial.print (UVC);
  Serial.print(" w/cm^2, ");
}

sd_cardClass sd = sd_cardClass();
