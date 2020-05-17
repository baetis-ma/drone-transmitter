#define GPIO_INPUT_SW_1     2
#define GPIO_INPUT_SW_2     0
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_SW_1) | (1ULL<<GPIO_INPUT_SW_2) )

void buttons_task () {
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
    
    char disp_str[128];
    int sw1, sw2;
    while (1) {
       sw1 = gpio_get_level(GPIO_INPUT_SW_1);  
       sw2 = gpio_get_level(GPIO_INPUT_SW_2); 
       if (sw1 == 0) {select = 1; mode++; }
       mode = mode % 8;
       if (sw2 == 0) select = 0;
       //printf (" select %d     mode %d\n", select, mode);

       if(select == 1) {
          if(mode == 0)sprintf(disp_str,"|4  Disable");
          if(mode == 1)sprintf(disp_str,"|4  Calibrate");
          if(mode == 2)sprintf(disp_str,"|4  Monitor");
          if(mode == 3)sprintf(disp_str,"|4  Joystick");
          if(mode == 4)sprintf(disp_str,"|4  Motors");
          if(mode == 5)sprintf(disp_str,"|4  Disable3");
          if(mode == 6)sprintf(disp_str,"|4  RX Timer");
          if(mode == 7)sprintf(disp_str,"|4  Flight");

       }
       if(select == 0 && mode == 0)sprintf(disp_str,"|4  Disable");
       if(select == 0 && mode == 1)sprintf(disp_str,"|4  Calibrate");
       if(select == 0 && mode == 2)sprintf(disp_str,"4Alt  %4d||4Head %4d||Pitc %2d.%1d||4Roll %2d.%1d",
                          height, heading, mpitch/100,abs(mpitch%100)/10,mroll/100,abs(mroll%100)/10);
       if(select == 0 && mode == 3)sprintf(disp_str,"4Throt  %4d||4Yaw    %4d||4Pitch  %4d||4Roll   %4d",
		                          throttle, yaw, pitch, roll);
       if(select == 0 && mode == 4)sprintf(disp_str,"4Motor1 %04d||Motor2 %04d||Motor3 %04d||Motor4 %04d",
		          motor1, motor2, motor3, motor4);
       if(select == 0 && mode == 5)sprintf(disp_str,"|4  Disable");
       if(select == 0 && mode == 6)sprintf(disp_str,"4 %6d",fccnt);
       if(select == 0 && mode == 7)sprintf (disp_str, "1   F450 Transmitter |1   Altitude   Heading\
                |4M  %3d  %03d||4P  %3d  %03d||1volt=%2d.%1dv fc= %d|pitch=%2d.%1d roll=%2d.%1d",
             height, heading,heightprog,headingprog,voltage/10,voltage%10,fccnt,
             mpitch/100,abs(mpitch%100)/10,mroll/100,abs(mroll%100)/10);
       ssd1305_text(disp_str);

       vTaskDelay(10);
    }

}

