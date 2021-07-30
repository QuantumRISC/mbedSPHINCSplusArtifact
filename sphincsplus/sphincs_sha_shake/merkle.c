#include <stdint.h>
#include <string.h>

#include "thash.h"
#include "utils.h"
#include "utilsx1.h"
#include "wots.h"
#include "wotsx1.h"
#include "merkle.h"
#include "address.h"
#include "params.h"

/*
 * This generates a Merkle signature (WOTS signature followed by the Merkle
 * authentication path).  This is in this file because most of the complexity
 * is involved with the WOTS signature; the Merkle authentication path logic
 * is mostly hidden in treehashx4
 */ 
void merkle_sign(uint8_t *sig, unsigned char *root,
                 const unsigned char *sk_seed, const unsigned char *pub_seed,
                 uint32_t wots_addr[8], uint32_t tree_addr[8],
                 uint32_t idx_leaf)
{
    unsigned char *auth_path = sig + SPX_WOTS_BYTES;
    struct leaf_info_x1 info = { 0 };
    unsigned steps[ SPX_WOTS_LEN ];

    info.wots_sig = sig;
    chain_lengths(steps, root);
    info.wots_steps = (uint32_t*)steps;

    set_type(&tree_addr[0], SPX_ADDR_TYPE_HASHTREE);
    set_type(&info.leaf_addr[0], SPX_ADDR_TYPE_WOTS);
    set_type(&info.pk_addr[0], SPX_ADDR_TYPE_WOTSPK);
    copy_subtree_addr(&info.leaf_addr[0], wots_addr);
    copy_subtree_addr(&info.pk_addr[0], wots_addr);

    info.wots_sign_leaf = idx_leaf;

    treehashx1(root, auth_path, sk_seed, pub_seed,
                idx_leaf, 0,
                SPX_TREE_HEIGHT,
                wots_gen_leafx1,
                tree_addr, &info);
}

/* Compute root node of the top-most subtree. */
void merkle_gen_root(unsigned char *root,
           const unsigned char *sk_seed, const unsigned char *pub_seed)
{
    /* We do not need the auth path in key generation, but it simplifies the
       code to have just one treehash routine that computes both root and path
       in one function. */
    unsigned char auth_path[SPX_TREE_HEIGHT * SPX_N + SPX_WOTS_BYTES];
    uint32_t top_tree_addr[8] = {0};
    uint32_t wots_addr[8] = {0};

    set_layer_addr(top_tree_addr, SPX_D - 1);
    set_layer_addr(wots_addr, SPX_D - 1);

    merkle_sign(auth_path, root, sk_seed, pub_seed,
                wots_addr, top_tree_addr,
                ~0 /* ~0 means "don't bother generating an auth path */ );

}



void merkle_sign_streaming(streaming_ctx_t *ctx, unsigned char *root,
                 const unsigned char *sk_seed, const unsigned char *pub_seed,
                 uint32_t wots_addr[8], uint32_t tree_addr[8],
                 uint32_t idx_leaf)
{
    unsigned char current[2*SPX_N];

    struct leaf_info_x1 info = { 0 };
    unsigned steps[ SPX_WOTS_LEN ];

    chain_lengths(steps, root);
    info.wots_steps = (uint32_t*)steps;

    set_type(&tree_addr[0], SPX_ADDR_TYPE_HASHTREE);
    set_type(&info.leaf_addr[0], SPX_ADDR_TYPE_WOTS);
    set_type(&info.pk_addr[0], SPX_ADDR_TYPE_WOTSPK);
    copy_subtree_addr(&info.leaf_addr[0], wots_addr);
    copy_subtree_addr(&info.pk_addr[0], wots_addr);

    // for key generation: only generate the root.
    info.wots_sign_leaf = idx_leaf;
    if (idx_leaf == ~0)
    {
      treehashx1_streaming(ctx, root, 0, sk_seed, pub_seed,
                idx_leaf, 0,
                SPX_TREE_HEIGHT,
                wots_gen_leafx1_streaming,
                tree_addr, &info);
      return;
    }

    /* streaming authentication path: generate in order */
    wots_gen_leafx1_streaming(ctx,current + (SPX_N * (idx_leaf&1)), sk_seed, pub_seed, idx_leaf, &info);
    uint8_t *thash_target;
    for (unsigned int h = 0; h < SPX_TREE_HEIGHT; h++) {
      uint8_t *target = current +  SPX_N * (((idx_leaf>>h)&1) ^ 1);
      if (h == 0) {
        wots_gen_leafx1_streaming(ctx,target, sk_seed, pub_seed, idx_leaf^1, &info);
      } else {
      treehashx1_streaming(ctx, target, 0, sk_seed, pub_seed,
                  0,((idx_leaf>>h)^1)<<h,
                  h,
                  wots_gen_leafx1_streaming,
                  tree_addr, &info);
      }
      write_stream(ctx,target,SPX_N);
      thash_target = current +  SPX_N * (((idx_leaf >> (h+1))&1));
      set_tree_height(tree_addr, h+1);
      set_tree_index(tree_addr, (idx_leaf>>(h+1)) );
      thash(thash_target, current, 2 , pub_seed, tree_addr); 
    }
    memcpy(root,thash_target,SPX_N);
}

/* Compute root node of the top-most subtree. */
void merkle_gen_root_streaming(streaming_ctx_t *ctx,unsigned char *root,
           const unsigned char *sk_seed, const unsigned char *pub_seed)
{
    uint32_t top_tree_addr[8] = {0};
    uint32_t wots_addr[8] = {0};

    set_layer_addr(top_tree_addr, SPX_D - 1);
    set_layer_addr(wots_addr, SPX_D - 1);

    //merkle_sign(auth_path, root, sk_seed, pub_seed,
    //            wots_addr, top_tree_addr,
    //            ~0 /* ~0 means "don't bother generating an auth path */ );
    merkle_sign_streaming(ctx, root, sk_seed, pub_seed, wots_addr, top_tree_addr, ~0);
}
