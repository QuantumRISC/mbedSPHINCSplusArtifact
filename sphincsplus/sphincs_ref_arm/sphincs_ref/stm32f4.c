#include <hal.h>
#include <stdio.h>
#include "api.h"
#include <inttypes.h>

#define SPX_MLEN 32


unsigned char m[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                     0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                     0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                     0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08	};


/* Taken (and adapted) from https://www.microchip.com/forums/m453018.aspx */
char *itoa(uint64_t n)
{
  uint64_t v = n;
  static char buff [13];  // Ensure this buffer is big enough for the biggest value
  char *pBuff = &(buff [12]);  // And that this points to the last character of it

  *pBuff = '\0';
  do {
    *--pBuff = '0' + v % 10;
  } while (v = v / 10);

  if (n < 0) *--pBuff = '-';
  return (pBuff);
}


#define SAFETY_MARGIN 0x100
#define STACK_END   0x20000000 + SAFETY_MARGIN
#define STACK_BEGIN 0x20020000 - SAFETY_MARGIN

static void spray_stack()
{
  register void *sp asm ("sp");
  uintptr_t *stack_ptr;

  /*
   * setup current stack pointer as base value
   */
  stack_ptr = sp - SAFETY_MARGIN; // safety margin to the top

  /*
   * Stack ranges from 0x20020000 to 0x20000000
   */

  while(stack_ptr > (uintptr_t*)STACK_END) // also safety margin to the bottom
  {
    stack_ptr--;
    *stack_ptr = 0xDEADBEEF;
  }
}

static int get_stack_size(uintptr_t *start)
{
  /*
   * find the stack spray pattern and give its difference to the start ptr
   */

  uintptr_t *end;
  end = (uintptr_t*)STACK_END;

  while(end < (uintptr_t*)STACK_BEGIN)
  {
    end++;
    if(*end != 0xDEADBEEF)
    {
      break;
    }
  }

  return (unsigned)start - (unsigned)end;
}



int main()
{
  hal_setup();
  
  int ret_keygen;
  int ret_sign;
  int ret_verify;

  unsigned char sig[SPX_BYTES];
  unsigned char pk[SPX_PK_BYTES];
  unsigned char sk[SPX_SK_BYTES];
  size_t sig_len;

  uint64_t keygen_cycles, sign_cycles, verify_cycles;
  uint64_t start;

  register void *sp asm ("sp");
  uintptr_t *stack_start_pos;
  int stack_size_keygen, stack_size_sign, stack_size_verify;

  keygen_cycles = 0;
  sign_cycles = 0;
  verify_cycles = 0;
  
  stack_start_pos = sp;

  printf("%s\n====\n", PARAMS_STR);

  /*
   * measure keygen
   */
  spray_stack();
  start = hal_get_time();
  ret_keygen = crypto_sign_keypair(pk, sk);
  keygen_cycles = hal_get_time() - start;
  stack_size_keygen = get_stack_size(stack_start_pos);

  /*
   * measure sign
   */
  spray_stack();
  start = hal_get_time();
  ret_sign = crypto_sign_signature(sig, &sig_len, m, SPX_MLEN, sk);
  sign_cycles = hal_get_time() - start;
  stack_size_sign = get_stack_size(stack_start_pos);

  /*
   * measure verify
   */
  spray_stack();
  start = hal_get_time();
  ret_verify = crypto_sign_verify(sig, SPX_BYTES, m, SPX_MLEN, pk);
  verify_cycles = hal_get_time() - start;
  stack_size_verify = get_stack_size(stack_start_pos);

  printf("ret keygen, sign, verify: %d, %d, %d\n", ret_keygen, ret_sign, ret_verify);
  if(ret_keygen || ret_verify || ret_sign)
  {
    printf("FAILED\n");
    return -1;
  }

  printf("keygen cycles: %s\n", itoa(keygen_cycles));
  printf("sign cycles: %s\n", itoa(sign_cycles));
  printf("verify cycles: %s\n", itoa(verify_cycles));
  printf("---\n");
  printf("keygen stack usage: %d\n", stack_size_keygen);
  printf("sign stack usage: %d\n", stack_size_sign);
  printf("verify stack usage: %d\n", stack_size_verify);
  printf("\n\n");
}