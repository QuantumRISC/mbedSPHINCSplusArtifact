#ifndef SPX_WOTS_H
#define SPX_WOTS_H

#include <stdint.h>
#include "params.h"
#include "api.h"

/**
 * WOTS key generation. Takes a 32 byte seed for the private key, expands it to
 * a full WOTS private key and computes the corresponding public key.
 * It requires the seed pub_seed (used to generate bitmasks and hash keys)
 * and the address of this WOTS key pair.
 *
 * Writes the computed public key to 'pk'.
 */
void wots_gen_pk(unsigned char *pk, const unsigned char *seed,
                 const unsigned char *pub_seed, uint32_t addr[8]);

/**
 * Takes a n-byte message and the 32-byte seed for the private key to compute a
 * signature that is placed at 'sig'.
 */
void wots_sign(unsigned char *sig, const unsigned char *msg,
               const unsigned char *seed, const unsigned char *pub_seed,
               uint32_t addr[8]);

/**
 * Takes a WOTS signature and an n-byte message, computes a WOTS public key.
 *
 * Writes the computed public key to 'pk'.
 */
void wots_pk_from_sig(unsigned char *pk,
                      const unsigned char *sig, const unsigned char *msg,
                      const unsigned char *pub_seed, uint32_t addr[8]);


void chain_lengths(unsigned int *lengths, const unsigned char *msg);

/*
 * Streaming version
 */

void wots_leaf_from_pk_streaming(streaming_ctx_t *stream_ctx,
                                          unsigned char *leaf, const unsigned char *msg,
                                          const unsigned char *pub_seed, uint32_t *wots_addr, uint32_t *wots_pk_addr);

void wots_gen_leaf_streaming_from_sk(unsigned char *leaf, const unsigned char *sk_seed,
                                              const unsigned char *pub_seed, uint32_t wots_pk_addr[8], uint32_t wots_addr[8]);




#endif
