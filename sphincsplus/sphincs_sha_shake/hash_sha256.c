#if defined(VARIANT_SHA2_SIMPLE) || defined(VARIANT_SHA2_ROBUST)

#include <stdint.h>
#include <string.h>

#include "address.h"
#include "utils.h"
#include "params.h"
#include "hash.h"
#include "sha256.h"
#include "hash.h"

/* For SHA256, there is no immediate reason to initialize at the start,
   so this function is an empty operation. */
void initialize_hash_function_sha256(const unsigned char *pub_seed,
                              const unsigned char *sk_seed)
{
    seed_state(pub_seed);
    (void)sk_seed; /* Suppress an 'unused parameter' warning. */
}

/*
 * Computes PRF(key, addr), given a secret key of SPX_N bytes and an address
 */
void prf_addr_sha256(unsigned char *out, const unsigned char *key,
              const uint32_t addr[8])
{
    unsigned char buf[SPX_N + SPX_SHA256_ADDR_BYTES];
    unsigned char outbuf[SPX_SHA256_OUTPUT_BYTES];

    memcpy(buf, key, SPX_N);
    memcpy(buf + SPX_N, addr, SPX_SHA256_ADDR_BYTES);

    sha256(outbuf, buf, SPX_N + SPX_SHA256_ADDR_BYTES);
    memcpy(out, outbuf, SPX_N);
}

void gen_message_random_shaX(unsigned char *R, const unsigned char *sk_prf,
                        const unsigned char *optrand,
                        const unsigned char *m, unsigned long long mlen)
{
  if(SPX_N != 32) {
    gen_message_random_sha256(R, sk_prf, optrand, m, mlen);
  }
  else
  {
    gen_message_random_sha512(R, sk_prf, optrand, m, mlen);
  }
}

/**
 * Computes the message-dependent randomness R, using a secret seed as a key
 * for HMAC, and an optional randomization value prefixed to the message.
 * This requires m to have at least SPX_SHA256_BLOCK_BYTES + SPX_N space
 * available in front of the pointer, i.e. before the message to use for the
 * prefix. This is necessary to prevent having to move the message around (and
 * allocate memory for it).
 */
void gen_message_random_sha256(unsigned char *R, const unsigned char *sk_prf,
                        const unsigned char *optrand,
                        const unsigned char *m, unsigned long long mlen)
{
    unsigned char buf[SPX_SHA256_BLOCK_BYTES + SPX_SHA256_OUTPUT_BYTES];
    uint8_t state[40];
    int i;

    /* This implements HMAC-SHA256 */
    for (i = 0; i < (int)SPX_N; i++) {
        buf[i] = 0x36 ^ sk_prf[i];
    }
    memset(buf + SPX_N, 0x36, SPX_SHA256_BLOCK_BYTES - SPX_N);

    sha256_inc_init(state);
    sha256_inc_blocks(state, buf, 1);

    memcpy(buf, optrand, SPX_N);

    /* If optrand + message cannot fill up an entire block */
    if (SPX_N + mlen < SPX_SHA256_BLOCK_BYTES) {
        memcpy(buf + SPX_N, m, mlen);
        sha256_inc_finalize(buf + SPX_SHA256_BLOCK_BYTES, state,
                            buf, mlen + SPX_N);
    }
    /* Otherwise first fill a block, so that finalize only uses the message */
    else {
        memcpy(buf + SPX_N, m, SPX_SHA256_BLOCK_BYTES - SPX_N);
        sha256_inc_blocks(state, buf, 1);

        m += SPX_SHA256_BLOCK_BYTES - SPX_N;
        mlen -= SPX_SHA256_BLOCK_BYTES - SPX_N;
        sha256_inc_finalize(buf + SPX_SHA256_BLOCK_BYTES, state, m, mlen);
    }

    for (i = 0; i < (int)SPX_N; i++) {
        buf[i] = 0x5c ^ sk_prf[i];
    }
    memset(buf + SPX_N, 0x5c, SPX_SHA256_BLOCK_BYTES - SPX_N);

    sha256(buf, buf, SPX_SHA256_BLOCK_BYTES + SPX_SHA256_OUTPUT_BYTES);
    memcpy(R, buf, SPX_N);
}

/**
 * Computes the message-dependent randomness R, using a secret seed as a key
 * for HMAC, and an optional randomization value prefixed to the message.
 * This requires m to have at least SPX_SHA256_BLOCK_BYTES + SPX_N space
 * available in front of the pointer, i.e. before the message to use for the
 * prefix. This is necessary to prevent having to move the message around (and
 * allocate memory for it).
 */
