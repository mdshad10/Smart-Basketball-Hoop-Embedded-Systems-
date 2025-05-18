/*
 * lcd_helpers.c  –  7-segment timer / score utilities
 */

#include <stdint.h>
#include "lcd_helpers.h"
#include "lcd.h"

/* ------------------------------------------------------------------------------------ */
/*   These are some of the LCD helper functions to display the timer on the onboard LCD */
/* ------------------------------------------------------------------------------------ */

// This helper function allows the timer to be displayed on the two left most segment displays
void lcd_show_timer(uint8_t s)
{
    displayDigit(1, s / 10);
    displayDigit(2, s % 10);
}

// This helper function allows the score to be displayed on the two right most segment displays
void lcd_show_score(uint16_t s)
{
    /* This limits the score to 99 for display purposes(realistically a player won't score more than 99 in 60 seconds(max) */
    if (s > 99) s = 99;
    displayDigit(3, s / 10);
    displayDigit(4, s % 10);
}
