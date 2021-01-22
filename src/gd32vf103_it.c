#include "gd32vf103_it.h"
#include "gd32v_pjt_include.h"
#include "gd32vf103.h"
#include <stdio.h>

volatile int            flag = 0;
volatile uint32_t       oldtime = 0;
volatile int            difftime = 0;
volatile unsigned int   freq = 0;
volatile int            periode = 10;
volatile int            ai = 0;
// Manual ajust values.   10 100         1000    10000     periodes
volatile float adjust[] = {1,0.899754666,0.89088,0.88999254,1,1};


void TIMER1_IRQHandler(void)
{
   

        timer_interrupt_flag_clear(TIMER1,TIMER_INT_FLAG_UP);

        if (flag == 0) {
            oldtime = mtime_lo();
            flag = 1;
        } else {
            difftime = mtime_lo() - oldtime;

            // increase timer overflow by 10 if frequency input is too high
            if (difftime <= 1000) { 
                periode *= 10; 
                ai +=1;
                TIMER_CAR(TIMER1) = periode;
            } else if (difftime > 100000) {
                if (periode > 10) {
                    periode /= 10; 
                    ai -=1;
                    TIMER_CAR(TIMER1) = periode;
                }
            } 
            
            /*
            const int systickclk = 27000000; // 108Mhz / 4
            float systick_periode = 1.0 / 27000000.0;
            float difftime_in_seconds = (1.0 / 27000000.0) * difftime;
            float input_periode = ((1.0 / 27000000.0) * difftime) / ( periode ) ;
            float real_freq = 1.0 / (((1.0 / 27000000.0) * difftime) / ( periode ) ));
            
            //simplified version
            freq = (systickclk * periode) / difftime;
            freq  = (systickclk / difftime) * periode;
            */

            // direct version and manual adjust
            freq = adjust[ai] * (27000000 / difftime) * periode;

            flag = 0;
        }
    

}
