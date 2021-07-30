#include "hal.h"

//#define SERIAL_BAUD 38400

#include <libopencm3/cm3/dwt.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#if defined(LM3S)
#include <libopencm3/lm3s/gpio.h>
#include <libopencm3/lm3s/usart.h>

#define TRIGGER_GPIO GPIO0
#define SERIAL_USART USART0_BASE
#elif defined(STM32F4)
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>


#define SERIAL_USART USART2

#define LED_GPIO GPIOD
#define LED_PIN  GPIO12


//#define USE_DWT
#define STM32
#else
#error Unsupported libopencm3 family
#endif

#define _RCC_CAT(A, B) A##B
#define RCC_ID(NAME) _RCC_CAT(RCC_, NAME)

static int _hal_is_setup = 0;

static unsigned _systicks = 0;

static uint32_t _clock_freq;

void sys_tick_handler(void)
{
  _systicks += 1;
}

extern uint32_t rcc_ahb_frequency;
extern uint32_t rcc_apb1_frequency;
extern uint32_t rcc_apb2_frequency;



void hal_setup()
{
#if defined(LM3S)
  /* Nothing todo */
  /* rcc_clock_setup_in_xtal_8mhz_out_50mhz(); */
#elif defined(STM32F407VG)
  _clock_freq = 4000000;

  rcc_clock_setup_pll(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);
  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_USART2);
  rcc_periph_clock_enable(RCC_DMA1);

  gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2 | GPIO3);
  gpio_set_af(GPIOA, GPIO_AF7, GPIO2 | GPIO3);


  /* Enable GPIOD clock. */
  rcc_peripheral_enable_clock(&RCC_AHB1ENR, RCC_AHB1ENR_IOPDEN);

  /* Set GPIO12 (in GPIO port D) to 'output push-pull'. */
  gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT,
            GPIO_PUPD_NONE, GPIO12 | GPIO13 | GPIO14 | GPIO15);


  usart_set_baudrate(USART2, 115200);
  usart_set_databits(USART2, 8);
  usart_set_stopbits(USART2, USART_STOPBITS_1);
  usart_set_mode(USART2, USART_MODE_TX_RX);
  usart_set_parity(USART2, USART_PARITY_NONE);
  usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

  usart_enable(USART2);
#endif

#if defined(USE_DWT)
  dwt_enable_cycle_counter();
#else
  systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
  systick_set_reload(0xFFFFFFu);
  systick_interrupt_enable();
  systick_counter_enable();
#endif
  _hal_is_setup = 1;
}

void hal_putchar(int c)
{
  if (!_hal_is_setup) {
    hal_setup();
  }
  usart_send_blocking(SERIAL_USART, c);
}

int hal_getchar()
{
  if (!_hal_is_setup) {
    hal_setup();
  }
  return usart_recv_blocking(SERIAL_USART);
}

void hal_puts(const char* in)
{
  if (!_hal_is_setup) {
    hal_setup();
  }
  const char* cur = in;
  while (*cur) {
    usart_send_blocking(SERIAL_USART, *cur);
    cur += 1;
  }
}

uint64_t hal_get_time(void)
{
  if (!_hal_is_setup) {
    hal_setup();
  }
#if defined(USE_DWT)
  return dwt_read_cycle_counter();
#else
  uint64_t upper, ret;
  do {
    upper = _systicks;
    ret = (1 << 24) - (systick_get_value() & ((1 << 24) - 1));
    if (_systicks == upper) {
    }
  } while(upper != _systicks);
  return (upper << 24) | ret;
#endif
}

uint64_t hal_get_num_instr(void)
{
  if (!_hal_is_setup) {
    hal_setup();
  }
  return -1;
}

//  void hal_trigger_toggle(void)
//  {
//    if (!_hal_is_setup) {
//      hal_setup();
//    }
//  #if defined(LM3S)
//  #warning Trigger not supported on platform!
//  #elif defined(STM32)
//    gpio_toggle(TRIGGER_GPIO, TRIGGER_PIN);
//  #else
//  #warning Trigger not supported on platform!
//  #endif
//  }
  
  void hal_led_on(void)
  {
  #if defined(LM3S)
  #elif defined(STM32)
    gpio_set(LED_GPIO, LED_PIN);
  #else
  #warning LED not supported on platform!
  #endif
  }
  
  void hal_led_off(void)
  {
  #if defined(LM3S)
  #elif defined(STM32)
    gpio_clear(LED_GPIO, LED_PIN);
  #endif
  }

#include <errno.h>
#undef errno
extern int errno;

int __wrap__read(int file, char* ptr, int len)
{
  if (file == 0) {
    int i;
    for (i = 0; i < len; ++i) {
      ptr[i] = hal_getchar();
      if (ptr[i] == '\r') {
        ptr[i] = '\n';
      }
      if (ptr[i] == '\n') {
        i += 1;
        break;
      }
    }
    errno = 0;
    return i;
  } else {
    errno = ENOSYS;
  }
  return -1;
}

int __wrap__write(int file, char* ptr, int len)
{
  if (file == 1 || file == 2) {
    for (int i = 0; i < len; ++i) {
      hal_putchar(ptr[i]);
    }
    errno = 0;
    return len;
  } else {
    errno = ENOSYS;
  }
  return -1;
}

