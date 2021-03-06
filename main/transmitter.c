#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "ringbuf.h"

#include "esp8266/spi_struct.h"
#include "esp8266/gpio_struct.h"
#include "esp_system.h"
#include "esp_log.h"

#include "driver/gpio.h"
#include "driver/spi.h"
#include <esp_timer.h>

//globals
int select = 0, mode = 1;
int throttle = 1000; int yaw = 1500; int pitch = 1500; int roll = 1500; 
int motor1 = 0, motor2 = 0, motor3 = 0, motor4 = 0; 
int height=0, heightprog=0, heading=0, headingprog=0, xdisp=0, ydisp=0;
int mpitch=0, mroll=0, voltage=0, fccnt = 0; 

//spi dependancies
#define NRF24L01_CE_GPIO    16
#define NRF24L01_CE_MASK    (1ULL<<NRF24L01_CE_GPIO)
#include "./interfaces/spi.c"
#include "./tasks/rflink_task.c"

//i2c depenencies
#define I2C_SCL_IO           5                /*!< gpio number for I2C master clock */
#define I2C_SDA_IO           4                /*!< gpio number for I2C master data  */
#include "./interfaces/i2c.c"
#include "./functionc/ssd1306.c"

//joystick reading
#include "./tasks/ads1115.c"
//button reading
#include "./tasks/buttons_task.c"


void app_main(void)
{
    spi_initialize(); //setup transmitter radio

    i2c_init();   //setup and detect devices on i2c interface
    i2c_detect();
    

    ssd1305_init();   //setup oled display
    ssd1305_blank(0x00);
    char disp_str[128] = "4 Hi There";
    ssd1305_text(disp_str);

    //sends transmitter commands every 50 msec
    xTaskCreate (rflink_task, "rflink_task", 2048, NULL, 4, NULL);
    //reads joystick adcs every 40msec
    xTaskCreate (crap_joysticks, "read_joysticks", 2048, NULL, 4, NULL);
    //reads buttons and updates oled display
    xTaskCreate (buttons_task, "buttons_task", 4048, NULL, 4, NULL);

    while(1) {
       vTaskDelay(50);
    }

}
