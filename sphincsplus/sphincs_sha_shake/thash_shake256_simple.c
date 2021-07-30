#if defined(VARIANT_SHAKE_SIMPLE)

#include <stdint.h>
#include <string.h>

#include "thash.h"
#include "address.h"
#include "params.h"

#include "fips202.h"

/**
 * Takes an array of inblocks concatenated arrays of SPX_N bytes.
 */
void thash_shake256_simple(unsigned char *out, const unsigned char *in, unsigned int inblocks,
           const unsigned char *pub_seed, uint32_t addr[8])
{
    unsigned char buf[SPX_N + SPX_ADDR_BYTES + inblocks*SPX_N];

    memcpy(buf, pub_seed, SPX_N);
    memcpy(buf + SPX_N, addr, SPX_ADDR_BYTES);
    memcpy(buf + SPX_N + SPX_ADDR_BYTES, in, inblocks * SPX_N);

    shake256(out, SPX_N, buf, SPX_N + SPX_ADDR_BYTES + inblocks*SPX_N);
}



/*
 * Streaming
 */

void thash_init_shake256_simple(shake256_inc_state_t *shake256_state, const unsigned char *pub_seed, uint32_t addr[8])
{
  shake256_inc_init(shake256_state->s_inc);
  shake256_inc_absorb(shake256_state->s_inc, pub_seed, SPX_N);
  shake256_inc_absorb(shake256_state->s_inc, (uint8_t*) addr, SPX_ADDR_BYTES);
}

void thash_update_shake256_simple(shake256_inc_state_t *shake256_state, const unsigned char *in, int block_idx, const unsigned char *pub_seed, uint32_t addr[8])
{
  (void)addr;
  (void)block_idx;
  (void)pub_seed;

  shake256_inc_absorb(shake256_state->s_inc, in, SPX_N);
}

void thash_finalize_shake256_simple(unsigned char* out, shake256_inc_state_t *shake256_state)
{
  shake256_inc_finalize(shake256_state->s_inc);
  shake256_inc_squeeze(out, SPX_N, shake256_state->s_inc);
}
#endif