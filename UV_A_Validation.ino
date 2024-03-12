#include "uva.h"

//Create uva object
UVA uva = UVA();

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  Serial.println("----- UV-A Test -----");

  //Test uva connection
  if(!uva.init()){
    Serial.println("Failed to Connect to UVA");
  }
  
  else{
    Serial.println("UVA Connected");
  }
}

void loop() {

  Serial.print ("UVA = ");
  Serial.print (uva.readUVA());
  Serial.println(" UV Count");
   
  delay(3000);

}
