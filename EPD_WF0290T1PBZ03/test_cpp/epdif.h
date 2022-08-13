/**
 *  @filename   :   epdif.h
 *  @brief      :   Header file of epdif.cpp providing EPD interface functions
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

#ifndef EPDIF_H
#define EPDIF_H

#include <MSP430G2553.h>
//#include <Arduino.h>

// Pin definition
//#define RST_PIN         2
//#define DC_PIN          0
//#define CS_PIN          15
//#define BUSY_PIN        5



// p2.3
#define epdcklow           P2OUT &= 0xf7
#define epdckhigh          P2OUT |= 0x08

// p2.4
#define epddalow           P2OUT &= 0xef
#define epddahigh          P2OUT |= 0x10

// p2.5 busy
#define EPD_BUSY_RD	   P2IN&BIT1

// p3.4
#define epdcslow           P3OUT &= 0xef
#define epdcshigh          P3OUT |= 0x10

// p3.5
#define epddclow           P3OUT &= 0xdf
#define epddchigh          P3OUT |= 0x20

// p3.6
#define epdrstlow          P3OUT &= 0xbf
#define epdrsthigh         P3OUT |= 0x40

// p3.1
#define epdbslow          P3OUT &= 0xfd

// unknown p3.7
#define epdunknowhigh          P3OUT &= 0x80

// power on off, p2.6, L to power on
#define epdon           P2OUT &= 0xbf
#define epdoff          P2OUT |= 0x40

class EpdIf {
public:
    EpdIf(void);
    ~EpdIf(void);

    static int  IfInit(void);
    static void DigitalWrite(int pin, int value); 
    static int  DigitalRead(int pin);
    static void DelayMs(unsigned int delaytime);
    static void SpiTransfer(unsigned char data);
};



#endif


