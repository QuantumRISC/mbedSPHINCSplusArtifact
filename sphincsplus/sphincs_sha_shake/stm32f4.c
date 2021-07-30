#include <hal.h>
#include <stdlib.h>
#include <stdio.h>
#include "params.h"
#include "utils.h"
#include "rng.h"

#define SPX_MLEN 32

unsigned char m[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                     0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                     0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                     0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08	};


extern spx_param_set_e PARAMETER_SET_LIST[];
extern unsigned int PARAMETER_SET_LIST_SIZE;


void write_uint64_be(uint64_t val)
{
  hal_putchar(val >> 56);
  hal_putchar(val >> 48);
  hal_putchar(val >> 40);
  hal_putchar(val >> 32);
  hal_putchar(val >> 24);
  hal_putchar(val >> 16);
  hal_putchar(val >> 8);
  hal_putchar(val);
}

void write_int32_be(int val)
{
  hal_putchar(val >> 24);
  hal_putchar(val >> 16);
  hal_putchar(val >> 8);
  hal_putchar(val);
}

#define SAFETY_MARGIN 0x100
#define STACK_END   0x20000000 + SAFETY_MARGIN
#define STACK_BEGIN 0x20020000 - SAFETY_MARGIN

#ifdef DO_STACK_MEASUREMENTS
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
#endif

int do_keygen(unsigned char *pk, unsigned char *sk)
{
  return crypto_sign_keypair_streaming(pk, sk);
}

int do_sign(unsigned char *sk)
{
  size_t siglen;
  streaming_ctx_t ctx;

  init_streaming_ctx(&ctx, WRITE_SIG_FLAG, STREAM_SERIAL, NULL);
  int ret = crypto_sign_signature_streaming(&ctx, m, SPX_MLEN, sk);
  write_stream(&ctx, m, SPX_MLEN);
  destroy_streaming_ctx(&ctx);

  return ret;
}

int do_sign_local_buf(unsigned char *sk, unsigned char *sig_buf)
{
  size_t siglen;
  streaming_ctx_t ctx;

  init_streaming_ctx(&ctx, WRITE_SIG_FLAG, STREAM_LOCAL_BUFFER, sig_buf);
  int ret = crypto_sign_signature_streaming(&ctx, m, SPX_MLEN, sk);
  destroy_streaming_ctx(&ctx);

  return ret;
}


int do_verify(unsigned char *pk)
{
  streaming_ctx_t ctx;
  unsigned char msg[SPX_MLEN];

  init_streaming_ctx(&ctx, READ_SIG_FLAG, STREAM_SERIAL, NULL);
  read_stream(&ctx, msg, SPX_MLEN);

  int ret = crypto_sign_verify_streaming(&ctx, msg, SPX_MLEN, pk);
  destroy_streaming_ctx(&ctx);

  return ret;
}

int do_verify_local_buf(unsigned char *sig_buf, unsigned char *pk)
{
  streaming_ctx_t ctx;

  init_streaming_ctx(&ctx, READ_SIG_FLAG, STREAM_LOCAL_BUFFER, sig_buf);

  int ret = crypto_sign_verify_streaming(&ctx, m, SPX_MLEN, pk);
  destroy_streaming_ctx(&ctx);

  return ret;
}

