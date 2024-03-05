#include "uva.h"

//Create uva object
UVAClass uva = UVAClass();

void setup() {
  Serial.begin(115200);
  Wire.begin();
  //Test uva connection
  Serial.println("----- UV-A Test -----");
  
  if(!uva.init()){
    Serial.println("Failed to Connect to UVA");
  }
  
  else{
    Serial.println("UVA Connected");
  }
 

}

void loop() {

  Serial.print ("UVA = ");
  Serial.print (uva.readUVS());
  Serial.println(" UV Count");
   
  delay(3000);

}
