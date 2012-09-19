/**
 * @file
 * @brief Kernel software interrupts (a.k.a. deferred procedure call) handling.
 *
 * @date 14.02.10
 * @author Eldar Abusalimov
 *         - Initial API design
 * @author Dasha Teplyh
 *         - Documenting some parts of API
 */

#ifndef KERNEL_SOFTIRQ_H_
#define KERNEL_SOFTIRQ_H_

#include <kernel/softirq_lock.h>

#include <embox/unit.h>

/**
 * Total amount of possible soft IRQs.
 */
#define SOFTIRQ_NRS_TOTAL \
	OPTION_GET(NUMBER,nrs_total)

/**
 * Checks if the specified softirq_nr represents valid soft IRQ number.
 */
#define softirq_nr_valid(softirq_nr) \
	(1U << (softirq_nr))

#if !softirq_nr_valid(SOFTIRQ_NRS_TOTAL - 1)
# error "Illegal value for SOFTIRQ_NRS_TOTAL"
#endif

#define SOFTIRQ_NR_TEST 31
#define SOFTIRQ_NR_UART 30
#define SOFTIRQ_NR_TIMER 0

/**
 * Deferred Interrupt Service Routine type.
 *
 * @param softirq_nr the interrupt request number being handled
 * @param data the device tag specified at #softirq_install() time
 */
typedef void (*softirq_handler_t)(unsigned int softirq_nr, void *data);

/**
 * Installs the handler on the specified soft IRQ number replacing an old one
 * (if any).
 *
 * @param nr the soft IRQ number to install handler on
 * @param handler the deferred ISR itself (can be @c NULL)
 * @param data the optional device tag which will be passed to the handler.
 *
 * @return installation result
 * @retval 0 if all is OK
 * @retval -EINVAL if the @c nr doesn't represent valid soft IRQ number
 *
 * @see #softirq_nr_valid()
 */
extern int softirq_install(unsigned int nr, softirq_handler_t handler,
		void *data);

/**
 * Defers a call to the soft IRQ handler for the specified number (if any has
 * been installed earlier).
 *
 * @param nr the soft IRQ number
 *
 * @return the activation result
 * @retval 0 if the activation is done
 * @retval -EINVAL if the @c nr doesn't represent valid soft IRQ number
 */
extern int softirq_raise(unsigned int nr);

#endif /* KERNEL_SOFTIRQ_H_ */