void gen_message_random_sha512(unsigned char *R, const unsigned char *sk_prf,
                        const unsigned char *optrand,
                        const unsigned char *m, unsigned long long mlen)
{
    unsigned char buf[SPX_SHA512_BLOCK_BYTES + SPX_SHA512_OUTPUT_BYTES];
    uint8_t state[8 + SPX_SHA512_OUTPUT_BYTES];
    int i;

    /* This implements HMAC-SHA512 */
    for (i = 0; i < (int)SPX_N; i++) {
        buf[i] = 0x36 ^ sk_prf[i];
    }
    memset(buf + SPX_N, 0x36, SPX_SHA512_BLOCK_BYTES - SPX_N);

    sha512_inc_init(state);
    sha512_inc_blocks(state, buf, 1);

    memcpy(buf, optrand, SPX_N);

    /* If optrand + message cannot fill up an entire block */
    if (SPX_N + mlen < SPX_SHA512_BLOCK_BYTES) {
        memcpy(buf + SPX_N, m, mlen);
        sha512_inc_finalize(buf + SPX_SHA512_BLOCK_BYTES, state,
                            buf, mlen + SPX_N);
    }
    /* Otherwise first fill a block, so that finalize only uses the message */
    else {
        memcpy(buf + SPX_N, m, SPX_SHA512_BLOCK_BYTES - SPX_N);
        sha512_inc_blocks(state, buf, 1);

        m += SPX_SHA512_BLOCK_BYTES - SPX_N;
        mlen -= SPX_SHA512_BLOCK_BYTES - SPX_N;
        sha512_inc_finalize(buf + SPX_SHA512_BLOCK_BYTES, state, m, mlen);
    }

    for (i = 0; i < (int)SPX_N; i++) {
        buf[i] = 0x5c ^ sk_prf[i];
    }
    memset(buf + SPX_N, 0x5c, SPX_SHA512_BLOCK_BYTES - SPX_N);

    sha512(buf, buf, SPX_SHA512_BLOCK_BYTES + SPX_SHA512_OUTPUT_BYTES);
    memcpy(R, buf, SPX_N);
}

void hash_message_shaX(unsigned char *digest, uint64_t *tree, uint32_t *leaf_idx,
                  const unsigned char *R, const unsigned char *pk,
                  const unsigned char *m, unsigned long long mlen)
{
  if(SPX_N != 32) {
    hash_message_sha256(digest, tree, leaf_idx, R, pk, m, mlen);
  }
  else
  {
    hash_message_sha512(digest, tree, leaf_idx, R, pk, m, mlen);
  }
}


#define SPX_TREE_BITS (SPX_TREE_HEIGHT * (SPX_D - 1))
#define SPX_TREE_BYTES ((SPX_TREE_BITS + 7) / 8)
#define SPX_LEAF_BITS SPX_TREE_HEIGHT
#define SPX_LEAF_BYTES ((SPX_LEAF_BITS + 7) / 8)
#define SPX_DGST_BYTES (SPX_FORS_MSG_BYTES + SPX_TREE_BYTES + SPX_LEAF_BYTES)

/**
 * Computes the message hash using R, the public key, and the message.
 * Outputs the message digest and the index of the leaf. The index is split in
 * the tree index and the leaf index, for convenient copying to an address.
 */
void hash_message_sha256(unsigned char *digest, uint64_t *tree, uint32_t *leaf_idx,
                  const unsigned char *R, const unsigned char *pk,
                  const unsigned char *m, unsigned long long mlen)
{
    unsigned char seed[2*SPX_N + SPX_SHA256_OUTPUT_BYTES];

    /* Round to nearest multiple of SPX_SHA256_BLOCK_BYTES */
    unsigned int SPX_INBLOCKS = (((SPX_N + SPX_PK_BYTES + SPX_SHA256_BLOCK_BYTES - 1) & \
                        -SPX_SHA256_BLOCK_BYTES) / SPX_SHA256_BLOCK_BYTES);
    unsigned char inbuf[SPX_INBLOCKS * SPX_SHA256_BLOCK_BYTES];

    unsigned char buf[SPX_DGST_BYTES];
    unsigned char *bufp = buf;
    uint8_t state[40];

    sha256_inc_init(state);

    memcpy(inbuf, R, SPX_N);
    memcpy(inbuf + SPX_N, pk, SPX_PK_BYTES);

    /* If R + pk + message cannot fill up an entire block */
    if (SPX_N + SPX_PK_BYTES + mlen < SPX_INBLOCKS * SPX_SHA256_BLOCK_BYTES) {
        memcpy(inbuf + SPX_N + SPX_PK_BYTES, m, mlen);
        sha256_inc_finalize(seed + 2*SPX_N, state, inbuf, SPX_N + SPX_PK_BYTES + mlen);
    }
    /* Otherwise first fill a block, so that finalize only uses the message */
    else {
        memcpy(inbuf + SPX_N + SPX_PK_BYTES, m,
               SPX_INBLOCKS * SPX_SHA256_BLOCK_BYTES - SPX_N - SPX_PK_BYTES);
        sha256_inc_blocks(state, inbuf, SPX_INBLOCKS);

        m += SPX_INBLOCKS * SPX_SHA256_BLOCK_BYTES - SPX_N - SPX_PK_BYTES;
        mlen -= SPX_INBLOCKS * SPX_SHA256_BLOCK_BYTES - SPX_N - SPX_PK_BYTES;
        sha256_inc_finalize(seed + 2*SPX_N, state, m, mlen);
    }

    // H_msg: MGF1-SHA-256(R || PK.seed || seed)
    memcpy(seed, R, SPX_N);
    memcpy(seed + SPX_N, pk, SPX_N);

    /* By doing this in two steps, we prevent hashing the message twice;
       otherwise each iteration in MGF1 would hash the message again. */
    mgf1_sha256(bufp, SPX_DGST_BYTES, seed, 2*SPX_N + SPX_SHA256_OUTPUT_BYTES);

    memcpy(digest, bufp, SPX_FORS_MSG_BYTES);
    bufp += SPX_FORS_MSG_BYTES;

    *tree = bytes_to_ull(bufp, SPX_TREE_BYTES);
    *tree &= (~(uint64_t)0) >> (64 - SPX_TREE_BITS);
    bufp += SPX_TREE_BYTES;

    *leaf_idx = bytes_to_ull(bufp, SPX_LEAF_BYTES);
    *leaf_idx &= (~(uint32_t)0) >> (32 - SPX_LEAF_BITS);
}

