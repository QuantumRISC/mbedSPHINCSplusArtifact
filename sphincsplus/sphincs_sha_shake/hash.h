#ifndef SPX_HASH_H
#define SPX_HASH_H

#include <stdint.h>

void prf_addr(unsigned char *out, const unsigned char *key,const uint32_t addr[8]);

void gen_message_random(unsigned char *R, const unsigned char *sk_prf,
                        const unsigned char *optrand,
                        const unsigned char *m, unsigned long long mlen);

void hash_message(unsigned char *digest, uint64_t *tree, uint32_t *leaf_idx,
                           const unsigned char *R, const unsigned char *pk,
                           const unsigned char *m, unsigned long long mlen);

void initialize_hash_function(const unsigned char *pub_seed,
                                       const unsigned char *sk_seed);

unsigned get_hash_state_size();

#if defined(VARIANT_SHAKE_SIMPLE) || defined(VARIANT_SHAKE_ROBUST)
void initialize_hash_function_shake256(const unsigned char *pub_seed,
                              const unsigned char *sk_seed);

void prf_addr_shake256(unsigned char *out, const unsigned char *key,
              const uint32_t addr[8]);

void gen_message_random_shake256(unsigned char *R, const unsigned char *sk_seed,
                        const unsigned char *optrand,
                        const unsigned char *m, unsigned long long mlen);

void hash_message_shake256(unsigned char *digest, uint64_t *tree, uint32_t *leaf_idx,
                  const unsigned char *R, const unsigned char *pk,
                  const unsigned char *m, unsigned long long mlen);
#endif


#if defined(VARIANT_SHA2_SIMPLE) || defined(VARIANT_SHA2_ROBUST)
void initialize_hash_function_sha256(const unsigned char *pub_seed,
                              const unsigned char *sk_seed);

void prf_addr_sha256(unsigned char *out, const unsigned char *key,
              const uint32_t addr[8]);

void gen_message_random_sha256(unsigned char *R, const unsigned char *sk_seed,
                        const unsigned char *optrand,
                        const unsigned char *m, unsigned long long mlen);

void gen_message_random_sha512(unsigned char *R, const unsigned char *sk_seed,
                        const unsigned char *optrand,
                        const unsigned char *m, unsigned long long mlen);

void hash_message_sha256(unsigned char *digest, uint64_t *tree, uint32_t *leaf_idx,
                  const unsigned char *R, const unsigned char *pk,
                  const unsigned char *m, unsigned long long mlen);

void hash_message_shaX(unsigned char *digest, uint64_t *tree, uint32_t *leaf_idx,
                  const unsigned char *R, const unsigned char *pk,
                  const unsigned char *m, unsigned long long mlen);

void gen_message_random_shaX(unsigned char *R, const unsigned char *sk_prf,
                        const unsigned char *optrand,
                        const unsigned char *m, unsigned long long mlen);

void hash_message_sha512(unsigned char *digest, uint64_t *tree, uint32_t *leaf_idx,
                  const unsigned char *R, const unsigned char *pk,
                  const unsigned char *m, unsigned long long mlen);
#endif

#endif
