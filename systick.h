#ifndef SYSTICK_H_
#define SYSTICK_H_

#include <stdint.h>

#define SYSTICK_FREQ 1000    /* 1 kHz ⇒ 1-ms */
extern volatile uint32_t systick_counter;


void    systick_init(void);
void    wait_ms(uint32_t ms);

#endif /* SYSTICK_H_ */
