//#include "PlatformData.h"
#define NORETURN

/* This file only contains stubs and in effect removes most
 * time features from the TPM firmware 
 * (they are not required for our experiments)
 *********************************************************/

#include "BaseTypes.h"
#include "Platform_fp.h"
#include "TpmFail_fp.h"
#include <assert.h>
#include <time.h>

void _plat__TimerReset(void)
{
  return;
}

void _plat__TimerRestart(void)
{
  return;
}

uint64_t _plat__RealTime(void)
{
    return hal_get_time();
}

uint64_t _plat__TimerRead(void)
{
    return hal_get_time();
}

BOOL _plat__TimerWasReset(void)
{
    return 0;
}

BOOL _plat__TimerWasStopped(void)
{
    return 0;
}

void _plat__ClockAdjustRate(int adjust)
{
  return;
}

