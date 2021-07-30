#include "thash.h"
#include "params.h"

void thash(unsigned char *out, const unsigned char *in, unsigned int inblocks,
           const unsigned char *pub_seed, uint32_t addr[8]) {
#if defined(VARIANT_SHA2_SIMPLE) || defined(VARIANT_SHAKE_SIMPLE)
  if (PARAMSET_TYPE == SIMPLE)
  {
    #if defined(VARIANT_SHA2_SIMPLE)
    if (PARAMSET_HASH_FUNC == HASH_SHA)
    {
      thash_sha256_simple(out, in, inblocks, pub_seed, addr);
    }
    #endif
    #if defined(VARIANT_SHAKE_SIMPLE)
    if (PARAMSET_HASH_FUNC == HASH_SHAKE)
    {
      thash_shake256_simple(out, in, inblocks, pub_seed, addr);
    }
    #endif
  }
#endif // simple
#if defined(VARIANT_SHA2_ROBUST) || defined(VARIANT_SHAKE_ROBUST)
  if (PARAMSET_TYPE == ROBUST)
  {
    #if defined(VARIANT_SHA2_ROBUST)
    if (PARAMSET_HASH_FUNC == HASH_SHA)
    {
      thash_sha256(out, in, inblocks, pub_seed, addr);
    }
    #endif
    #if defined(VARIANT_SHAKE_ROBUST)
    if (PARAMSET_HASH_FUNC == HASH_SHAKE)
    {
      thash_shake256(out, in, inblocks, pub_seed, addr);
    }
    #endif
  }
#endif // robust
}


void thash_init(void *hash_state, const unsigned char *pub_seed, uint32_t addr[8]) {
#if defined(VARIANT_SHA2_SIMPLE) || defined(VARIANT_SHAKE_SIMPLE)
  if (PARAMSET_TYPE == SIMPLE)
  {
    #if defined(VARIANT_SHA2_SIMPLE)
    if (PARAMSET_HASH_FUNC == HASH_SHA)
    {
      thash_init_sha256_simple((sha256_inc_state_t*)hash_state, pub_seed, addr);
    }
    #endif
    #if defined(VARIANT_SHAKE_SIMPLE)
    if (PARAMSET_HASH_FUNC == HASH_SHAKE)
    {
      thash_init_shake256_simple((shake256_inc_state_t*)hash_state, pub_seed, addr);
    }
    #endif
  }
#endif // simple
#if defined(VARIANT_SHA2_ROBUST) || defined(VARIANT_SHAKE_ROBUST)
  if (PARAMSET_TYPE == ROBUST)
  {
    #if defined(VARIANT_SHA2_ROBUST)
    if (PARAMSET_HASH_FUNC == HASH_SHA)
    {
      thash_init_sha256((sha256_inc_state_t*)hash_state, &((sha256_inc_state_t*)hash_state)->mfg_state, pub_seed, addr);
    }
    #endif
    #if defined(VARIANT_SHAKE_ROBUST)
    if (PARAMSET_HASH_FUNC == HASH_SHAKE)
    {
      thash_init_shake256((shake256_inc_state_t*)hash_state, pub_seed, addr);
    }
    #endif
  }
#endif // robust
}


void thash_update(void *hash_state, const unsigned char *in, int block_idx, const unsigned char *pub_seed, uint32_t addr[8]) {
#if defined(VARIANT_SHA2_SIMPLE) || defined(VARIANT_SHAKE_SIMPLE)
  if (PARAMSET_TYPE == SIMPLE)
  {
    #if defined(VARIANT_SHA2_SIMPLE)
    if (PARAMSET_HASH_FUNC == HASH_SHA)
    {
      thash_update_sha256_simple((sha256_inc_state_t*)hash_state, in, block_idx, pub_seed, addr);
    }
    #endif
    #if defined(VARIANT_SHAKE_SIMPLE)
    if (PARAMSET_HASH_FUNC == HASH_SHAKE)
    {
      thash_update_shake256_simple((shake256_inc_state_t*)hash_state,in, block_idx, pub_seed, addr);
    }
    #endif
  }
#endif // simple
#if defined(VARIANT_SHA2_ROBUST) || defined(VARIANT_SHAKE_ROBUST)
  if (PARAMSET_TYPE == ROBUST)
  {
    #if defined(VARIANT_SHA2_ROBUST)
    if (PARAMSET_HASH_FUNC == HASH_SHA)
    {
      thash_update_sha256((sha256_inc_state_t*)hash_state, &((sha256_inc_state_t*)hash_state)->mfg_state, in, block_idx, pub_seed, addr);
    }
    #endif
    #if defined(VARIANT_SHAKE_ROBUST)
    if (PARAMSET_HASH_FUNC == HASH_SHAKE)
    {
      thash_update_shake256((shake256_inc_state_t*)hash_state, in, block_idx, pub_seed, addr);
    }
    #endif
  }
#endif // robust
}



void thash_finalize(unsigned char* out, void* hash_state)
{
#if defined(VARIANT_SHA2_SIMPLE) || defined(VARIANT_SHAKE_SIMPLE)
  if (PARAMSET_TYPE == SIMPLE)
  {
    #if defined(VARIANT_SHA2_SIMPLE)
    if (PARAMSET_HASH_FUNC == HASH_SHA)
    {
      thash_finalize_sha256_simple(out, (sha256_inc_state_t*)hash_state);
    }
    #endif
    #if defined(VARIANT_SHAKE_SIMPLE)
    if (PARAMSET_HASH_FUNC == HASH_SHAKE)
    {
      thash_finalize_shake256_simple(out, (shake256_inc_state_t*)hash_state);
    }
    #endif
  }
#endif // simple
#if defined(VARIANT_SHA2_ROBUST) || defined(VARIANT_SHAKE_ROBUST)
  if (PARAMSET_TYPE == ROBUST)
  {
    #if defined(VARIANT_SHA2_ROBUST)
    if (PARAMSET_HASH_FUNC == HASH_SHA)
    {
      thash_finalize_sha256(out, (sha256_inc_state_t*)hash_state);
    }
    #endif
    #if defined(VARIANT_SHAKE_ROBUST)
    if (PARAMSET_HASH_FUNC == HASH_SHAKE)
    {
      thash_finalize_shake256(out, (shake256_inc_state_t *)hash_state);
    }
    #endif
  }
#endif // robust
}
