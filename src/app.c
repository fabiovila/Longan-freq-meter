#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lcd/lcd.h"
#include "gd32v_pjt_include.h"

#include "gd32vf103.h"
#include <stdio.h>

extern volatile uint32_t difftime;
extern volatile unsigned int freq;
extern volatile int periode;

float freqsmooth;


void timer_configuration(void)
{
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER1);

    timer_deinit(TIMER1);

    timer_struct_para_init(&timer_initpara);

    timer_initpara.prescaler         = 0; // 107
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = periode;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1,&timer_initpara);

    timer_external_trigger_as_external_clock_config(TIMER1, TIMER_SMCFG_TRGSEL_CI0FE0,TIMER_IC_POLARITY_RISING,0);
    timer_external_clock_mode0_config(TIMER1,TIMER_EXT_TRI_PSC_OFF,TIMER_ETP_RISING,0);

    timer_auto_reload_shadow_enable(TIMER1);

    timer_interrupt_enable(TIMER1,TIMER_INT_UP);

    timer_enable(TIMER1);
}


int main(void)
{

    uint8_t tmpbuf[128];
    // Initialize LEDs
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOC);
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1|GPIO_PIN_2);
    LEDR(1);
    LEDG(1);
    LEDB(1);

    // Initialize Display
    Lcd_Init();
    LCD_Clear(BLACK);
    BACK_COLOR=BLACK;

    eclic_global_interrupt_enable();

    eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL2_PRIO2);

    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_AF);

    // PA0
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_0);

    eclic_irq_enable(TIMER1_IRQn,0,1);

    timer_configuration();

    LCD_ShowStr(0, 32, (u8 *) "  Waiting Signal ... ", GREEN, OPAQUE);

    while (freq == 0);
    freqsmooth = freq;

    LCD_Clear(BLACK);

    while (1) {

                freqsmooth = 0.1 * freq + 0.9 * freqsmooth;

                sprintf((char *)tmpbuf, " Freq: %u ",(unsigned int) freqsmooth);
                LCD_ShowStr(0, 0, tmpbuf, GREEN, OPAQUE);


                LCD_DrawLine(20,24,300,24,BLUE);

                sprintf((char *)tmpbuf, " DIFF: %lu ", difftime);
                LCD_ShowStr(0, 32, tmpbuf, RED, OPAQUE);

                sprintf((char *)tmpbuf, " TIMER1: %lu ", timer_counter_read(TIMER1));
                LCD_ShowStr(0,48, tmpbuf, RED, OPAQUE);

                sprintf((char *)tmpbuf, " SYSTICK: %lu ", mtime_lo());
                LCD_ShowStr(0,64, tmpbuf,RED, OPAQUE);

                
            
        

    }
}