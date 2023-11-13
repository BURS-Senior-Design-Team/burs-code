#ifndef led_h
#define led_h

class ledClass
{
  public:
    ledClass();
    void SETUP();
    void BLINK();
    void OFF();
};

extern ledClass led;


#endif
