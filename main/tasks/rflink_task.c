//spi dependancies
//#define NRF24L01_CE_GPIO    16
//#define NRF24L01_CE_MASK    (1ULL<<NRF24L01_CE_GPIO)

int nrf24_transmit_pkt ( uint8_t *data, int length) {
    //setup nrf24l01 transmitter
    gpio_set_level (NRF24L01_CE_GPIO, 0);
    spi_write_byte ( 0x20 | 0x00, 0x00); //no crc, tx mode
    spi_write_byte ( 0x20 | 0x01, 0x00); //no auto ack
    spi_write_byte ( 0x20 | 0x02, 0x01); //pipe0
    spi_write_byte ( 0x20 | 0x03, 0x03);
    spi_write_byte ( 0x20 | 0x04, 0x04);
    spi_write_byte ( 0x20 | 0x05, 0x05); //freq channel 5
    spi_write_byte ( 0x20 | 0x06, 0x06); //low power, 1MB/sec
    spi_write_byte ( 0x20 | 0x11, 0x20); //use all 32 bytes

    //turn on and flush fifo
    spi_write_byte ( 0x20 | 0x00, 0x02); //turn on
    spi_write_byte ( 0xe1, 0x00);        //flush tx fifo
    spi_write_byte ( 0x20 | 0x07, 0x70);
    ets_delay_us(10);                          //busy-wait

    //vTaskDelay(1);
    //printf("   post flush  = 0x%02x  0x%02x\n", 
    //      spi_read_bytes ( 0x07, data, 1), spi_read_bytes ( 0x17, data, 1));

    //send packet to nrf24l01 transmitter
    spi_write_bytes ( 0xa0, data, length);

    //vTaskDelay(1);
    //printf("   post fifow  = 0x%02x  0x%02x\n", 
    //      spi_read_bytes ( 0x07, data, 1), spi_read_bytes ( 0x17, data, 1));

    //ce chip radio enable
    //with jumper wires on bread board ce pulse 45us was about 50%
    gpio_set_level (NRF24L01_CE_GPIO, 1);
    ets_delay_us(500);   //busy-wait at 250us only sends first two bytes
    gpio_set_level (NRF24L01_CE_GPIO, 0);

    //vTaskDelay(1);
    //printf("   post trans  = 0x%02x  0x%02x\n", 
    //      spi_read_bytes ( 0x07, data, 1), spi_read_bytes ( 0x17, data, 1));

    spi_write_byte ( 0x20 | 0x00, 0x00); //turn off

    return(0);
}

int wait_rcv_pkt ( uint8_t *data, int timeout) {
    //setup nrf24l01 transmitter
    gpio_set_level (NRF24L01_CE_GPIO, 0);
    spi_write_byte ( 0x20 | 0x00, 0x00); //no crc, tx mode
    spi_write_byte ( 0x20 | 0x00, 0x01); //no crc, rx mode
    spi_write_byte ( 0x20 | 0x01, 0x00); //no auto ack
    spi_write_byte ( 0x20 | 0x02, 0x01); //pipe0
    spi_write_byte ( 0x20 | 0x03, 0x03);
    spi_write_byte ( 0x20 | 0x04, 0x00);
    spi_write_byte ( 0x20 | 0x05, 0x05); //freq channel 5
    spi_write_byte ( 0x20 | 0x06, 0x06); //low power, 1MB/sec
    spi_write_byte ( 0x20 | 0x11, 0x20); //use all 32 bytes

    //turn on and flush fifo
    spi_write_byte ( 0x20 | 0x00, 0x03); //turn on
    spi_write_byte ( 0xe2, 0x00);        //flush rx fifo
    spi_write_byte ( 0x20 | 0x07, 0x70);

    gpio_set_level (NRF24L01_CE_GPIO, 1);
    ets_delay_us(100);                          //busy-wait

    int waitcnt = 0;
    int timestart = esp_timer_get_time();
    while(1){
	spi_read_bytes ( 0x07, data, 1);
	vTaskDelay(1);
        if( (data[0] & 0x40) > 1 || waitcnt > timeout) break;
        ++waitcnt;
    }
    //if (waitcnt > timeout) printf("wait timed out %d\n", waitcnt);
    if(waitcnt < timeout) {
        spi_read_bytes ( 0x61, data, 32+1);
    }
    //set ce = 0
    gpio_set_level (NRF24L01_CE_GPIO, 0);
    //printf("waited %8.4fsec   ", (float)(esp_timer_get_time()-timestart)/1000000);
    return(waitcnt);
}

void rflink_task () {
    uint8_t data[33];
    int cnt = 0;
    int timeout = 5, waitlen;
    TickType_t xLoopStart = xTaskGetTickCount();
    while(1) {
	data[0] = cnt/256;      data[1] = cnt%256;
	data[2] = throttle/256; data[3] = throttle%256;
	data[4] = yaw/256;      data[5] = yaw%256;
	data[6] = pitch/256;    data[7] = pitch%256;
	data[8] = roll/256;     data[9] = roll%256;
	data[10] = mode/256;    data[11] = mode%256;
	for(int a=12;a<32;a++)data[a]=0;
	//for(int a=0;a<12;a++)printf("%3d ",data[a]);printf("\n");
        nrf24_transmit_pkt ( data, 32 );
        ++cnt;
        waitlen = wait_rcv_pkt ( (uint8_t*)data, timeout); 
	//if (waitlen < timeout) printf("packet rcvd in %d\n", waitlen); else printf("timed out\n");
	if (waitlen < timeout){
	    fccnt = 256 * data[0] +data[1];
            height = data[2]-128 ;
            heightprog = data[3]-128;
            heading = 2*data[4];
            headingprog = 2*data[5];
            xdisp = data[6]-128;
            ydisp = data[7]-128;
            mpitch = (256 * data[8] + data[9]) -18000;
            mroll = (256 * data[10] + data[11]) -18000;
	    motor1 = 256 * data[12] + data[13];
	    motor2 = 256 * data[14] + data[15];
	    motor3 = 256 * data[16] + data[17];
	    motor4 = 256 * data[18] + data[19];
            voltage= 100+data[20] ;
	}
	vTaskDelayUntil(&xLoopStart, 5);
    }
}

