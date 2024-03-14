#include "uvb.h"

//Create UVB object
UVB uvb = UVB();

void setup(){
    // start the UART
    Serial.begin(115200);
    Wire.begin();
    
    Serial.println("----- UV-B Test -----");

    // initialize the UVB Sensor with default paramenters
    // Range X1, 100ms integration time
    if (!uvb.init()){
        Serial.println("Can't Find UVB");
    }
    else{
      Serial.println("UVB Connected");
    }
}

void loop(){

    // print the data
    Serial.print("UVB Count: ");
    Serial.print(uvb.readUVB());
    Serial.println(" count");

    delay(3000); // wait some time
}
