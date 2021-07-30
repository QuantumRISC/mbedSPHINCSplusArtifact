#ifndef HAL_H
#define HAL_H

#include <stdint.h>

/**
 * \brief Setup the HAL.
 *
 * \param clock  Mode for the clock.
 */
void hal_setup(void);

/**
 * \brief Send a character to the host.
 *
 * \param c Character to send.
 */
void hal_putchar(int c);

/**
 * \brief Receive character from the host.
 *
 * \param c Received character.
 */
int hal_getchar(void);

/**
 * \brief Send string to host.
 *
 * \param str String to send.
 */
void hal_puts(const char* str);

/**
 * \brief Get the current time.
 *
 * The definition of time may vary between HAL implementations, but
 * usually defined as number of cycles since boot.
 *
 * \return Time relative to boot.
 */
uint64_t hal_get_time(void);

/**
 * \brief Toggle the trigger.
 *
 * \return The number of retired instructions since boot.
 */
void hal_trigger_toggle(void);

/**
 * \brief Turn on the LED.
 *
 * \return The number of retired instructions since boot.
 */
void hal_led_on(void);

/**
 * \brief Turn off the LED.
 *
 * \return The number of retired instructions since boot.
 */
void hal_led_off(void);

#endif
