#ifndef sd_card_h
#define sd_card_h

#include "sensor.h"

class sd_cardClass
{
  public:
    sd_cardClass();
    void writeTime();
    void writeAltitude();
    void writeUVA();
    void writeUVB();
    void writeUVC();
    void writeExternalTemp();
    void writeInternalTemp();
    void writeScientific();
    void writeHouseKeeping();
    
};

extern sd_cardClass sd;

#endif
