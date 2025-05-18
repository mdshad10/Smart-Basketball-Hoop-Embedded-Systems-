/*
 * breakbeam.c  –  ADC-based IR beam sensor
 */

#include <stdint.h>
#include "MKL46Z4.h"
#include "systick.h"
#include "breakbeam.h"
#include "game.h"
#include "lcd_helpers.h"
#include "led.h"
#include "buzzer.h"

/* ============================================================================== */
/*   This is where breakbeam sensor is initalized and the reading system is setup */
/* ============================================================================== */

//Initialize breakbeam
void breakbeam_init(void)
{
    SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;

    /* This configures ADC0 for 12-bit resolution and clock divide by 8 */
    ADC0->CFG1 = ADC_CFG1_MODE(2)    /* 12-bit mode */
                | ADC_CFG1_ADIV(3)    /* Divide by 8 */
                | ADC_CFG1_ADICLK(0); /* Bus clock */
}

static uint16_t read_breakbeam(void)
{
    /* Start a conversion on ADC channel 11 (PTC2) */
    ADC0->SC1[0] = ADC_SC1_ADCH(11);

    /* Wait for conversion complete */
    while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));

    /* Then read the result */
    return ADC0->R[0];
}

/* When the breakbeam reads a value of 700 or less then that means the laser was disconnected
 * from the receptor which means the basketball went through the hoop and cover the laser from hitting
 * the receptor. This causes the breakbeam sensor to read a value of 700 or less which signals a make
 * and there is visual feedback with the green on board led lighting up and the buzzer playing the sound
 * for a score and the score is incremented by 1.
 */
void check_breakbeam(void)
{
    uint16_t result = read_breakbeam();
    uint32_t now_ms = systick_counter;

    if (result <= 700 && (now_ms - last_beam_break_ms > BEAM_DEBOUNCE_MS))
    {
        score++;
        last_beam_break_ms = now_ms;
        lcd_show_score(score);
        green_on_frdm(); // visual feedback
        wait_ms(80);
        green_off_frdm();
        buzzer_play_powerup();  // audio feedback
    }
}
