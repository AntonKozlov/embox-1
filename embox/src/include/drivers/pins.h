/**
 * @file
 * @brief Pins system interface
 *
 * @date 26.11.2010
 * @author Anton Kozlov
 */

#ifndef PINS_H_
#define PINS_H_

#include <hal/pins.h>

/**
 * @param ch_mask Bits changed caused handler call
 * @param mon_mask Bits monitored by handler
 */
typedef void (*pin_handler_t)(int ch_mask, int mon_mask);

/**
 * Monitor changing of mask bits, on change pin_handler will be called
 */
extern void pin_set_input_monitor(int mask, pin_handler_t pin_handled);

#endif /* PINS_H_ */
