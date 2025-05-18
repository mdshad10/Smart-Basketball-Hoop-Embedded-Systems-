/*
 * basketball_game.c  –  High-level mini-basketball game loop
 *
 *  Created on: May 2 2025
 *      Author: Kaelem Bent (kab472) & Md Shad (mss464)
 */

#include <stdint.h>
#include <stdbool.h>
#include "MKL46Z4.h"
#include "lcd.h"
#include "led.h"
#include "board.h"

#include "game.h"
#include "systick.h"
#include "buttons.h"
#include "buzzer.h"
#include "breakbeam.h"
#include "lcd_helpers.h"

/* --------------------------------------------------------------- */
/*          Definitions that were only used in main                */
/* --------------------------------------------------------------- */
#define delay_1s()    wait_ms(1000)
#define delay_0_5s()  wait_ms(500)

/* =============================================================== */
/*              Global game-state variables                        */
/* =============================================================== */
volatile uint8_t  preset        = 0;
volatile uint8_t  running       = 0;
volatile uint32_t sw1_press_ms  = 0;
volatile uint8_t  sw1_pressed   = 0;
volatile uint16_t score         = 0;
volatile uint32_t last_beam_break_ms = 0;

/* ================================================================================== */
/*   This is the logic for prepping for the countdown/timer to start the round        */
/* ================================================================================== */

// After switch 3 is pressed to start the round, we added a mini 3 second timer to allow
// the player to get ready and get in the proper position to shoot the ball
// In the 3 second countdown we have the led light up red and the buzzer plays a low frequency right before
// the timer approaches 0 we flash green and play a high pitch sound to signal the start of the round

static void countdown_prep(void)
{
    /* Flash LED0‑RED (PTE16) on and off (750ms each) three times,
       then flash LED0‑GREEN (PTE6) for one second before countdown. */

    /* We reset the score for every new game/round */
    score = 0;
    lcd_show_score(score);

    /* make sure user LEDs are off */
    red_off();
    green_off();

    for (int n = 3; n > 0; --n) {
        lcd_show_timer(n);       /* display 3, 2, 1 */
        red_on();           // LED0 red on

        /* Play short beep for each countdown number */
        buzzer_play_tone(1500, 100);  //Ascending tone

        wait_ms(800);            // Remainder of the 3/4 second on period
        red_off();                // LED0 red off
        wait_ms(900);            // 3/4 second off
    }


    /* GREEN LED flashes for one second to signal start */
    green_on();             // LED0 green on

    /* Play high frequency sound when countdown finishes and the game starts */

    buzzer_play_tone(2000, 2000);  // Higher frequency beep
    wait_ms(700);
    green_off();
}

/* =============================================================== */
/*   Main                                                          */
/* =============================================================== */
int main(void)
{
    // Initialize everything(leds, lcd, timers, buzzers, breakbeam sensor, switches) before the start
	set15MHz();
    led_init();
    green_off();
    init_lcd();
    clearDisplay();
    systick_init();
    buttons_init();
    buzzer_init();
    breakbeam_init();
    green_off();

    for (;;) {
        if (running == 0) {
            lcd_show_timer(preset);
            lcd_show_score(0);  /* Show 0 score in idle state */
        }
        else if (running == 1) {
            countdown_prep();
            running = 2;
        }
        else {
            uint8_t sec = preset;
            while (sec) {
                lcd_show_timer(sec);

                /* Check for beam breaks during each second */
                uint32_t start_time = systick_counter;
                while (systick_counter - start_time < 1000) {
                    check_breakbeam();
                    /* Small delay to prevent excessive ADC sampling */
                    wait_ms(20);
                }

                --sec;
            }

            /* Time's up! Then play game over sound */
            buzzer_play_game_over();

            /* Flash zeros on timer digits to signal end of a round */

            for (int i = 0; i < 6; ++i) {
                if (i & 1) {
                    clearDigit(1);
                    clearDigit(2);
                }
                else {
                    lcd_show_timer(0);
                }
                delay_0_5s();
            }

            /* Play victory sound if the score > 0 */
            // We were generous to count a make of at least one to be considered a victory :)
            if (score > 0) {
            	buzzer_play_victory();
            	green_on();
				wait_ms(800);
            	green_off();

            }

            /* This keeps showing the final score and the timer is reset to zero */
            lcd_show_timer(0);
            preset  = 0;
            running = 0;
        }
    }
}
