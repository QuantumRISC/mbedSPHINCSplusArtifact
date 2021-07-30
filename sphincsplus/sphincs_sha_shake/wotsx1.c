#include <stdint.h>
#include <string.h>

#include "fips202.h"
#include "utils.h"
#include "hash.h"
#include "thash.h"
#include "wots.h"
#include "wotsx1.h"
#include "address.h"
#include "params.h"

/*
 * This generates a WOTS public key
 * It also generates the WOTS signature if leaf_info indicates
 * that we're signing with this WOTS key
 */
void wots_gen_leafx1(unsigned char *dest,
                   const unsigned char *sk_seed,
                   const unsigned char *pub_seed,
                   uint32_t leaf_idx, void *v_info) {
    struct leaf_info_x1 *info = v_info;
    uint32_t *leaf_addr = info->leaf_addr;
    uint32_t *pk_addr = info->pk_addr;
    unsigned int i, k;
    unsigned char pk_buffer[ SPX_WOTS_BYTES ];
    unsigned char *buffer;
    uint32_t wots_k_mask;

    if (leaf_idx == info->wots_sign_leaf) {
        /* We're traversing the leaf that's signing; generate the WOTS */
        /* signature */
        wots_k_mask = 0;
    } else {
        /* Nope, we're just generating pk's; turn off the signature logic */
        wots_k_mask = ~0;
    }

    set_keypair_addr( leaf_addr, leaf_idx );
    set_keypair_addr( pk_addr, leaf_idx );

    for (i = 0, buffer = pk_buffer; i < SPX_WOTS_LEN; i++, buffer += SPX_N) {
        uint32_t wots_k = info->wots_steps[i] | wots_k_mask; /* Set wots_k to */
            /* the step if we're generating a signature, ~0 if we're not */

        /* Start with the secret seed */
        set_chain_addr(leaf_addr, i);
        set_hash_addr(leaf_addr, 0);
 
        prf_addr(buffer, sk_seed, leaf_addr);

        /* Iterate down the WOTS chain */
        for (k=0;; k++) {
            /* Check if this is the value that needs to be saved as a */
            /* part of the WOTS signature */
            if (k == wots_k) {
                memcpy( info->wots_sig + i * SPX_N, buffer, SPX_N );
                //TODO: stream_write();
            }

            /* Check if we hit the top of the chain */
            if (k == SPX_WOTS_W - 1) break;

            /* Iterate one step on the chain */
            set_hash_addr(leaf_addr, k);

            thash(buffer, buffer, 1, pub_seed, leaf_addr);
        }
    }

    /* Do the final thash to generate the public keys */
    thash(dest, pk_buffer, SPX_WOTS_LEN, pub_seed, pk_addr);
}


void wots_gen_leafx1_streaming(streaming_ctx_t *ctx, unsigned char *dest,
                   const unsigned char *sk_seed,
                   const unsigned char *pub_seed,
                   uint32_t leaf_idx, void *v_info) {
    struct leaf_info_x1 *info = v_info;
    uint32_t *leaf_addr = info->leaf_addr;
    uint32_t *pk_addr = info->pk_addr;
    unsigned int i, k;
    unsigned char buffer[SPX_N];

    uint32_t wots_k_mask;

    unsigned long hash_state[get_hash_state_size()/sizeof(unsigned long) + 1];

    if (leaf_idx == info->wots_sign_leaf) {
        /* We're traversing the leaf that's signing; generate the WOTS */
        /* signature */
        wots_k_mask = 0;
    } else {
        /* Nope, we're just generating pk's; turn off the signature logic */
        wots_k_mask = ~0;
    }

    set_keypair_addr( leaf_addr, leaf_idx );
    set_keypair_addr( pk_addr, leaf_idx );
    thash_init(hash_state, pub_seed, pk_addr);
    for (i = 0; i < SPX_WOTS_LEN; i++) {
        uint32_t wots_k = info->wots_steps[i] | wots_k_mask; /* Set wots_k to */
            /* the step if we're generating a signature, ~0 if we're not */

        /* Start with the secret seed */
        set_chain_addr(leaf_addr, i);
        set_hash_addr(leaf_addr, 0);

        prf_addr(buffer, sk_seed, leaf_addr);

        /* Iterate down the WOTS chain */
        for (k=0;; k++) {
            /* Check if this is the value that needs to be saved as a */
            /* part of the WOTS signature */
            if (k == wots_k) {
                write_stream(ctx,buffer,SPX_N);
            }
            /* Check if we hit the top of the chain */
            if (k == SPX_WOTS_W - 1) {
              thash_update(hash_state,buffer,1,pub_seed,pk_addr);
              break;
            }
            /* Iterate one step on the chain */
            set_hash_addr(leaf_addr, k);
            thash(buffer, buffer, 1, pub_seed, leaf_addr);
        }

    }

    /* Do the final thash to generate the public keys */
    thash_finalize(dest, hash_state);
}
