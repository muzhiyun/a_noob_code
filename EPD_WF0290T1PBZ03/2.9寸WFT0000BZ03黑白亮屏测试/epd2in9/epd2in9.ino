/**
 *  @filename   :   epd2in9-demo.ino
 *  @brief      :   2.9inch e-paper display demo
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

#include <SPI.h>
#include "epd2in9_WF29T1.h"
#include "epdpaint.h"
#include "imagedata.h"

Epd epd;

void setup()
{
    //pinMode(12, OUTPUT);
    //digitalWrite(12, LOW);
    //delay(200);

    // put your setup code here, to run once:
    Serial.begin(74880);
    Serial.println("e-Paper init-----------");
    if (epd.Init() != 0) {
        Serial.println("e-Paper init failed");
        return;
    }

    Serial.println("e-Paper clear");
    epd.Clear();

    Serial.println("e-Paper show pic");
    //epd.Display(IMAGE_DATA);

    delay(1000);

    Serial.println("e-Paper clear and sleep");
    //epd.Clear();
    //epd.Sleep();

}

void loop()
{

}
