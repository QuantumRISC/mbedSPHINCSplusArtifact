#if defined(VARIANT_SHA2_SIMPLE)

#include <stdint.h>
#include <string.h>

#include "thash.h"
#include "address.h"
#include "params.h"
#include "sha256.h"

/**
 * Takes an array of inblocks concatenated arrays of SPX_N bytes.
 */
void thash_sha256_simple(unsigned char *out, const unsigned char *in, unsigned int inblocks,
           const unsigned char *pub_seed, uint32_t addr[8])
{
    unsigned char buf[SPX_SHA256_ADDR_BYTES + inblocks*SPX_N];
    unsigned char outbuf[SPX_SHA256_OUTPUT_BYTES];
    uint8_t sha2_state[40];

    (void)pub_seed; /* Suppress an 'unused parameter' warning. */

    /* Retrieve precomputed state containing pub_seed */
    memcpy(sha2_state, state_seeded, 40 * sizeof(uint8_t));

    memcpy(buf, addr, SPX_SHA256_ADDR_BYTES);
    memcpy(buf + SPX_SHA256_ADDR_BYTES, in, inblocks * SPX_N);

    sha256_inc_finalize(outbuf, sha2_state, buf, SPX_SHA256_ADDR_BYTES + inblocks*SPX_N);
    memcpy(out, outbuf, SPX_N);
}



/*
 * Streaming
 */

void thash_init_sha256_simple(sha256_inc_state_t *sha256_state, const unsigned char *pub_seed, uint32_t addr[8])
{
  sha256_inc_state_init(sha256_state);
//  sha256_inc_state_update(sha256_state, pub_seed, SPX_N);
  (void)pub_seed;
  memcpy(sha256_state->state, state_seeded, 40);
  sha256_inc_state_update(sha256_state, (uint8_t*) addr, SPX_SHA256_ADDR_BYTES);
}

void thash_update_sha256_simple(sha256_inc_state_t *sha256_state, const unsigned char *in, int block_idx, const unsigned char *pub_seed, uint32_t addr[8])
{
  (void)addr;
  (void)block_idx;
  (void)pub_seed;

  sha256_inc_state_update(sha256_state, in, SPX_N);
}

void thash_finalize_sha256_simple(unsigned char* out, sha256_inc_state_t *sha256_state)
{
  sha256_inc_state_finalize(sha256_state, out);
}

#endif