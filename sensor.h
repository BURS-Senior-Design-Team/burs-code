#ifndef sensor_h
#define sensor_h

class SensorClass
{
  public:
    SensorClass();
    long get_UVA(int test_it, long demo_uv_a[], long uv_a);
    long get_UVB(int test_it, long demo_uv_b[], long uv_b);
    long get_UVC(int test_it, long demo_uv_c[], long uv_c);
    long get_Altitude(long testAltitude);
    long get_int_temp(int test_it, long demo_int_temp[], long int_temp);
    long get_ext_temp(int test_it, long demo_ext_temp[], long ext_temp);
    unsigned long get_time(unsigned long long current_time);
    
};

extern SensorClass Sensor;


#endif