void hash_message_sha512(unsigned char *digest, uint64_t *tree, uint32_t *leaf_idx,
                  const unsigned char *R, const unsigned char *pk,
                  const unsigned char *m, unsigned long long mlen)
{
    unsigned char seed[2*SPX_N + SPX_SHA512_OUTPUT_BYTES];

    /* Round to nearest multiple of SPX_SHA512_BLOCK_BYTES */
    unsigned int SPX_INBLOCKS = (((SPX_N + SPX_PK_BYTES + SPX_SHA512_BLOCK_BYTES - 1) & \
                        -SPX_SHA512_BLOCK_BYTES) / SPX_SHA512_BLOCK_BYTES);
    unsigned char inbuf[SPX_INBLOCKS * SPX_SHA512_BLOCK_BYTES];

    unsigned char buf[SPX_DGST_BYTES];
    unsigned char *bufp = buf;
    uint8_t state[8 + SPX_SHA512_OUTPUT_BYTES];

    sha512_inc_init(state);

    memcpy(inbuf, R, SPX_N);
    memcpy(inbuf + SPX_N, pk, SPX_PK_BYTES);

    /* If R + pk + message cannot fill up an entire block */
    if (SPX_N + SPX_PK_BYTES + mlen < SPX_INBLOCKS * SPX_SHA512_BLOCK_BYTES) {
        memcpy(inbuf + SPX_N + SPX_PK_BYTES, m, mlen);
        sha512_inc_finalize(seed + 2*SPX_N, state, inbuf, SPX_N + SPX_PK_BYTES + mlen);
    }
    /* Otherwise first fill a block, so that finalize only uses the message */
    else {
        memcpy(inbuf + SPX_N + SPX_PK_BYTES, m,
               SPX_INBLOCKS * SPX_SHA512_BLOCK_BYTES - SPX_N - SPX_PK_BYTES);
        sha512_inc_blocks(state, inbuf, SPX_INBLOCKS);

        m += SPX_INBLOCKS * SPX_SHA512_BLOCK_BYTES - SPX_N - SPX_PK_BYTES;
        mlen -= SPX_INBLOCKS * SPX_SHA512_BLOCK_BYTES - SPX_N - SPX_PK_BYTES;
        sha512_inc_finalize(seed + 2*SPX_N, state, m, mlen);
    }

    memcpy(seed, R, SPX_N);
    memcpy(seed + SPX_N, pk, SPX_N);

    /* By doing this in two steps, we prevent hashing the message twice;
       otherwise each iteration in MGF1 would hash the message again. */
    mgf1_sha512(bufp, SPX_DGST_BYTES, seed, 2*SPX_N + SPX_SHA512_OUTPUT_BYTES);

    memcpy(digest, bufp, SPX_FORS_MSG_BYTES);
    bufp += SPX_FORS_MSG_BYTES;

    *tree = bytes_to_ull(bufp, SPX_TREE_BYTES);
    *tree &= (~(uint64_t)0) >> (64 - SPX_TREE_BITS);
    bufp += SPX_TREE_BYTES;

    *leaf_idx = bytes_to_ull(bufp, SPX_LEAF_BYTES);
    *leaf_idx &= (~(uint32_t)0) >> (32 - SPX_LEAF_BITS);
}


#endif
