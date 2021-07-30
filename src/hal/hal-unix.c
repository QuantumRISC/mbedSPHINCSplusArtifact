#define _XOPEN_SOURCE 600
#include <stdlib.h>

#include "hal.h"

#include <stdio.h>
#include <unistd.h>

#include <sys/stat.h>

#include <fcntl.h>

int save_out;
int save_in;

void hal_setup()
{
  int masterfd;
  char *slavename;

  masterfd = open("/dev/ptmx", O_RDWR);
  grantpt(masterfd);
  unlockpt(masterfd);
  slavename = ptsname(masterfd);

  // Make sure ptmx stays open if a client closes the other side:
  open(slavename, O_RDWR);

  printf("Connect to %s for serial in/out.\n", slavename);
  printf("Press key to continue.\n");

  getchar();

  save_out = dup(fileno(stdout));
  save_in  = dup(fileno(stdin));

  if (-1 == dup2(masterfd, fileno(stdout))) { perror("cannot redirect stdout"); }
  if (-1 == dup2(masterfd, fileno(stdin))) { perror("cannot redirect stdin"); }
}

void hal_putchar(int c)
{
  putchar(c);
}

int hal_getchar(void)
{
  return getchar();
}

void hal_puts(const char* in)
{
  puts(in);
}

uint64_t hal_get_time(void)
{
  #if defined(__x86_64__)
    unsigned int lo, hi;
    __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t) hi << 32) | lo;
  #else
    return -1;
  #endif
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
