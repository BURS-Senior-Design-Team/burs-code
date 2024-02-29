#include <Wire.h>
#include <LTR390.h>


 LTR390 ltr390 = LTR390();

void setup() {
   Serial.begin(115200);
  Wire.begin();
  if(!ltr390.init()){
    Serial.println("UV_A Not Connected");
  }
  else{
    Serial.println("UV_A Connected");
    ltr390.setMode(LTR390_MODE_UVS);
  }

}

void loop() {


         Serial.print("UV Index: "); 
         Serial.println(ltr390.getUVI());

      
  
}
