/**
 *  @filename   :   epd2in9.cpp
 *  @brief      :   Implements for e-paper library
 *  @author     :   Yehui from Waveshare
 *
 *  Copyright (C) Waveshare     September 9 2017
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdlib.h>
#include "epd2in9_WF29T1.h"


    const char lut_vcom[15] = {0x19, 0x1E, 0x04, 0x28, 0x28, 0x05, 0x1E, 0x19, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; //LUT for different color
    const char lut_ww[15] = {0x19, 0x5E, 0x04, 0xA8, 0x68, 0x05, 0x9E, 0x19, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    const char lut_bw[15] = {0x99, 0x9E, 0x04, 0xA8, 0x68, 0x05, 0x5E, 0x59, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    const char lut_wb[15] = {0x99, 0x9E, 0x04, 0xA8, 0x68, 0x05, 0x5E, 0x59, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    const char lut_bb[15] = {0x99, 0x9E, 0x04, 0xA8, 0x68, 0x05, 0x5E, 0x59, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


Epd::~Epd()
{
};

Epd::Epd()
{
    reset_pin = RST_PIN;
    dc_pin = DC_PIN;
    cs_pin = CS_PIN;
    busy_pin = BUSY_PIN;
    width = EPD_WIDTH;
    height = EPD_HEIGHT;
};

/**
 *  @brief: basic function for sending commands
 */
void Epd::SendCommand(unsigned char command)
{
    DigitalWrite(dc_pin, LOW);
    DigitalWrite(cs_pin, LOW);
    SpiTransfer(command);
    DigitalWrite(cs_pin, HIGH);
}

/**
 *  @brief: basic function for sending data
 */
void Epd::SendData(unsigned char data)
{
    DigitalWrite(dc_pin, HIGH);
    DigitalWrite(cs_pin, LOW);
    SpiTransfer(data);
    DigitalWrite(cs_pin, HIGH);
}

/**
 *  @brief: Wait until the busy_pin goes HIGH
 */
void Epd::WaitUntilIdle(void)
{
    SendCommand(0x71);
    while(DigitalRead(busy_pin) == LOW) {      //LOW: idle, HIGH: busy
        SendCommand(0x71);
        DelayMs(100);
    }
}


/******************************************************************************
function :	LUT download
parameter:
******************************************************************************/
void Epd::SetFullReg(void)
{
    // SendCommand(0X50);			//VCOM AND DATA INTERVAL SETTING
    // SendData(0xb7);		//WBmode:VBDF 17|D7 VBDW 97 VBDB 57		WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7

    unsigned int count;
    SendCommand(0x20);
    for(count=0; count<15; count++) {
        SendData(lut_vcom[count]);
    }

    SendCommand(0x21);
    for(count=0; count<15; count++) {
        SendData(lut_ww[count]);
    }

    SendCommand(0x22);
    for(count=0; count<15; count++) {
        SendData(lut_bw[count]);
    }

    SendCommand(0x23);
    for(count=0; count<15; count++) {
        SendData(lut_wb[count]);
    }

    SendCommand(0x24);
    for(count=0; count<15; count++) {
        SendData(lut_bb[count]);
    }
}


void Epd::TurnOnDisplay(void)
{
    SendCommand(0x12);		 //DISPLAY REFRESH
    DelayMs(10);     //!!!The delay here is necessary, 200uS at least!!!

    WaitUntilIdle();
}

int Epd::Init(void)
{
    /* this calls the peripheral hardware interface, see epdif */
    if (IfInit() != 0) {
        return -1;
    }

    Reset();

    SendCommand(0x01);	//POWER SETTING
    SendData(0x07);
    SendData(0x00);
    SendData(0x0B);
    SendData(0x00);

    SendCommand(0x06); //booster soft start
    SendData(0x17);    //A
    SendData(0x17);    //B
    SendData(0x17);    //C
    SendCommand(0x04); //Power on
    WaitUntilIdle();
    SendCommand(0x00); // PANEL_SETTING
    SendData(0xBF);
	
	SendCommand(0x50);	//PLL setting
    SendData(0x37);     // 3a 100HZ   29 150Hz 39 200HZ	31 171HZ

    SendCommand(0x30);
    SendData(0x39);
	
    SendCommand(0x61); // TCON_RESOLUTION
    SendData(0x80);
    SendData(0x01);
    SendData(0x28);
	
    SendCommand(0x82); // VCM_DC_SETTING_REGISTER
    SendData(0X0E);


    SetFullReg();

    return 0;
}

/**
 *  @brief: module reset.
 *          often used to awaken the module in deep sleep,
 *          see Epd::Sleep();
 */
void Epd::Reset(void)
{
    DigitalWrite(reset_pin, HIGH);
    DelayMs(200);
    DigitalWrite(reset_pin, LOW);                //module reset
    DelayMs(20);
    DigitalWrite(reset_pin, HIGH);
    DelayMs(200);
}

void Epd::Clear(void)
{
    int w, h;
    w = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
    h = EPD_HEIGHT;

    // SendCommand(0x10);
    // for (int j = 0; j < h; j++) {
    //     for (int i = 0; i < w; i++) {
    //         SendData(0x00);
    //     }
    // }

    // SendCommand(0x13);
    // for (int j = 0; j < h; j++) {
    //     for (int i = 0; i < w; i++) {
    //         SendData(0xFF);
    //     }
    // }

    SendCommand(0x10);
    for (uint32_t i = 0; i < 9472; i++)
    {
        SendData(0xFF);
    }

    // SetFullReg();
    TurnOnDisplay();
}

void Epd::Display(const unsigned char *frame_buffer)
{
    int w = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
    int h = EPD_HEIGHT;

    // SendCommand(0x10);
    // for (int j = 0; j < h; j++) {
    //     for (int i = 0; i < w; i++) {
    //         SendData(0x00);
    //     }
    // }
    // // Dev_Delay_ms(10);
    // SendCommand(0x13);
    // for (int j = 0; j < h; j++) {
    //     for (int i = 0; i < w; i++) {
    //         SendData(pgm_read_byte(&frame_buffer[i + j * w]));
    //     }
    // }
    // // Dev_Delay_ms(10);

    SendCommand(0x10);
    unsigned char temp1,temp2,temp3;	
for (int j = 0; j < h; j++) 
        for (int i = 0; i < w; i++) {
        temp2=0;
			temp3=0;
        temp1 = pgm_read_byte(&frame_buffer[i + j * w]);

						if(temp1&1)
							temp2 += 0x03;
						if(temp1&2)
							temp2 += 0x0c;
						if(temp1&4)
							temp2 += 0x30;
						if(temp1&8)
							temp2 += 0xc0;
						
						if(temp1&0x10)
							temp3 += 0x03;
						if(temp1&0x20)
							temp3 += 0x0c;
						if(temp1&0x40)
							temp3 += 0x30;
						if(temp1&0x80)
							temp3 += 0xc0;

		SendData(temp3);
        SendData(temp2);
    }

    // SetFullReg();
    TurnOnDisplay();
}

/**
 *  @brief: After this command is transmitted, the chip would enter the
 *          deep-sleep mode to save power.
 *          The deep sleep mode would return to standby by hardware reset.
 *          The only one parameter is a check code, the command would be
 *          executed if check code = 0xA5.
 *          You can use Epd::Init() to awaken
 */
void Epd::Sleep()
{
    SendCommand(0x50);
    SendData(0x37);
    SendCommand(0x02);
}



/* END OF FILE */


