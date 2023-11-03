enum State {ARM, UV, FLOAT, DES, OFF};  // Define the states

State state = ARM;  // Start in the OFF state

void setup() {
//pinhigh for transistor gate
//initialize led function
int sample_rate = 1hz;
float altitdue;
float time_;
}

void loop() {

  switch (state) {

    //In ARM state sample rate = 1hz
    //recording altitude and time to SD
    //will change to UV state when altitude reaches 100ft
    case ARM:
      altitude = //digitalread altimeter;
      time_ = //digitalread altimeter;
      //start logging time in sd-card
      //start logging altitude in sd-card
      //turn on LED
        
      if (altitude >= 100) {
        //change sample rate to 4hz
        state = UV;
      }
      break;

    //In UV state sample rate = 4hz
    //recording UV data, altitude, time, external temp, internal temp, and heater control voltage
    //will change to Float state when reaching ceiling of 90kft
    case UV:
      //record time
      //record UV-A
      //record UV-B
      //record UV-C
      //record internal temp
      //record external temp
      //record heater voltage
      if (altitude >= 90,000) {
        sample_rate = 1hz;
        state = FLOAT;
      }
      break;

    //In Float state sample rate = 1hz
    //recording UVdata, altitude, time, external temp, internal temp and heater control voltage
    //will change to DES state once we are below 90kft
    case Float
      float local_altitude = //read altitude
      local_altitude = (altitude + local_altitude)/2;
      while (altitude < 90,000) {
        //start descent timer
        if(timer <5 mins){
          state = DES;
        }
      }
      break;

      //In DES state sample rate = 8Hz
      //recording UVdata, altitude, time, external temp, internal temp and heater control voltage
      //will change to OFF state once we are below 10kft
      case DES:
      if (altitude <= 10,000) {
        //turn off all sampling
        //turn off all recording
        state = OFF;
      }
      break;

      
      case OFF:
      if (time_>=12hrs) {
        //power down transistor and shuts power off;
      }
      else {
        state = DES;
      }
      break;
  }
}
