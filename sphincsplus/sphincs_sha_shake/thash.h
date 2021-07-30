#ifndef SPX_THASH_H
#define SPX_THASH_H

#include <stdint.h>

/*
 * streaming
 */
#include "fips202.h" // state type


void thash(unsigned char *out, const unsigned char *in, unsigned int inblocks,
           const unsigned char *pub_seed, uint32_t addr[8]);
void thash_init(void *hash_state, const unsigned char *pub_seed, uint32_t addr[8]);
void thash_update(void *hash_state, const unsigned char *in, int block_idx, const unsigned char *pub_seed, uint32_t addr[8]);
void thash_finalize(unsigned char* out, void* hash_state);

#if defined(VARIANT_SHAKE_ROBUST)
void thash_shake256(unsigned char *out, const unsigned char *in, unsigned int inblocks,
           const unsigned char *pub_seed, uint32_t addr[8]);
void thash_init_shake256(shake256_inc_state_t *shake256_state, const unsigned char *pub_seed, uint32_t addr[8]);
void thash_update_shake256(shake256_inc_state_t *shake256_state, const unsigned char *in, int block_idx, const unsigned char *pub_seed, uint32_t addr[8]);
void thash_finalize_shake256(unsigned char* out, shake256_inc_state_t* shake256_state);
#endif

#if defined(VARIANT_SHAKE_SIMPLE)
void thash_shake256_simple(unsigned char *out, const unsigned char *in, unsigned int inblocks,
                           const unsigned char *pub_seed, uint32_t addr[8]);
void thash_init_shake256_simple(shake256_inc_state_t *shake256_state, const unsigned char *pub_seed, uint32_t addr[8]);
void thash_update_shake256_simple(shake256_inc_state_t *shake256_state, const unsigned char *in, int block_idx, const unsigned char *pub_seed, uint32_t addr[8]);
void thash_finalize_shake256_simple(unsigned char* out, shake256_inc_state_t *shake256_state);
#endif


#if defined(VARIANT_SHA2_ROBUST)
#include "sha256.h" // state types
void thash_sha256(unsigned char *out, const unsigned char *in, unsigned int inblocks,
           const unsigned char *pub_seed, uint32_t addr[8]);
void thash_init_sha256(sha256_inc_state_t *sha256_state, mgf1_sha256_state_t *mgf1_sha256_state, const unsigned char *pub_seed, uint32_t addr[8]);
void thash_update_sha256(sha256_inc_state_t *sha256_state, mgf1_sha256_state_t *mgf1_sha256_state, const unsigned char *in, int block_idx, const unsigned char *pub_seed, uint32_t addr[8]);
void thash_finalize_sha256(unsigned char* out, sha256_inc_state_t *sha256_state);
#endif

#if defined(VARIANT_SHA2_SIMPLE)
#include "sha256.h" // state types
void thash_sha256_simple(unsigned char *out, const unsigned char *in, unsigned int inblocks,
                         const unsigned char *pub_seed, uint32_t addr[8]);
void thash_init_sha256_simple(sha256_inc_state_t *sha256_state, const unsigned char *pub_seed, uint32_t addr[8]);
void thash_update_sha256_simple(sha256_inc_state_t *sha256_state, const unsigned char *in, int block_idx, const unsigned char *pub_seed, uint32_t addr[8]);
void thash_finalize_sha256_simple(unsigned char* out, sha256_inc_state_t *sha256_state);
#endif

#endif
