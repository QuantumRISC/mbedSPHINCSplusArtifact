#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "fors.h"
#include "utils.h"
#include "utilsx1.h"
#include "hash.h"
#include "thash.h"
#include "address.h"
#include "sha256.h" // for state types
#include "fips202.h" // for state types

static void fors_gen_sk(unsigned char *sk, const unsigned char *sk_seed,
                        uint32_t fors_leaf_addr[8])
{
  prf_addr(sk, sk_seed, fors_leaf_addr);
}

static void fors_sk_to_leaf(unsigned char *leaf, const unsigned char *sk,
                            const unsigned char *pub_seed,
                            uint32_t fors_leaf_addr[8])
{
  thash(leaf, sk, 1, pub_seed, fors_leaf_addr);
}


/**
 * Interprets m as SPX_FORS_HEIGHT-bit unsigned integers.
 * Assumes m contains at least SPX_FORS_HEIGHT * SPX_FORS_TREES bits.
 * Assumes indices has space for SPX_FORS_TREES integers.
 */
static void message_to_indices(uint32_t *indices, const unsigned char *m)
{
    unsigned int i, j;
    unsigned int offset = 0;

    for (i = 0; i < SPX_FORS_TREES; i++) {
        indices[i] = 0;
        for (j = 0; j < SPX_FORS_HEIGHT; j++) {
            indices[i] ^= ((m[offset >> 3] >> (offset & 0x7)) & 0x1) << j;
            offset++;
        }
    }
}


struct fors_gen_leaf_info {
    uint32_t leaf_addrx[8];
};
void fors_gen_leafx1(unsigned char *leaf,
                            const unsigned char *sk_seed,
                            const unsigned char *pub_seed,
                            uint32_t addr_idx, void *info)
{
    struct fors_gen_leaf_info *fors_info = info;
    uint32_t *fors_leaf_addr = fors_info->leaf_addrx;

    /* Only set the parts that the caller doesn't set */
    set_tree_index(fors_leaf_addr, addr_idx);

    fors_gen_sk(leaf, sk_seed, fors_leaf_addr);
    fors_sk_to_leaf(leaf, leaf,
                    pub_seed, fors_leaf_addr);
}


/**
 * Signs a message m, deriving the secret key from sk_seed and the FTS address.
 * Assumes m contains at least SPX_FORS_HEIGHT * SPX_FORS_TREES bits.
 */
void fors_sign(unsigned char *sig, unsigned char *pk,
               const unsigned char *m,
               const unsigned char *sk_seed, const unsigned char *pub_seed,
               const uint32_t fors_addr[8])
{
    uint32_t indices[SPX_FORS_TREES];
    unsigned char roots[SPX_FORS_TREES * SPX_N];
    uint32_t fors_tree_addr[8] = {0};
    struct fors_gen_leaf_info fors_info = {0};
    uint32_t *fors_leaf_addr = fors_info.leaf_addrx;
    uint32_t fors_pk_addr[8] = {0};
    uint32_t idx_offset;
    unsigned int i;

    copy_keypair_addr(fors_tree_addr, fors_addr);
    set_type(fors_tree_addr, SPX_ADDR_TYPE_FORSTREE);
    copy_keypair_addr(fors_leaf_addr, fors_addr);
    set_type(fors_leaf_addr, SPX_ADDR_TYPE_FORSTREE);

    copy_keypair_addr(fors_pk_addr, fors_addr);
    set_type(fors_pk_addr, SPX_ADDR_TYPE_FORSPK);

    message_to_indices(indices, m);

    for (i = 0; i < SPX_FORS_TREES; i++) {
        idx_offset = i * (1 << SPX_FORS_HEIGHT);

        set_tree_height(fors_tree_addr, 0);
        set_tree_index(fors_tree_addr, indices[i] + idx_offset);

        /* Include the secret key part that produces the selected leaf node. */
        fors_gen_sk(sig, sk_seed, fors_tree_addr);
        sig += SPX_N;

        /* Compute the authentication path for this leaf node. */
        treehashx1(roots + i*SPX_N, sig, sk_seed, pub_seed,
                 indices[i], idx_offset, SPX_FORS_HEIGHT, fors_gen_leafx1,
                 fors_tree_addr, &fors_info);
        sig += SPX_N * SPX_FORS_HEIGHT;
    }

    /* Hash horizontally across all tree roots to derive the public key. */
    thash(pk, roots, SPX_FORS_TREES, pub_seed, fors_pk_addr);
}


