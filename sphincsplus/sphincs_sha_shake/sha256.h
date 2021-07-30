#ifndef SPX_SHA256_H
#define SPX_SHA256_H

#if defined(VARIANT_SHA2_SIMPLE) || defined(VARIANT_SHA2_ROBUST)

#include "params.h"

#define SPX_SHA256_BLOCK_BYTES 64
#define SPX_SHA512_BLOCK_BYTES 128
#define SPX_SHA256_OUTPUT_BYTES 32  /* This does not necessarily equal SPX_N */
#define SPX_SHA512_OUTPUT_BYTES 64  /* This does not necessarily equal SPX_N */

#define SPX_SHA256_ADDR_BYTES 22

#include <stddef.h>
#include <stdint.h>

void sha256_inc_init(uint8_t *state);
void sha256_inc_blocks(uint8_t *state, const uint8_t *in, size_t inblocks);
void sha256_inc_finalize(uint8_t *out, uint8_t *state, const uint8_t *in, size_t inlen);
void sha256(uint8_t *out, const uint8_t *in, size_t inlen);


void sha512_inc_init(uint8_t *state);
void sha512_inc_blocks(uint8_t *state, const uint8_t *in, size_t inblocks);
void sha512_inc_finalize(uint8_t *out, uint8_t *state, const uint8_t *in, size_t inlen);
void sha512(uint8_t *out, const uint8_t *in, size_t inlen);

void mgf1_sha256(unsigned char *out, unsigned long outlen,
          const unsigned char *in, unsigned long inlen);

void mgf1_sha512(unsigned char *out, unsigned long outlen,
          const unsigned char *in, unsigned long inlen);

extern uint8_t state_seeded[40];

void seed_state(const unsigned char *pub_seed);



/*
 * streaming
 */
typedef struct
{
  unsigned char inbuf[SPX_N_MAX + SPX_SHA256_ADDR_BYTES + 4]; // possibly wasting some bytes (SPX_N_MAX - SPX_N)
  unsigned long i;

  /* tmp buffer that holds unprocessed bytes. */
  unsigned char tmp[24]; // works for SPX_N = {16, 24, 32}
  uint8_t used_tmp_bytes;
} mgf1_sha256_state_t;
typedef struct
{
  /* the sha256 state */
  uint8_t state[40];

  /* tmp buffer that holds unprocessed bytes (updates in 64 byte blocks) */
  unsigned char tmp[64];
  uint8_t used_tmp_bytes;
#if defined(VARIANT_SHA2_ROBUST)
  mgf1_sha256_state_t mfg_state;
#endif
} sha256_inc_state_t;

void sha256_inc_state_init(sha256_inc_state_t *sha256_state);
void sha256_inc_state_update(sha256_inc_state_t *sha256_state, const unsigned char *data, size_t data_len);
void sha256_inc_state_finalize(sha256_inc_state_t *sha256_state, unsigned char *out);
void mgf1_sha256_state_init(mgf1_sha256_state_t *state, const unsigned char *pub_seed, const uint32_t addr[8]);
void mgf1_sha256_update(unsigned char *out, mgf1_sha256_state_t* state);

#endif

#endif
