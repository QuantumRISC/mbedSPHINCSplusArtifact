//#include <_ansi.h>
#include <errno.h>
#include <limits.h>
//#include <_syslist.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>

/* config.h.in.  Generated automatically from configure.in by autoheader.  */

/* Missing syscall names */
#undef MISSING_SYSCALL_NAMES

/* Using ELF format */
#undef HAVE_ELF

/* Using GNU LD */
#undef HAVE_GNU_LD

/* .previous directive allowed */
#undef HAVE_ASM_PREVIOUS_DIRECTIVE

/* .pushsection/.popsection directives allowed */
#undef HAVE_ASM_POPSECTION_DIRECTIVE

/* support for section attributes */
#undef HAVE_SECTION_ATTRIBUTES

/* symbol prefix */
#undef __SYMBOL_PREFIX

#ifndef __WARNING_H__
#define __WARNING_H__

#ifdef HAVE_GNU_LD
# ifdef HAVE_ELF

/* We want the .gnu.warning.SYMBOL section to be unallocated.  */
#  ifdef HAVE_ASM_PREVIOUS_DIRECTIVE
#   define __make_section_unallocated(section_string)   \
  asm(".section " section_string "\n .previous");
#  elif defined (HAVE_ASM_POPSECTION_DIRECTIVE)
#   define __make_section_unallocated(section_string)   \
  asm(".pushsection " section_string "\n .popsection");
#  else
#   define __make_section_unallocated(section_string)
#  endif