/**
 * Streaming version
 *
 * Signs a message m, deriving the secret key from sk_seed and the FTS address.
 * Assumes m contains at least SPX_FORS_HEIGHT * SPX_FORS_TREES bits.
 */
void fors_sign_streaming(streaming_ctx_t *stream_ctx, unsigned char *pk,
                                  const unsigned char *m,
                                  const unsigned char *sk_seed, const unsigned char *pub_seed,
                                  const uint32_t fors_addr[8])
{
  uint32_t indices[SPX_FORS_TREES];
  unsigned char cur_root[SPX_N];
  uint32_t fors_tree_addr[8] = {0};
  struct fors_gen_leaf_info fors_info = {0};
  uint32_t *fors_leaf_addr = fors_info.leaf_addrx;
  uint32_t fors_pk_addr[8] = {0};
  uint32_t idx_offset;
  unsigned int i;

  copy_keypair_addr(fors_tree_addr, fors_addr);
  set_type(fors_tree_addr, SPX_ADDR_TYPE_FORSTREE);
  copy_keypair_addr(fors_leaf_addr, fors_addr);
  set_type(fors_leaf_addr, SPX_ADDR_TYPE_FORSTREE);
  copy_keypair_addr(fors_pk_addr, fors_addr);
  set_type(fors_pk_addr, SPX_ADDR_TYPE_FORSPK);

  unsigned long hash_state[get_hash_state_size()/sizeof(unsigned long) + 1];
  thash_init(hash_state, pub_seed, fors_pk_addr);

  message_to_indices(indices, m);

  for (i = 0; i < SPX_FORS_TREES; i++) {
    idx_offset = i * (1 << SPX_FORS_HEIGHT);

    set_tree_height(fors_tree_addr, 0);
    set_tree_index(fors_tree_addr, indices[i] + idx_offset);

    /* Include the secret key part that produces the selected leaf node. */
    /* NOTE: use cur_root as tmp buffer for the secret key part */
    fors_gen_sk(cur_root, sk_seed, fors_tree_addr);  //
    write_stream(stream_ctx, cur_root, SPX_N);

    uint8_t current[2*SPX_N];
    uint32_t idx_leaf = indices[i];
    uint8_t *thash_target;
    fors_gen_leafx1(current + (SPX_N*(idx_leaf&1)), sk_seed, pub_seed,idx_leaf+idx_offset, &fors_info);
    for (unsigned int h = 0; h < SPX_FORS_HEIGHT; h++) {
       uint8_t *target = current +  SPX_N * (((idx_leaf>>h)&1) ^ 1);
       treehashx1( target, 0, sk_seed, pub_seed,
                   0,
                   idx_offset  + (((idx_leaf >> h)^1)<<h), // use the offset to start at the right subtree
                   h,
                   fors_gen_leafx1,
                   fors_tree_addr, &fors_info);
       write_stream(stream_ctx,target,SPX_N);
       thash_target = current +  SPX_N * (((idx_leaf >> (h+1))&1));
       set_tree_height(fors_tree_addr, h+1);
       uint32_t treeidxr = (idx_offset>>(h+1));
       uint32_t treeidxl = (idx_leaf>>(h+1));
       uint32_t treeidx = treeidxl+treeidxr;
       set_tree_index(fors_tree_addr, treeidx);
       thash(thash_target, current, 2 , pub_seed, fors_tree_addr);
    }
    memcpy(cur_root,thash_target,SPX_N);
    thash_update(hash_state, cur_root, i, pub_seed, fors_pk_addr);
  }

  /* Hash horizontally across all tree roots to derive the public key. */
  thash_finalize(pk, hash_state);
}

/**
 * Derives the FORS public key from a signature.
 * This can be used for verification by comparing to a known public key, or to
 * subsequently verify a signature on the derived public key. The latter is the
 * typical use-case when used as an FTS below an OTS in a hypertree.
 * Assumes m contains at least SPX_FORS_HEIGHT * SPX_FORS_TREES bits.
 */
