#include <stddef.h>
#include <string.h>
#include <stdint.h>

#include "api.h"
#include "merkle.h"
#include "params.h"
#include "wots.h"
#include "fors.h"
#include "hash.h"
#include "thash.h"
#include "address.h"
#include "rng.h"
#include "utils.h"

/*
 * Returns the length of a secret key, in bytes
 */
unsigned long long crypto_sign_secretkeybytes(void)
{
    return CRYPTO_SECRETKEYBYTES;
}

/*
 * Returns the length of a public key, in bytes
 */
unsigned long long crypto_sign_publickeybytes(void)
{
    return CRYPTO_PUBLICKEYBYTES;
}

/*
 * Returns the length of a signature, in bytes
 */
unsigned long long crypto_sign_bytes(void)
{
    return CRYPTO_BYTES;
}

/*
 * Returns the length of the seed required to generate a key pair, in bytes
 */
unsigned long long crypto_sign_seedbytes(void)
{
    return CRYPTO_SEEDBYTES;
}

/*
 * Generates an SPX key pair given a seed of length
 * Format sk: [SK_SEED || SK_PRF || PUB_SEED || root]
 * Format pk: [PUB_SEED || root]
 */
int crypto_sign_seed_keypair(unsigned char *pk, unsigned char *sk,
                             const unsigned char *seed)
{
  /* Initialize SK_SEED, SK_PRF and PUB_SEED from seed. */
  memcpy(sk, seed, CRYPTO_SEEDBYTES);

  memcpy(pk, sk + 2*SPX_N, SPX_N);

  /* This hook allows the hash function instantiation to do whatever
     preparation or computation it needs, based on the public seed. */
  initialize_hash_function(pk, sk);

  /* Compute root node of the top-most subtree. */
  merkle_gen_root(sk+3*SPX_N, sk,sk+2*SPX_N);
  memcpy(pk + SPX_N, sk + 3*SPX_N, SPX_N);

  return 0;
}

int crypto_sign_seed_keypair_streaming(unsigned char *pk, unsigned char *sk,
                             const unsigned char *seed)
{

  /* We do not need the auth path in key generation, but it simplifies the
     code to have just one treehash routine that computes both root and path
     in one function. */


  /* Initialize SK_SEED, SK_PRF and PUB_SEED from seed. */
  memcpy(sk, seed, CRYPTO_SEEDBYTES);

  memcpy(pk, sk + 2*SPX_N, SPX_N);

  /* This hook allows the hash function instantiation to do whatever
     preparation or computation it needs, based on the public seed. */
  initialize_hash_function(pk, sk);


  /* Compute root node of the top-most subtree. */
  merkle_gen_root_streaming(0,sk+3*SPX_N,sk,sk+2*SPX_N);
  memcpy(pk + SPX_N, sk + 3*SPX_N, SPX_N);

  return 0;
}

/*
 * Generates an SPX key pair.
 * Format sk: [SK_SEED || SK_PRF || PUB_SEED || root]
 * Format pk: [PUB_SEED || root]
 */
int crypto_sign_keypair(unsigned char *pk, unsigned char *sk)
{
  unsigned char seed[CRYPTO_SEEDBYTES];
  randombytes(seed, CRYPTO_SEEDBYTES);
  crypto_sign_seed_keypair(pk, sk, seed);

  return 0;
}

int crypto_sign_keypair_streaming(unsigned char *pk, unsigned char *sk)
{
  unsigned char seed[CRYPTO_SEEDBYTES];
  randombytes(seed, CRYPTO_SEEDBYTES);
  crypto_sign_seed_keypair_streaming(pk, sk, seed);

  return 0;
}

/**
 * Returns an array containing a detached signature.
 */
