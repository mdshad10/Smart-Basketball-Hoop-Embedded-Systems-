/*
 * buzzer.c  –  Piezo driver & melodies
 */

#include <stdint.h>
#include "MKL46Z4.h"
#include "systick.h"
#include "buzzer.h"
#include "led.h"



/* --------------------------------------------------------------- */
/*                     Buzzer (PTC1)                               */
/* --------------------------------------------------------------- */

#define BUZZER_PORT    PORTC
#define BUZZER_GPIO    PTC
#define BUZZER_PIN     1
#define BUZZER_FREQ_HZ 2000       // frequency in Hz for standard beep
#define BUZZER_FREQ_LOUD_HZ 4000  // frequency in Hz for louder/higher beep


/* ------------------------------------------------------------ */
/*       Buzzer initialization and control                      */
/* ------------------------------------------------------------ */
void buzzer_init(void)
{
    /* Enable clock for PORTC */
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;

    /* This is configuring PTC1 as GPIO output for buzzer */
    BUZZER_PORT->PCR[BUZZER_PIN] = PORT_PCR_MUX(1);  /* GPIO mode */
    BUZZER_GPIO->PDDR |= (1 << BUZZER_PIN);          /* Set as output */

    /* This makes sure the buzzer is off initially */
    BUZZER_GPIO->PCOR = (1 << BUZZER_PIN);
}

/* This toggles PTC1 at the requested frequency for N milliseconds.
 * The 15 MHz core clock means loop_3cycles() executes 5 MHz. */
/* The non-blocking buzzer tone generator uses systick timer */

void buzzer_play_tone(uint16_t freq_hz, uint16_t duration_ms)
{
    uint32_t half_period_us = 500000 / freq_hz;  /* defining  a half period in microseconds */
    uint32_t cycles = 15 * half_period_us;

    for (int i = 0; i < freq_hz * duration_ms / 1000; i++) {
        BUZZER_GPIO->PTOR = (1 << BUZZER_PIN);   /* Toggle buzzer pin */
        loop_3cycles(cycles / 3);                /* Delay for half period by dividing by 3 for loop_3cycles */
        BUZZER_GPIO->PTOR = (1 << BUZZER_PIN);   /* Toggle buzzer pin */
        loop_3cycles(cycles / 3);                /* Delay for other another half period */
    }
}

// Help from AI to setup the melodies/durations but we had to manually adjust to make sure
// the buzzer was producing the correct sounds at the right tempo
// This plays a victory sound and the logic when it is played is handled in main

void buzzer_play_victory(void)
{
    // Tempo: quarter = 500 ms, eighth = 250 ms, half = 1000 ms
    const uint16_t melody[] = {
        330,  440,  // E4, A4
        494,  466,  // B4, A#4
        440,  392,  // A4, G4
        659,  784,  // E5, G5
        880,  784,  // A5, G5
        659,  523,  // E5, C5
        587,  494   // D5, B4
    };
    const uint16_t durations[] = {
        1000, 1000,  // half, half
         250,  250,  // eighth, eighth
         250,  250,  // eighth, eighth
         500,  500,  // quarter, quarter
         500,  500,  // quarter, quarter
         250,  250,  // eighth, eighth
         500,  250,  // quarter, eighth
         250,  500   // eighth, quarter
    };

    // Using the melodies and durations the proper sound is produced to represent a victory
    // We are trying to play the Super Mario Theme song as a fun song to represent victory

    for (uint8_t i = 0; i < sizeof(melody)/sizeof(melody[0]); i++) {
        if (melody[i] == 0) {
            wait_ms(durations[i]);
        } else {
            buzzer_play_tone(melody[i], durations[i]);
        }
        wait_ms(100);
}
}

// This is the sound that plays when a basket is made in order to signal a successful make
// This is a short and simple sound in order to quickly and clearly signal a make
// We created the sound by manipulating the frequencies we were inputting into our buzzer_play_tone function

void buzzer_play_powerup(void)
{
    /*  Bright coin chime using two higher notes */
    buzzer_play_tone( 3136, 140);
    wait_ms(50);
    buzzer_play_tone( 2349, 140);

    /*  Downward coin sweep by start/stop a bit higher */
    for (uint16_t f = 2300; f >= 900; f -= 20) {
        buzzer_play_tone(f, 6);
    }

    /* Drawer thud which is a nudge up one tone */
    wait_ms(30);
    buzzer_play_tone( 659, 170);
}
/* This plays when the game is over or when the timer reaches zero */
// We created the sound by manipulating the frequencies we were inputting into our buzzer_play_tone function
void buzzer_play_game_over(void)
{
    buzzer_play_tone(1568, 150);
    wait_ms(50);
    buzzer_play_tone(1175, 150);
    wait_ms(50);
    buzzer_play_tone(784, 300);
}
