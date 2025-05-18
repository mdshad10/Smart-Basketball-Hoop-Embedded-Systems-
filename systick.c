/*
 * systick.c  –  1-ms system tick and busy-wait helper
 */

#include "MKL46Z4.h"
#include "systick.h"

/* --------------------------------------------------------------- */
/*                 Global 1-ms counter (was in main)               */
/* --------------------------------------------------------------- */
volatile uint32_t systick_counter = 0;   /* global millisecond tick */

/* ---------------------------------------------------------------- */
/*  SysTick helpers to help with our timing to make it precise      */
/* ---------------------------------------------------------------- */

void systick_init(void)
{
    SysTick_Config(SystemCoreClock / SYSTICK_FREQ);    /* 1-ms tick */
}

/* This is a busy-wait delay using the global tick */
void wait_ms(uint32_t ms)
{
    uint32_t start = systick_counter;
    while ((systick_counter - start) < ms) {
        __NOP();
    }
}

/* Called every 1 ms ------------------------------------------------*/
void SysTick_Handler(void)
{
    extern volatile uint8_t  sw1_pressed;
    extern volatile uint32_t sw1_press_ms;
    extern volatile uint8_t  running;
    extern volatile uint8_t  preset;

    systick_counter++;

    /*  If SW1 is held ≥2 s, force preset to 30 s (arcade default) */
    if (sw1_pressed && (systick_counter - sw1_press_ms >= 2000)) {
        sw1_pressed = 0;
        if (!running) preset = 30;
    }
}
