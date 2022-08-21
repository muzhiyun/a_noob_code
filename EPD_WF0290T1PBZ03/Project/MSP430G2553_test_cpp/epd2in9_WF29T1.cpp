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
    width = EPD_WIDTH;
    height = EPD_HEIGHT;
};

/**
 *  @brief: basic function for sending commands
 */
void Epd::SendCommand(unsigned char command)
{
    //DigitalWrite(dc_pin, LOW);
    epddclow;
    //DigitalWrite(cs_pin, LOW);
    epdcslow;
    SpiTransfer(command);
    //DigitalWrite(cs_pin, HIGH);
    epdcshigh;
}

/**
 *  @brief: basic function for sending data
 */
void Epd::SendData(unsigned char data)
{
    //DigitalWrite(dc_pin, HIGH);
    epddchigh;
    //DigitalWrite(cs_pin, LOW);
    epdcslow;
    SpiTransfer(data);
    //DigitalWrite(cs_pin, HIGH);
    epdcshigh;
}

/**
 *  @brief: Wait until the busy_pin goes HIGH
 */
void Epd::WaitUntilIdle(void)
{
    SendCommand(0x71);
    while(EPD_BUSY_RD == 0) {      //LOW: idle, HIGH: busy
        SendCommand(0x71);
        DelayMs(1000);
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
    



    SendCommand(0x06); //booster soft start
    SendData(0x17);    //A
    SendData(0x17);    //B
    SendData(0x17);    //C
    SendCommand(0x01);	//POWER SETTING
    SendData(0x03);
    SendData(0x00);
    SendData(0x2B);
    SendData(0x2B);
    SendData(0x09);
    SendCommand(0x04); //Power on
    DelayMs(100);
    WaitUntilIdle();
    DelayMs(100);
    SendCommand(0x00); // PANEL_SETTING
    SendData(0xBF);
	
    SendCommand(0x30);	//PLL setting
    SendData(0x3a);  
    //SendData(0x39); 
 


	
    SendCommand(TCON_RESOLUTION); // TCON_RESOLUTION
    SendData(0x80);            //0x80=128
    SendData(0x01);            //0x128=296
    SendData(0x28);
	
    SendCommand(0x82); // VCM_DC_SETTING_REGISTER
    SendData(0X12);
    //SendData(0X0E);
    
    SendCommand(0x50);	
    SendData(0x77);    


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
    //DigitalWrite(reset_pin, HIGH);
    epdrsthigh;
    DelayMs(100);
    //DigitalWrite(reset_pin, LOW);                //module reset
    epdrstlow;
    DelayMs(100);
    //DigitalWrite(reset_pin, HIGH);
    epdrsthigh;
    DelayMs(100);
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
    //for (uint32_t i = 0; i < 9472; i++)
    //(EPD_WIDTH * EPD_HEIGHT / 8)
    //4736
    for (uint32_t i = 0; i < 9472 ; i++)
    {
        SendData(0xFF);
    }

    // SetFullReg();
    TurnOnDisplay();
}

void Epd::Display(const unsigned char *frame_buffer,int flag)
{
    
     if(flag ==0){      //2bbp sizeof(imagedata)=9472
       int w = (EPD_WIDTH % 4 == 0)? (EPD_WIDTH / 4 ): (EPD_WIDTH / 4 + 1);
       int h = EPD_HEIGHT;
       SendCommand(DATA_START_TRANSMISSION_1);
       SendCommand(0x10);
       for (int j = 0; j < h*w; j++) {
               SendData(frame_buffer[j]);
       }
     }else{             //1bbp sizeof(imagedata)=4736
    // // Dev_Delay_ms(10);
    //DelayMs(1000);
    // SendCommand(0x13);
    // for (int j = 0; j < h; j++) {
    //     for (int i = 0; i < w; i++) {
    //         SendData(pgm_read_byte(&frame_buffer[i + j * w]));
    //     }
    // }
    // // Dev_Delay_ms(10);
    int w = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
    int h = EPD_HEIGHT;
    SendCommand(0x10);
    unsigned char temp1,temp2,temp3;	
    for (int j = 0; j < h; j++) 
        for (int i = 0; i < w; i++) {
            temp2=0;
            temp3=0;
            temp1 = (frame_buffer[i + j * w]);

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
    SendData(0x77);
    SendCommand(0x02);
}



/* END OF FILE */