int crypto_sign_signature(uint8_t *sig, size_t *siglen,
                          const uint8_t *m, size_t mlen, const uint8_t *sk)
{
    const unsigned char *sk_seed = sk;
    const unsigned char *sk_prf = sk + SPX_N;
    const unsigned char *pk = sk + 2*SPX_N;
    const unsigned char *pub_seed = pk;

    unsigned char optrand[SPX_N];
    unsigned char mhash[SPX_FORS_MSG_BYTES];
    unsigned char root[SPX_N];
    unsigned long long i;
    uint64_t tree;
    uint32_t idx_leaf;
    uint32_t wots_addr[8] = {0};
    uint32_t tree_addr[8] = {0};

    /* This hook allows the hash function instantiation to do whatever
       preparation or computation it needs, based on the public seed. */
    initialize_hash_function(pub_seed, sk_seed);


    set_type(wots_addr, SPX_ADDR_TYPE_WOTS);
    set_type(tree_addr, SPX_ADDR_TYPE_HASHTREE);

    /* Optionally, signing can be made non-deterministic using optrand.
       This can help counter side-channel attacks that would benefit from
       getting a large number of traces when the signer uses the same nodes. */
    randombytes(optrand, SPX_N);
    /* Compute the digest randomization value. */
    gen_message_random(sig, sk_prf, optrand, m, mlen);


    /* Derive the message digest and leaf index from R, PK and M. */
    hash_message(mhash, &tree, &idx_leaf, sig, pk, m, mlen);
    sig += SPX_N;

    set_tree_addr(wots_addr, tree);
    set_keypair_addr(wots_addr, idx_leaf);

    /* Sign the message hash using FORS. */
    fors_sign(sig, root, mhash, sk_seed, pub_seed, wots_addr);
    sig += SPX_FORS_BYTES;

    for (i = 0; i < SPX_D; i++) {
        set_layer_addr(tree_addr, i);
        set_tree_addr(tree_addr, tree);

        copy_subtree_addr(wots_addr, tree_addr);
        set_keypair_addr(wots_addr, idx_leaf);
    
        merkle_sign(sig,root,sk_seed,pub_seed,wots_addr,tree_addr,idx_leaf);
        sig += SPX_WOTS_BYTES + SPX_TREE_HEIGHT * SPX_N;

        /* Update the indices for the next layer. */
        idx_leaf = (tree & ((1 << SPX_TREE_HEIGHT)-1));
        tree = tree >> SPX_TREE_HEIGHT;
    }

    *siglen = SPX_BYTES;

    return 0;
}

int crypto_sign_signature_streaming(streaming_ctx_t *stream_ctx,
                                    const uint8_t *m, size_t mlen, const uint8_t *sk)

{
  const unsigned char *sk_seed = sk;
  const unsigned char *sk_prf = sk + SPX_N;
  const unsigned char *pk = sk + 2*SPX_N;
  const unsigned char *pub_seed = pk;

  unsigned char optrand[SPX_N];
  unsigned char mhash[SPX_FORS_MSG_BYTES];
  unsigned char root[SPX_N];
  unsigned long long i;
  uint64_t tree;
  uint32_t idx_leaf;
  uint32_t wots_addr[8] = {0};
  uint32_t tree_addr[8] = {0};

  /* This hook allows the hash function instantiation to do whatever
     preparation or computation it needs, based on the public seed. */
  initialize_hash_function(pub_seed, sk_seed);

  set_type(wots_addr, SPX_ADDR_TYPE_WOTS);
  set_type(tree_addr, SPX_ADDR_TYPE_HASHTREE);

  /* Optionally, signing can be made non-deterministic using optrand.
     This can help counter side-channel attacks that would benefit from
     getting a large number of traces when the signer uses the same nodes. */
  randombytes(optrand, SPX_N);
  /* Compute the digest randomization value.
   * NOTE: first SPX_N bytes of sig: R[SPX_N]
   * NOTE: re-use "root" buffer for  R
   */
  gen_message_random(root, sk_prf, optrand, m, mlen);

  write_stream(stream_ctx, root, SPX_N);

  /* Derive the message digest and leaf index from R, PK and M. */
  /* NOTE: root = R */
  hash_message(mhash, &tree, &idx_leaf, root, pk, m, mlen);

//  sig += SPX_N;

  set_tree_addr(wots_addr, tree);
  set_keypair_addr(wots_addr, idx_leaf);

  /* Sign the message hash using FORS. */
  fors_sign_streaming(stream_ctx, root, mhash, sk_seed, pub_seed, wots_addr);
//  sig += SPX_FORS_BYTES;

  for (i = 0; i < SPX_D; i++) {
    set_layer_addr(tree_addr, i);
    set_tree_addr(tree_addr, tree);

    copy_subtree_addr(wots_addr, tree_addr);
    set_keypair_addr(wots_addr, idx_leaf);

    merkle_sign_streaming(stream_ctx, root, sk_seed,  pub_seed, wots_addr, tree_addr, idx_leaf);

    /* Update the indices for the next layer. */
    idx_leaf = (tree & ((1 << SPX_TREE_HEIGHT)-1));
    tree = tree >> SPX_TREE_HEIGHT;
  }

  return 0;
}

