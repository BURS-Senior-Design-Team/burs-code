/******************************************************************************
https://github.com/sichel94/GUVB-C31SM 

Distributed as-is; no warranty is given.
******************************************************************************/
#include "guvbc31sm.h"

GUVB sensor;

void setup(){
    // start the UART
    Serial.begin(115200);
    Serial.println("--- GUVB-C31SM Demo ---");

    // initialize the UVB Sensor with default paramenters
    // Range X1, 100ms integration time
    if (sensor.begin() == false)
        Serial.println("Cant verify chip ID, check connection?");
}

void loop(){
    // read the values from the device
    float uvi = sensor.getUVIndex();
    float uvb = sensor.getUV();

    // print the data
    //Serial.print("UV Index: ");
    //Serial.print(uvi, 2);
    Serial.print(" eq. UVB irradiation: ");
    //Serial.print(uvb, 2);
    Serial.print(uvb);
    Serial.println(" uW/cm^2");

    delay(250); // wait some time
}