//#include "./i2c.h"
#define  ADS1115_ADDR    0x48

void read_joysticks() {
    uint8_t tmp_str[2];
    int adc1, adc2, adc3, adc4;
    int jmin, jmax, jcenter, jhyst;
    while(1) {
       tmp_str[0] = 0x40; tmp_str[1] = 0x80;  
       i2c_write_block( ADS1115_ADDR, 0x01, tmp_str, 2); 
       vTaskDelay(2);
       i2c_read (ADS1115_ADDR, 0x00, tmp_str, 2);
       i2c_read (ADS1115_ADDR, 0x00, tmp_str, 2);
       adc2 = (256 * tmp_str[0] + tmp_str[1]);

       tmp_str[0] = 0x50; tmp_str[1] = 0x80;  
       i2c_write_block( ADS1115_ADDR, 0x01, tmp_str, 2); 
       vTaskDelay(2);
       i2c_read (ADS1115_ADDR, 0x00, tmp_str, 2);
       i2c_read (ADS1115_ADDR, 0x00, tmp_str, 2);
       adc1 = (256 * tmp_str[0] + tmp_str[1]) ;

       tmp_str[0] = 0x60; tmp_str[1] = 0x80;  
       i2c_write_block( ADS1115_ADDR, 0x01, tmp_str, 2); 
       vTaskDelay(2);
       i2c_read (ADS1115_ADDR, 0x00, tmp_str, 2);
       i2c_read (ADS1115_ADDR, 0x00, tmp_str, 2);
       adc4 = (256 * tmp_str[0] + tmp_str[1]);

       tmp_str[0] = 0x70; tmp_str[1] = 0x80;  
       i2c_write_block( ADS1115_ADDR, 0x01, tmp_str, 2); 
       vTaskDelay(2);
       i2c_read (ADS1115_ADDR, 0x00, tmp_str, 2);
       i2c_read (ADS1115_ADDR, 0x00, tmp_str, 2);
       adc3 = (256 * tmp_str[0] + tmp_str[1]) ;

       jmin = 0x0000; jmax = 0x4400; jcenter = 0x21d0; jhyst = 0x0040; 
       if (adc1 >= jmin && adc1 <= (jcenter-jhyst)) 
	       throttle = 1000 + 500 * (adc1 - jmin) / ((jcenter-jhyst) - jmin); 
       else
       if (adc1 <= jmax && adc1 >=(jcenter+jhyst)) 
	       throttle = 1500 + 500 * (adc1 - (jcenter+jhyst)) / (jmax - (jcenter+jhyst)); 
       else throttle = 1500; 

       jmin = 0x0000; jmax = 0x4400; jcenter = 0x21d0; jhyst = 0x0040; 
       if (adc2 >= jmin && adc2 <= (jcenter-jhyst)) 
	       yaw = 2000 - 500 * (adc2 - jmin) / ((jcenter-jhyst) - jmin); 
       else
       if (adc2 <= jmax && adc2 >=(jcenter+jhyst)) 
	       yaw = 1500 - 500 * (adc2 - (jcenter+jhyst)) / (jmax - (jcenter+jhyst)); 
       else yaw = 1500; 

       jmin = 0x0000; jmax = 0x4400; jcenter = 0x2200; jhyst = 0x0040; 
       if (adc3 >= jmin && adc3 <= (jcenter-jhyst)) 
	       pitch = 1000 + 500 * (adc3 - jmin) / ((jcenter-jhyst) - jmin); 
       else
       if (adc3 <= jmax && adc3 >=(jcenter+jhyst)) 
	       pitch = 1500 + 500 * (adc3 - (jcenter+jhyst)) / (jmax - (jcenter+jhyst)); 
       else pitch = 1500; 

       jmin = 0x0000; jmax = 0x4400; jcenter = 0x2170; jhyst = 0x0040; 
       if (adc4 >= jmin && adc4 <= (jcenter-jhyst)) 
	       roll = 1000 + 500 * (adc4 - jmin) / ((jcenter-jhyst) - jmin); 
       else
       if (adc4 <= jmax && adc4 >=(jcenter+jhyst)) 
	       roll = 1500 + 500 * (adc4 - (jcenter+jhyst)) / (jmax - (jcenter+jhyst)); 
       else roll = 1500; 

       //printf(" 0x%04x  0x%04x  0x%04x  0x%04x    ", adc1, adc2, adc3, adc4);
       //printf(" %4d  %4d  %4d  %4d\n", throttle, yaw, pitch, roll);

       vTaskDelay(50);
   }
}