int main()
{
#ifdef DO_STACK_MEASUREMENTS
  register void *sp asm ("sp");
  uintptr_t *stack_start_pos;

  int stack_size_keygen, stack_size_sign, stack_size_verify;
  int ret;
#endif
  hal_setup();

  hal_getchar();  // wait for any char

  int ret_keygen;
  int ret_sign;
  int ret_verify;

/*
 * ===================
 *  Stack Measurements
 * ===================
 */
#ifdef DO_STACK_MEASUREMENTS
  (void)ret;
  /*
   * setup current stack pointer as base value
   */
  stack_start_pos = sp;

  for(size_t paramset = 0; paramset < PARAMETER_SET_LIST_SIZE; paramset++)
  {
    srand2(100);
    init_params(PARAMETER_SET_LIST[paramset]);
    unsigned char sk[SPX_SK_BYTES];
    unsigned char pk[SPX_PK_BYTES];

    spray_stack();
    ret_keygen = do_keygen(pk, sk);
    write_int32_be(ret_keygen); // print result (prevents optimizing out function)
    stack_size_keygen = get_stack_size(stack_start_pos);

    // write out PK
    for(size_t i = 0; i < SPX_PK_BYTES; i++)
    {
      hal_putchar(pk[i]);
    }

    spray_stack();
    ret_sign = do_sign(sk);   // streams out sig and msg
    write_int32_be(ret_sign); // print result (prevents optimizing out function)
    stack_size_sign = get_stack_size(stack_start_pos);

    spray_stack();
    ret_verify = do_verify(pk); // streams in msg and sig
    write_int32_be(ret_verify); // print result (prevents optimizing out function)
    stack_size_verify = get_stack_size(stack_start_pos);

    /*
     * Substract the SPX_SK_BYTES from the verify measurement. This would typically not be on the stack for
     * some party that merely verifies a signature.
     */
    stack_size_verify -= SPX_SK_BYTES;

    if(ret_keygen || ret_verify || ret_sign)
    {
      printf("FAILED\n");
      while(1) {};
    }
    printf("%s\n", get_param_name(PARAMETER_SET_LIST[paramset]));

    write_int32_be(stack_size_keygen);                                            // stack size from main
    write_int32_be(stack_size_keygen + (0x20020000 - (unsigned)stack_start_pos)); // stack size from start of RAM

    write_int32_be(stack_size_sign);                                              // stack size from main
    write_int32_be(stack_size_sign + (0x20020000 - (unsigned)stack_start_pos));   // stack size from start of RAM

    write_int32_be(stack_size_verify);                                            // stack size from main
    write_int32_be(stack_size_verify + (0x20020000 - (unsigned)stack_start_pos)); // stack size from start of RAM
  }
#endif

#ifdef DO_SPEED_MEASUREMENTS

# define SPX_BYTES_MAX 50000
# define SPX_N_MAX 32
  unsigned char sig[SPX_BYTES_MAX];
  unsigned char pk[2*SPX_N_MAX];
  unsigned char sk[4*SPX_N_MAX];

  streaming_ctx_t ctx;

  uint64_t keygen_cycles, sign_cycles, verify_cycles;
  uint64_t start;

  init_streaming_ctx(&ctx, WRITE_SIG_FLAG, STREAM_SERIAL, NULL);

  for(size_t paramset = 0; paramset < PARAMETER_SET_LIST_SIZE; paramset++)
  {
    init_params(PARAMETER_SET_LIST[paramset]);
    keygen_cycles = 0;
    sign_cycles = 0;
    verify_cycles = 0;

    /*
     * measure keygen
     */
    hal_getchar();
    start = hal_get_time();
    ret_keygen = do_keygen(pk, sk);
    keygen_cycles = hal_get_time() - start;
    hal_putchar(0);

    // write out pk, sk
    for(size_t i = 0; i < SPX_PK_BYTES; i++)
    {
      hal_putchar(pk[i]);
    }
    for(size_t i = 0; i < SPX_SK_BYTES; i++)
    {
      hal_putchar(sk[i]);
    }


    /*
     * measure sign
     */
    hal_getchar();
    start = hal_get_time();
    ret_sign = do_sign_local_buf(sk, sig);
    sign_cycles = hal_get_time() - start;
    hal_putchar(0);

    // write out sig
    write_stream(&ctx, sig, SPX_BYTES);

    /*
     * measure verify
     */
    hal_getchar();
    start = hal_get_time();
    ret_verify = do_verify_local_buf(sig, pk);
    verify_cycles = hal_get_time() - start;
    hal_putchar(0);

    write_int32_be(ret_keygen);
    write_int32_be(ret_sign);
    write_int32_be(ret_verify);
    if(ret_keygen || ret_verify || ret_sign)
    {
      printf("FAILED\n");
      while(1) {};
    }

    printf("%s\n", get_param_name(PARAMETER_SET_LIST[paramset]));
    write_uint64_be(keygen_cycles);
    write_uint64_be(sign_cycles);
    write_uint64_be(verify_cycles);
  }

  destroy_streaming_ctx(&ctx);
#endif
}
