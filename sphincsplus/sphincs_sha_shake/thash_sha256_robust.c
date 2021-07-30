#if defined(VARIANT_SHA2_ROBUST)

#include <stdint.h>
#include <string.h>

#include "thash.h"
#include "address.h"
#include "params.h"
#include "sha256.h"

/**
 * Takes an array of inblocks concatenated arrays of SPX_N bytes.
 */
void thash_sha256(unsigned char *out, const unsigned char *in, unsigned int inblocks,
           const unsigned char *pub_seed, uint32_t addr[8])
{
    unsigned char buf[SPX_N + SPX_SHA256_ADDR_BYTES + inblocks*SPX_N];
    unsigned char outbuf[SPX_SHA256_OUTPUT_BYTES];
    unsigned char bitmask[inblocks * SPX_N];
    uint8_t sha2_state[40];
    unsigned int i;

    memcpy(buf, pub_seed, SPX_N);
    memcpy(buf + SPX_N, addr, SPX_SHA256_ADDR_BYTES);
    mgf1_sha256(bitmask, inblocks * SPX_N, buf, SPX_N + SPX_SHA256_ADDR_BYTES);

    /* Retrieve precomputed state containing pub_seed */
    memcpy(sha2_state, state_seeded, 40 * sizeof(uint8_t));

    for (i = 0; i < inblocks * SPX_N; i++) {
        buf[SPX_N + SPX_SHA256_ADDR_BYTES + i] = in[i] ^ bitmask[i];
    }

    sha256_inc_finalize(outbuf, sha2_state, buf + SPX_N,
                        SPX_SHA256_ADDR_BYTES + inblocks*SPX_N);
    memcpy(out, outbuf, SPX_N);
}


/*
 * Streaming
 */

void thash_init_sha256(sha256_inc_state_t *sha256_state, mgf1_sha256_state_t *mgf1_sha256_state, const unsigned char *pub_seed, uint32_t addr[8])
{
  sha256_inc_state_init(sha256_state);
//  sha256_inc_state_update(sha256_state, pub_seed, SPX_N);
  memcpy(sha256_state->state, state_seeded, 40);
  sha256_inc_state_update(sha256_state, (uint8_t*) addr, SPX_SHA256_ADDR_BYTES);

  mgf1_sha256_state_init(mgf1_sha256_state, pub_seed, addr);
}

void thash_update_sha256(sha256_inc_state_t *sha256_state, mgf1_sha256_state_t *mgf1_sha256_state, const unsigned char *in, int block_idx, const unsigned char *pub_seed, uint32_t addr[8])
{
  (void)addr;
  (void)block_idx;
  (void)pub_seed;

  unsigned char buf[SPX_N];
  unsigned char bitmask[SPX_N];
  unsigned int j;

  mgf1_sha256_update(bitmask, mgf1_sha256_state);
  for(j = 0; j < SPX_N; j++)
  {
    buf[j] = in[j] ^ bitmask[j];
  }
  sha256_inc_state_update(sha256_state, buf, sizeof(buf));
}

void thash_finalize_sha256(unsigned char* out, sha256_inc_state_t *sha256_state)
{
  sha256_inc_state_finalize(sha256_state, out);
}

#endif