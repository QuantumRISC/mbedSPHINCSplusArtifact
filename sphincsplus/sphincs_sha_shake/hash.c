#include "hash.h"
#include "params.h"
#include "sha256.h"
#include "fips202.h"
#ifdef TARGET_UNIX
# include <stdio.h>
# include <stdlib.h>
#endif

unsigned get_hash_state_size()
{
#if defined(VARIANT_SHA2_SIMPLE) || defined(VARIANT_SHA2_ROBUST)
  if (PARAMSET_HASH_FUNC == HASH_SHA)  {
    return sizeof(sha256_inc_state_t);
  }
#endif
#if defined(VARIANT_SHAKE_SIMPLE) || defined(VARIANT_SHAKE_ROBUST)
  if (PARAMSET_HASH_FUNC == HASH_SHAKE)
  {
    return sizeof(shake256_inc_state_t);
  }
#endif

#ifdef TARGET_UNIX
  printf("Invalid config!\n");
  exit(-1);
#endif
  return 0;
}

void prf_addr(unsigned char *out, const unsigned char *key, const uint32_t *addr)
{
#if defined(VARIANT_SHA2_SIMPLE) || defined(VARIANT_SHA2_ROBUST)
  if (PARAMSET_HASH_FUNC == HASH_SHA)
  {
    prf_addr_sha256(out, key, addr);
  }
#endif
#if defined(VARIANT_SHAKE_SIMPLE) || defined(VARIANT_SHAKE_ROBUST)
  if (PARAMSET_HASH_FUNC == HASH_SHAKE)
  {
    prf_addr_shake256(out, key, addr);
  }
#endif
}

void gen_message_random(unsigned char *R, const unsigned char *sk_prf,
                             const unsigned char *optrand,
                             const unsigned char *m, unsigned long long mlen)
{
#if defined(VARIANT_SHA2_SIMPLE) || defined(VARIANT_SHA2_ROBUST)
  if(PARAMSET_HASH_FUNC == HASH_SHA)
  {
    gen_message_random_shaX(R, sk_prf, optrand, m, mlen);
  }
#endif
#if defined(VARIANT_SHAKE_SIMPLE) || defined(VARIANT_SHAKE_ROBUST)
  if(PARAMSET_HASH_FUNC == HASH_SHAKE)
  {
    gen_message_random_shake256(R, sk_prf, optrand, m, mlen);
  }
#endif
}

void hash_message(unsigned char *digest, uint64_t *tree, uint32_t *leaf_idx,
                  const unsigned char *R, const unsigned char *pk,
                  const unsigned char *m, unsigned long long mlen)
{
#if defined(VARIANT_SHA2_SIMPLE) || defined(VARIANT_SHA2_ROBUST)
  if(PARAMSET_HASH_FUNC == HASH_SHA)
  {
    hash_message_shaX(digest, tree, leaf_idx, R, pk, m, mlen);
  }
#endif
#if defined(VARIANT_SHAKE_SIMPLE) || defined(VARIANT_SHAKE_ROBUST)
  if(PARAMSET_HASH_FUNC == HASH_SHAKE)
  {
    hash_message_shake256(digest, tree, leaf_idx, R, pk, m, mlen);
  }
#endif
}

void initialize_hash_function(const unsigned char *pub_seed,
                              const unsigned char *sk_seed)
{
#if defined(VARIANT_SHA2_SIMPLE) || defined(VARIANT_SHA2_ROBUST)
  if(PARAMSET_HASH_FUNC == HASH_SHA)
  {
    initialize_hash_function_sha256(pub_seed, sk_seed);
  }
#endif
#if defined(VARIANT_SHAKE_SIMPLE) || defined(VARIANT_SHAKE_ROBUST)
  if(PARAMSET_HASH_FUNC == HASH_SHAKE)
  {
    initialize_hash_function_shake256(pub_seed, sk_seed);
  }
#endif
}