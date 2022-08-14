
//#include "io430.h"

#include "epd2in9_WF29T1.h"
#include "epdpaint.h"
#include "imagedata.h"
#include "uart.h"

Epd epd;

void delay_ms(int count)
{
    for (int i = 0; i < count; i++)
    {
        for (int j = 0; j < 160; j++)
            _NOP();
    }
}

int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  
      /* Use Calibration values for 1MHz Clock DCO*/
    DCOCTL = 0;
    BCSCTL1 = CALBC1_1MHZ + DIVA_3;  // XTS is 0, ACLK divide 8
    DCOCTL = CALDCO_1MHZ;

    BCSCTL3 = LFXT1S_2; // VLO 12KHz

    // alt name TACCTL0
    CCTL0 = CCIE;                             // CCR0 interrupt enabled
    TACTL = TASSEL_1 + MC_1 + ID_3;           // ACLK/8, upmode
    // ACLK 12kHz/8 = 1500Hz,  TA0 clk = 1500/8 = 187.5
    CCR0 =  11250;   // 1 minute per interrupt

    
    uart_init();

    uart_send_str("test_cpp a7105 communication \r\n");
    delay_ms(1000);
    uart_send_str(__DATE__);
    //uart_send_str(" \r\n");
     delay_ms(1000);
    uart_send_str(__TIME__);
    uart_send_str("\r\n");

  
  if (epd.Init() != 0) {
        uart_send_str("e-Paper init failed\r\n");
        return 0;
    }
    delay_ms(1000);
    uart_send_str("e-Paper clear\r\n");
    epd.Clear();

    uart_send_str("e-Paper show pic\r\n");
    epd.Display(IMAGE_DATA);

    delay_ms(5000);

    uart_send_str("e-Paper clear and sleep\r\n");
    //epd.Clear();
    epd.Sleep();
    
    while(1)
    {
    }

  return 0;
}