/**
 * Verifies a detached signature and message under a given public key.
 */
int crypto_sign_verify(const uint8_t *sig, size_t siglen,
                       const uint8_t *m, size_t mlen, const uint8_t *pk)
{
    const unsigned char *pub_seed = pk;
    const unsigned char *pub_root = pk + SPX_N;
    unsigned char mhash[SPX_FORS_MSG_BYTES];
    unsigned char wots_pk[SPX_WOTS_BYTES];
    unsigned char root[SPX_N];
    unsigned char leaf[SPX_N];
    unsigned int i;
    uint64_t tree;
    uint32_t idx_leaf;
    uint32_t wots_addr[8] = {0};
    uint32_t tree_addr[8] = {0};
    uint32_t wots_pk_addr[8] = {0};

    if (siglen != SPX_BYTES) {
        return -1;
    }

    /* This hook allows the hash function instantiation to do whatever
       preparation or computation it needs, based on the public seed. */
    initialize_hash_function(pub_seed, NULL);

    set_type(wots_addr, SPX_ADDR_TYPE_WOTS);
    set_type(tree_addr, SPX_ADDR_TYPE_HASHTREE);
    set_type(wots_pk_addr, SPX_ADDR_TYPE_WOTSPK);

    /* Derive the message digest and leaf index from R || PK || M. */
    /* The additional SPX_N is a result of the hash domain separator. */
    hash_message(mhash, &tree, &idx_leaf, sig, pk, m, mlen);

    sig += SPX_N;

    /* Layer correctly defaults to 0, so no need to set_layer_addr */
    set_tree_addr(wots_addr, tree);
    set_keypair_addr(wots_addr, idx_leaf);

    fors_pk_from_sig(root, sig, mhash, pub_seed, wots_addr);
    sig += SPX_FORS_BYTES;

    /* For each subtree.. */
    for (i = 0; i < SPX_D; i++) {
        set_layer_addr(tree_addr, i);
        set_tree_addr(tree_addr, tree);

        copy_subtree_addr(wots_addr, tree_addr);
        set_keypair_addr(wots_addr, idx_leaf);

        copy_keypair_addr(wots_pk_addr, wots_addr);

        /* The WOTS public key is only correct if the signature was correct. */
        /* Initially, root is the FORS pk, but on subsequent iterations it is
           the root of the subtree below the currently processed subtree. */
        wots_pk_from_sig(wots_pk, sig, root, pub_seed, wots_addr);
        sig += SPX_WOTS_BYTES;

        /* Compute the leaf node using the WOTS public key. */
        thash(leaf, wots_pk, SPX_WOTS_LEN, pub_seed, wots_pk_addr);


      /* Compute the root node of this subtree. */
        compute_root(root, leaf, idx_leaf, 0, sig, SPX_TREE_HEIGHT,
                     pub_seed, tree_addr);
        sig += SPX_TREE_HEIGHT * SPX_N;

        /* Update the indices for the next layer. */
        idx_leaf = (tree & ((1 << SPX_TREE_HEIGHT)-1));
        tree = tree >> SPX_TREE_HEIGHT;
    }

    /* Check if the root node equals the root node in the public key. */
    if (memcmp(root, pub_root, SPX_N)) {
        return -1;
    }

    return 0;
}


