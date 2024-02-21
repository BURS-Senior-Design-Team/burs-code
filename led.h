#ifndef led_h
#define led_h
#include "sensor.h"

class ledClass
{
  public:
    ledClass();
    void SETUP(); //set up pinout for led functionality
    void LED_Indicator(); //turns on leds based on following functions protocalls
    void RTC_and_InternalTemp_LED(); //turn on led if rtc is connected
    void Altimeter_LED();  //turn on led if altimeter is connected       
    void UV_A_LED();  //turn on led if UV-A is connected
    void blink_LED(const uint8_t PIN, int STATE);
    void OFF();

    //Variables used for blinking led
    bool uv_a_led_on = false;
    bool rtcANDinternaltemp_led_on = false;
    bool altimeter_led_on = false;
    
  private:
    const uint8_t RTC_and_InternalTemp_LED_PIN = 2;
    const uint8_t Altimeter_LED_PIN = 3;
    const uint8_t UV_A_LED_PIN = 4;
};

extern ledClass led;


#endif
