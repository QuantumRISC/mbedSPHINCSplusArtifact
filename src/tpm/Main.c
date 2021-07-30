#include <stdint.h>
#include <stdio.h>
#include "clientMacros.h"
#include "_TPM_Init_fp.h"
#include "hal.h"
#include "tis.h"
#include <api.h>

#define TPM_MAX_RESPONSE 2048
#define b2u32(x) (((x)[0] << 24) | ((x)[1] << 16) | ((x)[2] << 8) | ((x)[3]))
uint8_t buf[TPM_MAX_RESPONSE] = {0};
int main() {
  hal_setup();
  for (int i = 0; i < 20230; i++) {
    for (int j = 0; j < 1024; j++)
      asm volatile("nop");
    if ((i % 10000) == 0) {
      volatile uint64_t t = hal_get_time();
      hal_send_int("time", t);
    }
  }
  // initialization of TPM firmware and TIS 
  TIS_init(buf);
  printf("[+] booting TPM(%d)\n",SPX_N);
  g_inFailureMode = 0;
  TPM_Manufacture(0);
  _plat__Signal_PowerOn();
  PCRSimStart();
  PCRStartup(SU_RESET, 0);
  Startup_In startup_param = {
      .startupType = TPM_SU_CLEAR,
  };
  TPM2_Startup(&startup_param);
  g_DRTMHandle = TPM_RH_UNASSIGNED;
  uint32_t parent_handle = CP();
  // end of initialization
  printf("Primary Handle is 0x%08x\n", parent_handle);
  printf("[+] TPM booted\n");
  printf("%d\n",hal_get_time());
  printf("%d\n",hal_get_time());
  // TPM booted
  volatile uint32_t sts;
  uint32_t responseSize = TPM_MAX_RESPONSE; // increase as needed
  uint8_t *request = g_TIS.fifo;
  uint8_t *response = g_TIS.fifo;
  // this is the main loop of the TPM
  while (1) {
    sts = (volatile uint32_t)g_TIS.sts[0]; // readout current state of affairs
    if ((sts & TIS_STS_TPM_GO) != 0) {
      uint8_t *b = g_TIS.fifo + 2;
      phex("Received TPM command", g_TIS.fifo, b2u32(b));
      // uint32_t requestSize = g_TIS.fifo_fill+2;
      uint32_t requestSize = (((uint32_t)g_TIS.fifo[2]) << 24) |
                             (((uint32_t)g_TIS.fifo[3]) << 16) |
                             (((uint32_t)g_TIS.fifo[4]) << 8) |
                             (((uint32_t)g_TIS.fifo[5]) << 0);
      // create input for ExecuteCommand
      responseSize = TPM_MAX_RESPONSE;
      ExecuteCommand(requestSize, request, &responseSize, &response);
      // TODO: set g_TIS.fifo_fill for reading out response or change interface
      g_TIS.fifo_fill = 0;
      // put response into tis and block here
      phex("Command Response ready", g_TIS.fifo, responseSize);
      g_TIS.fifo_rsp_max = responseSize;
      g_TIS.sts[0] &= ~(TIS_STS_TPM_GO);      // disable GO bit
      g_TIS.sts[0] |= TIS_STS_DATA_AVAILABLE; // set data available bit

      while (1) {
        if ((g_TIS.sts[0] & TIS_STS_DATA_AVAILABLE) == 0) {
          g_TIS.fifo_rsp_max = 0;
          g_TIS.fifo_fill = 0;
          break;
        }
      }
    } else {
      sts |= TIS_STS_COMMAND_READY;
    }
  }
  return 0; // we never reach this point
}
