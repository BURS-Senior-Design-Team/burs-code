void writeTime(unsigned long current_time){
  Serial.print (current_time);
  Serial.print (',');
}

void writeAltitude(double altitude){
  Serial.print (altitude);
  Serial.print (',');
}

void writeUVA(double UVA){
  Serial.print (UVA);
  Serial.print (',');
}

void writeUVB(double UVB){
  Serial.print (UVB);
  Serial.print (',');
}

void writeUVC(double UVC){
  Serial.print (UVC);
  Serial.print (',');
}

void writeExternalTemp(double ExternalTemp){
  Serial.print (ExternalTemp);
  Serial.print (',');
}

void writeSystemTemp(double SystemTemp){
  Serial.print (SystemTemp);
  Serial.print (',');
}
