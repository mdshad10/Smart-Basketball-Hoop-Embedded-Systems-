#ifndef GAME_H_
#define GAME_H_

#include <stdint.h>

/* --------------------------------------------------------------- */
/*     These are the game states which are modified in ISRs        */
/* --------------------------------------------------------------- */
extern volatile uint8_t  preset;               // 0-60 s, set by SW1
extern volatile uint8_t  running;              // 0 idle | 1 prep | 2 run
extern volatile uint32_t sw1_press_ms;         // timestamp of when SW1 is pressed
extern volatile uint8_t  sw1_pressed;          // flag while SW1 is held down
extern volatile uint16_t score;                // running score
extern volatile uint32_t last_beam_break_ms;   // for beam-break debouncing

#define BEAM_DEBOUNCE_MS 300   /* debounce time (ms)                   */

#endif /* GAME_H_ */