void fors_pk_from_sig(unsigned char *pk,
                      const unsigned char *sig, const unsigned char *m,
                      const unsigned char *pub_seed,
                      const uint32_t fors_addr[8])
{
    uint32_t indices[SPX_FORS_TREES];
    unsigned char roots[SPX_FORS_TREES * SPX_N];
    unsigned char leaf[SPX_N];
    uint32_t fors_tree_addr[8] = {0};
    uint32_t fors_pk_addr[8] = {0};
    uint32_t idx_offset;
    unsigned int i;

    copy_keypair_addr(fors_tree_addr, fors_addr);
    copy_keypair_addr(fors_pk_addr, fors_addr);

    set_type(fors_tree_addr, SPX_ADDR_TYPE_FORSTREE);
    set_type(fors_pk_addr, SPX_ADDR_TYPE_FORSPK);

    message_to_indices(indices, m);

    for (i = 0; i < SPX_FORS_TREES; i++) {
        idx_offset = i * (1 << SPX_FORS_HEIGHT);

        set_tree_height(fors_tree_addr, 0);
        set_tree_index(fors_tree_addr, indices[i] + idx_offset);

        /* Derive the leaf from the included secret key part. */
        fors_sk_to_leaf(leaf, sig, pub_seed, fors_tree_addr);
        sig += SPX_N;

        /* Derive the corresponding root node of this tree. */
        compute_root(roots + i*SPX_N, leaf, indices[i], idx_offset,
                     sig, SPX_FORS_HEIGHT, pub_seed, fors_tree_addr);
        sig += SPX_N * SPX_FORS_HEIGHT;
    }

    /* Hash horizontally across all tree roots to derive the public key. */
    thash(pk, roots, SPX_FORS_TREES, pub_seed, fors_pk_addr);
}


void fors_pk_from_sig_streaming(streaming_ctx_t *stream_ctx, unsigned char *pk,
                                       const unsigned char *m,
                                       const unsigned char *pub_seed,
                                       const uint32_t fors_addr[8])
{
  uint32_t indices[SPX_FORS_TREES];
//  unsigned char roots[SPX_FORS_TREES * SPX_N];
  unsigned char cur_root[SPX_N];
  unsigned char leaf[SPX_N];
  uint32_t fors_tree_addr[8] = {0};
  uint32_t fors_pk_addr[8] = {0};
  uint32_t idx_offset;
  unsigned int i;

  copy_keypair_addr(fors_tree_addr, fors_addr);
  copy_keypair_addr(fors_pk_addr, fors_addr);

  set_type(fors_tree_addr, SPX_ADDR_TYPE_FORSTREE);
  set_type(fors_pk_addr, SPX_ADDR_TYPE_FORSPK);

  message_to_indices(indices, m);

  unsigned long hash_state[get_hash_state_size()/sizeof(unsigned long) + 1];
  thash_init(hash_state, pub_seed, fors_pk_addr);

  for (i = 0; i < SPX_FORS_TREES; i++) {
    idx_offset = i * (1 << SPX_FORS_HEIGHT);

    set_tree_height(fors_tree_addr, 0);
    set_tree_index(fors_tree_addr, indices[i] + idx_offset);

    /* Derive the leaf from the included secret key part. */
    read_stream(stream_ctx, leaf, SPX_N);
    fors_sk_to_leaf(leaf, leaf, pub_seed, fors_tree_addr);

    /* Derive the corresponding root node of this tree. */
    /*
    unsigned char sig[SPX_N * SPX_FORS_HEIGHT];
    read_stream(stream_ctx, sig, SPX_N * SPX_FORS_HEIGHT);
    compute_root(roots + i*SPX_N, leaf, indices[i], idx_offset,
                 sig, SPX_FORS_HEIGHT, pub_seed, fors_tree_addr);
                 */
    compute_root_streaming(stream_ctx, cur_root, leaf, indices[i], idx_offset, SPX_FORS_HEIGHT,
                           pub_seed, fors_tree_addr);

    thash_update(hash_state, cur_root, i, pub_seed, fors_pk_addr);
  }

  /* Hash horizontally across all tree roots to derive the public key. */
//  thash(pk, roots, SPX_FORS_TREES, pub_seed, fors_pk_addr);
  thash_finalize(pk, hash_state);
}
