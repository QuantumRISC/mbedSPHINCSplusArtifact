#if defined(VARIANT_SHAKE_ROBUST)


#include <stdint.h>
#include <string.h>

#include "thash.h"
#include "address.h"
#include "params.h"

#include "fips202.h"
#include "sha256.h" // state types

/**
 * Takes an array of inblocks concatenated arrays of SPX_N bytes.
 */
void thash_shake256(unsigned char *out, const unsigned char *in, unsigned int inblocks,
           const unsigned char *pub_seed, uint32_t addr[8])
{
    unsigned char buf[SPX_N + SPX_ADDR_BYTES + inblocks*SPX_N];
    unsigned char bitmask[inblocks * SPX_N];
    unsigned int i;

    memcpy(buf, pub_seed, SPX_N);
    memcpy(buf + SPX_N, addr, SPX_ADDR_BYTES);

    shake256(bitmask, inblocks * SPX_N, buf, SPX_N + SPX_ADDR_BYTES);

    for (i = 0; i < inblocks * SPX_N; i++) {
      buf[SPX_N + SPX_ADDR_BYTES + i] = in[i] ^ bitmask[i];
    }

    shake256(out, SPX_N, buf, SPX_N + SPX_ADDR_BYTES + inblocks*SPX_N);
}



/*
 * Streaming
 */

void thash_init_shake256(shake256_inc_state_t *shake256_state, const unsigned char *pub_seed, uint32_t addr[8])
{
  shake256_inc_init(shake256_state->s_inc);
  shake256_inc_absorb(shake256_state->s_inc, pub_seed, SPX_N);
  shake256_inc_absorb(shake256_state->s_inc, (uint8_t*) addr, SPX_ADDR_BYTES);

  shake256_inc_init(shake256_state->s_inc_bitmask);
  shake256_inc_absorb(shake256_state->s_inc_bitmask, pub_seed, SPX_N);
  shake256_inc_absorb(shake256_state->s_inc_bitmask, (uint8_t*) addr, SPX_ADDR_BYTES);
  shake256_inc_finalize(shake256_state->s_inc_bitmask);
}

void thash_update_shake256(shake256_inc_state_t *shake256_state, const unsigned char *in, int block_idx, const unsigned char *pub_seed, uint32_t addr[8])
{
  (void)addr;
  (void)block_idx;
  (void)pub_seed;

  unsigned char buf[SPX_N];
  unsigned char bitmask[SPX_N];
  unsigned int i;

  shake256_inc_squeeze(bitmask, SPX_N, shake256_state->s_inc_bitmask);
  for (i = 0; i < SPX_N; i++)
  {
    buf[i] = in[i] ^ bitmask[i];
  }

  shake256_inc_absorb(shake256_state->s_inc, buf, SPX_N);
}

void thash_finalize_shake256(unsigned char* out, shake256_inc_state_t *shake256_state)
{
  shake256_inc_finalize(shake256_state->s_inc);
  shake256_inc_squeeze(out, SPX_N, shake256_state->s_inc);
}

#endif