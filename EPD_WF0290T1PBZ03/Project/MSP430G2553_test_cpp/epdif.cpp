/**
 *  @filename   :   epdif.cpp
 *  @brief      :   Implements EPD interface functions
 *                  Users have to implement all the functions in epdif.cpp
 *  @author     :   Yehui from Waveshare
 *
 *  Copyright (C) Waveshare     August 10 2017
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

#include "epdif.h"
//#include <SPI.h>


EpdIf::EpdIf() {
};

EpdIf::~EpdIf() {
};

/*
void EpdIf::DigitalWrite(int pin, int value) {
    digitalWrite(pin, value);
}

int EpdIf::DigitalRead(int pin) {
    return digitalRead(pin);
}

void EpdIf::DelayMs(unsigned int delaytime) {
    delay(delaytime);
}
*/

void EpdIf::DelayMs(unsigned int delaytime)
{
    for (int i = 0; i < delaytime; i++)
    {
        for (int j = 0; j < 160; j++)
            _NOP();
    }
}

 static void sendbyte(int sdbyte)
 {
 	 volatile unsigned int i;
  for (i =0; i<8;i++)
  {
     epdcklow; 
    if (sdbyte & 0x80)
      { epddahigh;
        }
    else
      { epddalow;
       }
    sdbyte <<=1;
     epdckhigh;  
  }
  epdcklow;
 }

void EpdIf::SpiTransfer(unsigned char data) {
    //digitalWrite(CS_PIN, LOW);
    epdcslow;
    //SPI.transfer(data);
    sendbyte(data);
    //digitalWrite(CS_PIN, HIGH);
    epdcshigh;
}

int EpdIf::IfInit(void) {
    //pinMode(CS_PIN, OUTPUT);
    //pinMode(RST_PIN, OUTPUT);
    //pinMode(DC_PIN, OUTPUT);
    //pinMode(BUSY_PIN, INPUT); 
    //SPI.begin();
    //SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
      epdunknowhigh;
      epdbslow ;  
      epdon;
      
  
  P2DIR |= BIT3 | BIT4 | BIT6 ;
  P3DIR |=  BIT1 | BIT4 | BIT5 | BIT6 | BIT7 ;
  


  //P1SEL = 0X0;
  //P1SEL2 = 0X0;
  P2SEL = 0X0;
  P2SEL2 = 0X0;
  P3SEL = 0X0;
  P3SEL2 = 0X0; 


  DelayMs(100);
  
   
    return 0;
}


