#ifndef BUZZER_H_
#define BUZZER_H_

#include <stdint.h>

void  buzzer_init(void);
void  buzzer_play_tone(uint16_t freq_hz, uint16_t duration_ms);
void  buzzer_play_victory(void);
void  buzzer_play_powerup(void);
void  buzzer_play_game_over(void);

#endif /* BUZZER_H_ */
