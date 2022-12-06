#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "ps2Defines.h"



// Initialize PS/2 keyboard support
// Parameters
//     pio  - keyboard suport pio number. 0 or 1
//     gpio - GPIO number of data pin, ctl pin must be on next
//            adjacent GPIO
// Returns  - none
void atk_init(uint pio, uint gpio);

void atk_send_command(unsigned short data );

unsigned int ps2_add_parity(unsigned char byte);

#ifdef __cplusplus
}
#endif
