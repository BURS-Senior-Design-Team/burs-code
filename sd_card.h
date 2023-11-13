#ifndef sd_card_h
#define sd_card_h

class sd_cardClass
{
  public:
    sd_cardClass();
    void writeTime(unsigned long current_time);
    void writeAltitude(long altitude);
    void writeUVA(long UVA);
    void writeUVB(long UVB);
    void writeUVC(long UVC);
    void writeExternalTemp(long ExternalTemp);
    void writeSystemTemp(long SystemTemp);
    void writeScientific(unsigned long current_time, long altitude, long UVA, long UVB, long UVC);
    
};

extern sd_cardClass sd;

#endif