#  ifdef HAVE_SECTION_ATTRIBUTES
#   define link_warning(symbol, msg)                     \
  static const char __evoke_link_warning_##symbol[]     \
    __attribute__ ((section (".gnu.warning." __SYMBOL_PREFIX #symbol), \
     __used__)) = msg;
#  else
#   define link_warning(symbol, msg)
#  endif

#else /* !ELF */

#  define link_warning(symbol, msg)             \
  asm(".stabs \"" msg "\",30,0,0,0\n"   \
      ".stabs \"" __SYMBOL_PREFIX #symbol "\",1,0,0,0\n");
# endif
#else /* !GNULD */
/* We will never be heard; they will all die horribly.  */
# define link_warning(symbol, msg)
#endif

/* A canned warning for sysdeps/stub functions.
   The GNU linker prepends a "warning: " string.  */
#define stub_warning(name) \
  link_warning (name, \
                #name " is not implemented and will always fail")

#endif /* __WARNING_H__ */


#undef errno
extern int errno;
extern char end;

#define GPIO_A    ((Gpio_Reg*)(0xF0000000))
#define TIMER_PRESCALER ((Prescaler_Reg*)0xF0020000)
#define TIMER_INTERRUPT ((InterruptCtrl_Reg*)0xF0020010)
#define TIMER_A ((Timer_Reg*)0xF0020040)
#define TIMER_B ((Timer_Reg*)0xF0020050)
#define UART      ((Uart_Reg*)(0xF0010000))

#ifndef PRESCALERCTRL_H_
#define PRESCALERCTRL_H_

#include <stdint.h>


typedef struct
{
  volatile uint32_t LIMIT;
} Prescaler_Reg;

static void prescaler_init(Prescaler_Reg* reg){
  (void)reg;

}

#endif /* PRESCALERCTRL_H_ */
#ifndef TIMERCTRL_H_
#define TIMERCTRL_H_

#include <stdint.h>


typedef struct
{
  volatile uint32_t CLEARS_TICKS;
  volatile uint32_t LIMIT;
  volatile uint32_t VALUE;
} Timer_Reg;

static void timer_init(Timer_Reg *reg){
	reg->CLEARS_TICKS  = 0;
	reg->VALUE = 0;
}


#endif /* TIMERCTRL_H_ */
#ifndef UART_H_
#define UART_H_


typedef struct
{
  volatile uint32_t DATA;
  volatile uint32_t STATUS;
  volatile uint32_t CLOCK_DIVIDER;
  volatile uint32_t FRAME_CONFIG;
} Uart_Reg;

enum UartParity {NONE = 0,EVEN = 1,ODD = 2};
enum UartStop {ONE = 0,TWO = 1};

typedef struct {
	uint32_t dataLength;
	enum UartParity parity;
	enum UartStop stop;
	uint32_t clockDivider;
} Uart_Config;

static uint32_t uart_writeAvailability(Uart_Reg *reg){
	return (reg->STATUS >> 16) & 0xFF;
}
static uint32_t uart_readOccupancy(Uart_Reg *reg){
	return reg->STATUS >> 24;
}

static void uart_write(Uart_Reg *reg, uint32_t data){
	while(uart_writeAvailability(reg) == 0);
	reg->DATA = data;
}

static void uart_applyConfig(Uart_Reg *reg, Uart_Config *config){
	reg->CLOCK_DIVIDER = config->clockDivider;
	reg->FRAME_CONFIG = ((config->dataLength-1) << 0) | (config->parity << 8) | (config->stop << 16);
}

#endif /* UART_H_ */


#ifndef INTERRUPTCTRL_H_
#define INTERRUPTCTRL_H_

#include <stdint.h>

typedef struct
{
  volatile uint32_t PENDINGS;
  volatile uint32_t MASKS;
} InterruptCtrl_Reg;

static void interruptCtrl_init(InterruptCtrl_Reg* reg){
	reg->MASKS = 0;
	reg->PENDINGS = 0xFFFFFFFF;
}

#endif /* INTERRUPTCTRL_H_ */
#ifndef GPIO_H_
#define GPIO_H_


typedef struct
{
  volatile uint32_t INPUT;
  volatile uint32_t OUTPUT;
  volatile uint32_t OUTPUT_ENABLE;
} Gpio_Reg;


#endif /* GPIO_H_ */


#ifndef COUNTER_H_
#define COUNTER_H_
#define COUNTER_VALUE  *((volatile uint32_t*)0xF0001000);
#endif /* UART_H_ */


#define _XOPEN_SOURCE 600
#include <stdlib.h>

#include "hal.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


int save_out = 1;
int save_in = 0;

void hal_setup()
{
}

void hal_putchar(int c)
{
  uart_write(UART,c);
}

int hal_getchar(void)
{
  while(uart_readOccupancy(UART) == 0);
  return UART->DATA;
}

void hal_puts(const char* in)
{
  while(*in) {
    hal_putchar(*in++);
  }
}


#define csr_read(csr)                             \
  ({                                              \
       register unsigned long __v;                \
       __asm__ __volatile__ ("csrr %0, " #csr     \
                             : "=r" (__v));       \
       __v;                                       \
   })
uint64_t hal_get_time(void)
{
  uint64_t lo = csr_read(mcycle);
  uint64_t hi = csr_read(mcycleh);
  return (hi << 32 ) | lo;
}

uint64_t hal_get_num_instr(void)
{
  return -1;
}

void hal_trigger_toggle(void)
{
#warning Trigger not supported on platform!
}

void hal_led_on(void)
{
#warning LED not supported on platform!
}

void hal_led_off(void)
{
  /* Nothing to be done. */
}


/*
 * Stub version of chown.
 */


int _chown(const char* path, uid_t owner, gid_t group);

int _chown(const char* path, uid_t owner, gid_t group)
{
  (void) path;
  (void) owner;
  (void) group;
  errno = ENOSYS;
  return -1;
}

stub_warning(_chown)
/*
 * Stub version of close.
 */


int _close(int fildes);

int _close(int fildes)
{
  (void) fildes;
  errno = ENOSYS;
  return -1;
}

stub_warning(_close)
/*
 * Version of environ for no OS.
 */

char* __env[1] = {0};
char** environ = __env;
/* Supply a definition of errno if one not already provided.  */

int errno;
/*
 * Stub version of execve.
 */


int _execve(char* name, char** argv, char** env);

int _execve(char* name, char** argv, char** env)
{
  (void) name;
  (void) argv;
  (void) env;
  errno = ENOSYS;
  return -1;
}

stub_warning(_execve)
/* Stub version of _exit.  */


void _exit(int rc);

void _exit(int rc)
{
  /* Default stub just causes a divide by 0 exception.  */
  int x = rc / INT_MAX;
  x = 4 / x;

  /* Convince GCC that this function never returns.  */
  for (;;)
    ;
}
/*
 * Stub version of fork.
 */


int _fork(void);

int _fork(void)
{
  errno = ENOSYS;
  return -1;
}

stub_warning(_fork)
/*
 * Stub version of fstat.
 */


int _fstat(int fildes, struct stat* st);

int _fstat(int fildes, struct stat* st)
{
  (void) fildes;
  (void) st;
  errno = ENOSYS;
  return -1;
}

stub_warning(_fstat)
/*
 * Stub version of getpid.
 */


int _getpid(void);

int _getpid(void)
{
  errno = ENOSYS;
  return -1;
}

stub_warning(_getpid)
/*
 * Stub version of gettimeofday.
 */


struct timeval;

int _gettimeofday(struct timeval* ptimeval, void* ptimezone);

int _gettimeofday(struct timeval* ptimeval, void* ptimezone)
{
  (void) ptimeval;
  (void) ptimezone;
  errno = ENOSYS;
  return -1;
}

stub_warning(_gettimeofday)
/*
 * Stub version of isatty.
 */


int _isatty(int file);

int _isatty(int file)
{
  (void) file;
  errno = ENOSYS;
  return 0;
}

stub_warning(_isatty)
/*
 * Stub version of kill.
 */


int _kill(int pid, int sig);

int _kill(int pid, int sig)
{
  (void) pid;
  (void) sig;
  errno = ENOSYS;
  return -1;
}

stub_warning(_kill)
/*
 * Stub version of link.
 */


int _link(char* existing, char* new);

int _link(char* existing, char* new)
{
  (void) existing;
  (void) new;
  errno = ENOSYS;
  return -1;
}

stub_warning(_link)
/*
 * Stub version of lseek.
 */


int _lseek(int file, int ptr, int dir);

int _lseek(int file, int ptr, int dir)
{
  (void) file;
  (void) ptr;
  (void) dir;
  errno = ENOSYS;
  return -1;
}

stub_warning(_lseek)
/*
 * Stub version of open.
 */


int _open(char* file, int flags, int mode);

int _open(char* file, int flags, int mode)
{
  (void) file;
  (void) flags;
  (void) mode;
  errno = ENOSYS;
  return -1;
}

stub_warning(_open)
/*
 * Stub version of read.
 */


int _read(int file, char* ptr, int len);

int _read(int file, char* ptr, int len)

{
  (void) file;
  (void) ptr;
  (void) len;
  errno = ENOSYS;
  return -1;
}

stub_warning(_read)
/*
 * Stub version of readlink.
 */


int _readlink(const char* path, char* buf, size_t bufsize);

int _readlink(const char* path, char* buf, size_t bufsize)
{
  (void) path;
  (void) buf;
  (void) bufsize;
  errno = ENOSYS;
  return -1;
}

stub_warning(_readlink)
/* Version of sbrk for no operating system.  */


char* heap_end = 0;
void* stack_end = &end;
int heap_size = 0;

void* _sbrk(int incr);

void* _sbrk(int incr)
{
  char* prev_heap_end;

  if (heap_end == 0) {
    heap_end = &end;
  }

  prev_heap_end = heap_end;
  heap_end += incr;
  stack_end = heap_end + (sizeof(long) - ((unsigned)heap_end) % sizeof(long));

  heap_size += incr;

  return (void*) prev_heap_end;
}
/*
 * Stub version of stat.
 */


int _stat(const char* file, struct stat* st);

int _stat(const char* file, struct stat* st)
{
  (void) file;
  (void) st;
  errno = ENOSYS;
  return -1;
}

stub_warning(_stat)
/*
 * Stub version of symlink.
 */


int _symlink(const char* path1, const char* path2);

int _symlink(const char* path1, const char* path2)
{
  (void) path1;
  (void) path2;
  errno = ENOSYS;
  return -1;
}

stub_warning(_symlink)
/*
 * Stub version of times.
 */


clock_t _times(struct tms* buf);

clock_t _times(struct tms* buf)
{
  (void) buf;
  errno = ENOSYS;
  return -1;
}

stub_warning(_times)
/*
 * Stub version of unlink.
 */


int _unlink(char* name);

int _unlink(char* name)
{
  (void) name;
  errno = ENOSYS;
  return -1;
}

stub_warning(_unlink)
/*
 * Stub version of wait.
 */


int _wait(int* status);

int _wait(int* status)
{
  (void) status;
  errno = ENOSYS;
  return -1;
}

stub_warning(_wait)
/*
 * Stub version of write.
 */



int _write(int file, char* ptr, int len);

int _write(int file, char* ptr, int len)
{
	(void) file;
  for (int i = 0; i < len; i++) {
      hal_putchar(i[(char*)ptr]);
  }
  return len;
}

stub_warning(_write)
