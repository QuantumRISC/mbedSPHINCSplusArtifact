#include <stdint.h>
#include <string.h>

#include "utils.h"
#include "hash.h"
#include "thash.h"
#include "wots.h"
#include "address.h"
#include "params.h"
#include "sha256.h" // for state types
#include "fips202.h" // for state types

/**
 * Computes the chaining function.
 * out and in have to be n-byte arrays.
 *
 * Interprets in as start-th value of the chain.
 * addr has to contain the address of the chain.
 */
static void gen_chain(unsigned char *out, const unsigned char *in,
                      unsigned int start, unsigned int steps,
                      const unsigned char *pub_seed, uint32_t addr[8])
{
    uint32_t i;

    /* Initialize out with the value at position 'start'. */
    memcpy(out, in, SPX_N);

    /* Iterate 'steps' calls to the hash function. */
    for (i = start; i < (start+steps) && i < SPX_WOTS_W; i++) {
        set_hash_addr(addr, i);
        thash(out, out, 1, pub_seed, addr);
    }
}

/**
 * base_w algorithm as described in draft.
 * Interprets an array of bytes as integers in base w.
 * This only works when log_w is a divisor of 8.
 */
static void base_w(unsigned int *output, const int out_len,
                   const unsigned char *input)
{
    int in = 0;
    int out = 0;
    unsigned char total;
    int bits = 0;
    int consumed;

    for (consumed = 0; consumed < out_len; consumed++) {
        if (bits == 0) {
            total = input[in];
            in++;
            bits += 8;
        }
        bits -= SPX_WOTS_LOGW;
        output[out] = (total >> bits) & (SPX_WOTS_W - 1);
        out++;
    }
}

/* Computes the WOTS+ checksum over a message (in base_w). */
static void wots_checksum(unsigned int *csum_base_w,
                          const unsigned int *msg_base_w)
{
    unsigned int csum = 0;
    unsigned char csum_bytes[(SPX_WOTS_LEN2 * SPX_WOTS_LOGW + 7) / 8];
    unsigned int i;

    /* Compute checksum. */
    for (i = 0; i < SPX_WOTS_LEN1; i++) {
        csum += SPX_WOTS_W - 1 - msg_base_w[i];
    }

    /* Convert checksum to base_w. */
    /* Make sure expected empty zero bits are the least significant bits. */
    csum = csum << ((8 - ((SPX_WOTS_LEN2 * SPX_WOTS_LOGW) % 8)) % 8);
    ull_to_bytes(csum_bytes, sizeof(csum_bytes), csum);
    base_w(csum_base_w, SPX_WOTS_LEN2, csum_bytes);
}

/* Takes a message and derives the matching chain lengths. */
void chain_lengths(unsigned int *lengths, const unsigned char *msg)
{
    base_w(lengths, SPX_WOTS_LEN1, msg);
    wots_checksum(lengths + SPX_WOTS_LEN1, lengths);
}


/**
 * Takes a WOTS signature and an n-byte message, computes a WOTS public key.
 *
 * Writes the computed public key to 'pk'.
 */
void wots_pk_from_sig(unsigned char *pk,
                      const unsigned char *sig, const unsigned char *msg,
                      const unsigned char *pub_seed, uint32_t addr[8])
{
    unsigned int lengths[SPX_WOTS_LEN];
    uint32_t i;

    chain_lengths(lengths, msg);

    for (i = 0; i < SPX_WOTS_LEN; i++) {
        set_chain_addr(addr, i);
        gen_chain(pk + i*SPX_N, sig + i*SPX_N,
                  lengths[i], SPX_WOTS_W - 1 - lengths[i], pub_seed, addr);
    }
}

void wots_leaf_from_pk_streaming(streaming_ctx_t *stream_ctx,
                                          unsigned char *leaf, const unsigned char *msg,
                                          const unsigned char *pub_seed, uint32_t *wots_addr, uint32_t *wots_pk_addr)
{
  unsigned int lengths[SPX_WOTS_LEN];
  uint32_t i;
  unsigned char cur_pk[SPX_N];
  unsigned char cur_wots_chain[SPX_N];

  chain_lengths(lengths, msg); /* TODO: remove lengths array */

  unsigned long hash_state[get_hash_state_size()/sizeof(unsigned long) + 1];
  thash_init(hash_state, pub_seed, wots_pk_addr);

  for (i = 0; i < SPX_WOTS_LEN; i++)
  {
    read_stream(stream_ctx, cur_wots_chain, SPX_N);
    set_chain_addr(wots_addr, i);
    gen_chain(cur_pk, cur_wots_chain,
              lengths[i], SPX_WOTS_W - 1 - lengths[i], pub_seed, wots_addr);

    thash_update(hash_state, cur_pk, i, pub_seed, wots_pk_addr);
  }

  thash_finalize(leaf, hash_state);
}