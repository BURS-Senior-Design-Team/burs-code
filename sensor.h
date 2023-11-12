//function to get the uva measurement
double get_UVA(int test_it, long demo_uv_a[], double uv_a){
  return uv_a = demo_uv_a[test_it];
}

//function to get the uvb measurement
double get_UVB(int test_it, long demo_uv_b[], double uv_b){
  return uv_b = demo_uv_b[test_it];
}

//function to get the uvc measurement
double get_UVC(int test_it, long demo_uv_c[], double uv_c){
  return uv_c = demo_uv_c[test_it];
}

//function to get the altitude
double get_Altitude(int test_it, long testAltitude[], unsigned long altitude){
  return altitude = testAltitude[test_it];
}

//function to get the internal temperature
double get_int_temp(int test_it, long demo_int_temp[], double int_temp){
  return int_temp = demo_int_temp[test_it];
}

//function to get the external temperature
double get_ext_temp(int test_it, long demo_ext_temp[], double ext_temp){
  return ext_temp = demo_ext_temp[test_it];
}

//function to get the current time
unsigned long get_time(unsigned long current_time){
  return current_time = millis();
}