int crypto_sign_verify_streaming(streaming_ctx_t *stream_ctx,
                       const uint8_t *m, size_t mlen, const uint8_t *pk)
{
  const unsigned char *pub_seed = pk;
  const unsigned char *pub_root = pk + SPX_N;
  unsigned char mhash[SPX_FORS_MSG_BYTES];
//  unsigned char wots_pk[SPX_WOTS_BYTES];
  unsigned char root[SPX_N];
  unsigned char leaf[SPX_N];
  unsigned int i;
  uint64_t tree;
  uint32_t idx_leaf;
  uint32_t wots_addr[8] = {0};
  uint32_t tree_addr[8] = {0};
  uint32_t wots_pk_addr[8] = {0};

  /* This hook allows the hash function instantiation to do whatever
     preparation or computation it needs, based on the public seed. */
  initialize_hash_function(pub_seed, NULL);

  set_type(wots_addr, SPX_ADDR_TYPE_WOTS);
  set_type(tree_addr, SPX_ADDR_TYPE_HASHTREE);
  set_type(wots_pk_addr, SPX_ADDR_TYPE_WOTSPK);

  /* Derive the message digest and leaf index from R || PK || M. */
  /* The additional SPX_N is a result of the hash domain separator. */
  /* NOTE: use root to read in first SPX_N bytes */
  read_stream(stream_ctx, root, SPX_N);
  hash_message(mhash, &tree, &idx_leaf, root, pk, m, mlen);


  /* Layer correctly defaults to 0, so no need to set_layer_addr */
  set_tree_addr(wots_addr, tree);
  set_keypair_addr(wots_addr, idx_leaf);

//  unsigned char TMP_FORS_BYTES[SPX_FORS_BYTES];
//  read_stream(stream_ctx, TMP_FORS_BYTES, SPX_FORS_BYTES);
//  fors_pk_from_sig(root, TMP_FORS_BYTES, mhash, pub_seed, wots_addr);
  fors_pk_from_sig_streaming(stream_ctx, root, mhash, pub_seed, wots_addr);

  /* For each subtree.. */
  for (i = 0; i < SPX_D; i++) {
    set_layer_addr(tree_addr, i);
    set_tree_addr(tree_addr, tree);

    copy_subtree_addr(wots_addr, tree_addr);
    set_keypair_addr(wots_addr, idx_leaf);

    copy_keypair_addr(wots_pk_addr, wots_addr);

    /* The WOTS public key is only correct if the signature was correct. */
    /* Initially, root is the FORS pk, but on subsequent iterations it is
       the root of the subtree below the currently processed subtree. */
    wots_leaf_from_pk_streaming(stream_ctx, leaf, root, pub_seed, wots_addr, wots_pk_addr);

    /* Compute the root node of this subtree. */
    compute_root_streaming(stream_ctx, root, leaf, idx_leaf, 0, SPX_TREE_HEIGHT,
                 pub_seed, tree_addr);

    /* Update the indices for the next layer. */
    idx_leaf = (tree & ((1 << SPX_TREE_HEIGHT)-1));
    tree = tree >> SPX_TREE_HEIGHT;
  }

  /* Check if the root node equals the root node in the public key. */
  if (memcmp(root, pub_root, SPX_N)) {
    return -1;
  }

  return 0;
}


/**
 * Returns an array containing the signature followed by the message.
 */
int crypto_sign(unsigned char *sm, unsigned long long *smlen,
                const unsigned char *m, unsigned long long mlen,
                const unsigned char *sk)
{
    size_t siglen;

    crypto_sign_signature(sm, &siglen, m, (size_t)mlen, sk);

    memmove(sm + SPX_BYTES, m, mlen);
    *smlen = siglen + mlen;

    return 0;
}

/**
 * Verifies a given signature-message pair under a given public key.
 */
int crypto_sign_open(unsigned char *m, unsigned long long *mlen,
                     const unsigned char *sm, unsigned long long smlen,
                     const unsigned char *pk)
{
    /* The API caller does not necessarily know what size a signature should be
       but SPHINCS+ signatures are always exactly SPX_BYTES. */
    if (smlen < SPX_BYTES) {
        memset(m, 0, smlen);
        *mlen = 0;
        return -1;
    }

    *mlen = smlen - SPX_BYTES;

    if (crypto_sign_verify(sm, SPX_BYTES, sm + SPX_BYTES, (size_t)*mlen, pk)) {
        memset(m, 0, smlen);
        *mlen = 0;
        return -1;
    }

    /* If verification was successful, move the message to the right place. */
    memmove(m, sm + SPX_BYTES, *mlen);

    return 0;
}
