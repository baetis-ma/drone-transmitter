//#include "./i2c.h"
#define  ADS1115_ADDR    0x48

void crap_joysticks() {
    uint8_t tmp_str[2];
    int adc1, adc2, adc3, adc4;
    int jmin, jmax, jcenter, jhyst;
    int tthrottle, tyaw, tpitch, troll;
    while(1) {
       //read adcs
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

       //factor out crappiness of joysticks - hysterisis min and max
       jmin = 0x0000; jmax = 0x4400; jcenter = 0x2220; jhyst = 0x0040; 
       if (adc1 >= jmin && adc1 <= (jcenter-jhyst)) 
	       tthrottle = 1000 + 500 * (adc1 - jmin) / ((jcenter-jhyst) - jmin); 
       else
       if (adc1 <= jmax && adc1 >=(jcenter+jhyst)) 
	       tthrottle = 1500 + 500 * (adc1 - (jcenter+jhyst)) / (jmax - (jcenter+jhyst)); 
       else if (adc1 > jmax) tthrottle = 2000;
       else tthrottle = 1500; 

       jmin = 0x0000; jmax = 0x4400; jcenter = 0x2220; jhyst = 0x0040; 
       if (adc2 >= jmin && adc2 <= (jcenter-jhyst)) 
	       tyaw = 2000 - 500 * (adc2 - jmin) / ((jcenter-jhyst) - jmin); 
       else
       if (adc2 <= jmax && adc2 >=(jcenter+jhyst)) 
	       tyaw = 1500 - 500 * (adc2 - (jcenter+jhyst)) / (jmax - (jcenter+jhyst)); 
       else if (adc2 > jmax) tyaw = 1000;
       else tyaw = 1500; 

       jmin = 0x0000; jmax = 0x4400; jcenter = 0x2240; jhyst = 0x0040; 
       if (adc3 >= jmin && adc3 <= (jcenter-jhyst)) 
	       tpitch = 1000 + 500 * (adc3 - jmin) / ((jcenter-jhyst) - jmin); 
       else
       if (adc3 <= jmax && adc3 >=(jcenter+jhyst)) 
	       tpitch = 1500 + 500 * (adc3 - (jcenter+jhyst)) / (jmax - (jcenter+jhyst)); 
       else if (adc3 > jmax) tpitch = 2000;
       else tpitch = 1500; 

       jmin = 0x0000; jmax = 0x4400; jcenter = 0x2220; jhyst = 0x0040; 
       if (adc4 >= jmin && adc4 <= (jcenter-jhyst)) 
	       troll = 1000 + 500 * (adc4 - jmin) / ((jcenter-jhyst) - jmin); 
       else
       if (adc4 <= jmax && adc4 >=(jcenter+jhyst)) 
	       troll = 1500 + 500 * (adc4 - (jcenter+jhyst)) / (jmax - (jcenter+jhyst)); 
       else if (adc4 > jmax) troll = 2000;
       else troll = 1500; 

       //filter attitude values
       if (throttle <  1080) throttle = throttle + 0.10 * (tthrottle - 1500);
                        else throttle = throttle + 0.03 * (tthrottle - 1500);
       if (tthrottle == 1000) throttle = 1000; 
       if (throttle < 1000) throttle = 1000; if (throttle > 2000) throttle = 2000; //spikey should debounce

       if (tyaw == 1500) yaw = 1500; else yaw = yaw + 0.06 * (tyaw - 1500);
       if (yaw < 1000) yaw = 1000; if (yaw > 2000) yaw = 2000;

       if (tpitch == 1500) pitch = 1500; else pitch = pitch + 0.03 * (tpitch - 1500);
       if (pitch < 1000) pitch = 1000; if (pitch > 2000) pitch = 2000;

       if (troll == 1500) roll = 1500; else roll = roll + 0.03 * (troll - 1500);
       if (roll < 1000) roll = 1000; if (roll > 2000) roll = 2000;

       printf(" 0x%04x  0x%04x  0x%04x  0x%04x    ", adc1, adc2, adc3, adc4);
       printf(" %4d  %4d  %4d  %4d    ", tthrottle, tyaw, tpitch, troll);
       printf(" %4d  %4d  %4d  %4d\n", throttle, yaw, pitch, roll);

       vTaskDelay(50);
   }
}

