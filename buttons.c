/*
 * buttons.c  –  SW1 / SW3 GPIO setup and shared PORTC/PORTD ISR
 */

#include <stdint.h>
#include "MKL46Z4.h"
#include "buttons.h"
#include "game.h"
#include "systick.h"

/* GPIO pin map ----------------------------------------------------*/
/* Switch 1 */
#define SW1_PORT  PORTC  /* Press 1 s → +5 s / hold ≥2 s → 30 s preset */
#define SW1_GPIO  PTC
#define SW1_PIN   3
/* Switch 3 */
#define SW3_PORT  PORTC  /* Press to start the timer */
#define SW3_GPIO  PTC
#define SW3_PIN   12
#define PORTC_IRQN PORTC_PORTD_IRQn  /* shared IRQ vector */

/* ---------------------------------------------------------------- */
/*This is the GPIO/NVIC setup in order to use the buttons SW1 & SW3 */
/* ---------------------------------------------------------------- */
void buttons_init(void)
{
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;

    /* SW1 – IRQ on both edges to catch press & release */
    SW1_PORT->PCR[SW1_PIN] =  PORT_PCR_MUX(1)
                            | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK
                            | PORT_PCR_IRQC(0b1011);
    SW1_GPIO->PDDR &= ~(1u << SW1_PIN);

    /* SW3 – IRQ on falling edge (press) */
    SW3_PORT->PCR[SW3_PIN] =  PORT_PCR_MUX(1)
                            | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK
                            | PORT_PCR_IRQC(0b1010);
    SW3_GPIO->PDDR &= ~(1u << SW3_PIN);

    NVIC_EnableIRQ(PORTC_IRQN);
}

/* ================================================================================ */
/* This is the ISR shared by PORTC / PORTD and handles both of the buttons/switches */
/* ================================================================================ */

void PORTC_PORTD_IRQHandler(void)
{
    // For Switch 1
	if (SW1_PORT->PCR[SW1_PIN] & PORT_PCR_ISF_MASK) {
        uint8_t pressed = !(SW1_GPIO->PDIR & (1u << SW1_PIN)); //active low
        SW1_PORT->PCR[SW1_PIN] |= PORT_PCR_ISF_MASK; //clear flag

        if (pressed) {
            sw1_press_ms = systick_counter;
            sw1_pressed  = 1;
        } else {
        	/* short press <2 s increments preset */
        	if (sw1_pressed && (systick_counter - sw1_press_ms < 2000)) {
                if (!running) {
                    preset = (preset + 5 > 60) ? 60 : preset + 5;
                }
            }
            sw1_pressed = 0;
        }
    }
    // For Switch 3
    if (SW3_PORT->PCR[SW3_PIN] & PORT_PCR_ISF_MASK) {
        SW3_PORT->PCR[SW3_PIN] |= PORT_PCR_ISF_MASK;
        if (!running && preset) running = 1; //arm countdown
    }
}
