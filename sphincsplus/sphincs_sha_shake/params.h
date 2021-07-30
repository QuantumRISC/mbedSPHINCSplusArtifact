#ifndef SPX_PARAMS_H
#define SPX_PARAMS_H

/* define this as the max. possible value for SPX_N */
#define SPX_N_MAX 32

typedef enum
{
  HASH_SHA,
  HASH_SHAKE
} hash_func_e;

typedef enum
{
  SIMPLE,
  ROBUST
} paramset_type_e;

extern hash_func_e PARAMSET_HASH_FUNC;
extern paramset_type_e PARAMSET_TYPE;

/* Hash output length in bytes. */
extern unsigned int SPX_N;
/* Height of the hypertree. */
extern unsigned int SPX_FULL_HEIGHT;
/* Number of subtree layer. */
extern unsigned int SPX_D;
/* FORS tree dimensions. */
extern unsigned int SPX_FORS_HEIGHT;
extern unsigned int SPX_FORS_TREES;
/* Winternitz parameter, */
extern unsigned int SPX_WOTS_W;

/* The hash function is defined by linking a different hash.c file, as opposed
   to setting a extern unsigned int constant. */

/* For clarity */
extern unsigned int SPX_ADDR_BYTES;

/* WOTS parameters. */
extern unsigned int SPX_WOTS_LOGW;

extern unsigned int SPX_WOTS_LEN1;


extern unsigned int SPX_WOTS_LEN2;

extern unsigned int SPX_WOTS_LEN;
extern unsigned int SPX_WOTS_BYTES;
extern unsigned int SPX_WOTS_PK_BYTES;

/* Subtree size. */
extern unsigned int SPX_TREE_HEIGHT;

/* FORS parameters. */
extern unsigned int SPX_FORS_MSG_BYTES;
extern unsigned int SPX_FORS_BYTES;
extern unsigned int SPX_FORS_PK_BYTES;

/* Resulting SPX sizes. */
extern unsigned int SPX_BYTES;
extern unsigned int SPX_PK_BYTES;
extern unsigned int SPX_SK_BYTES;

/* Optionally, signing can be made non-deterministic using optrand.
   This can help counter side-channel attacks that would benefit from
   getting a large number of traces when the signer uses the same nodes. */
extern unsigned int SPX_OPTRAND_BYTES;

#include "hash_offsets.h"


typedef enum
{
  sphincs_sha256_128f_robust,
  sphincs_sha256_128s_robust,
  sphincs_sha256_192f_robust,
  sphincs_sha256_192s_robust,
  sphincs_sha256_256f_robust,
  sphincs_sha256_256s_robust,

  sphincs_sha256_128f_simple,
  sphincs_sha256_128s_simple,
  sphincs_sha256_192f_simple,
  sphincs_sha256_192s_simple,
  sphincs_sha256_256f_simple,
  sphincs_sha256_256s_simple,


  sphincs_shake256_128f_robust,
  sphincs_shake256_128s_robust,
  sphincs_shake256_192f_robust,
  sphincs_shake256_192s_robust,
  sphincs_shake256_256f_robust,
  sphincs_shake256_256s_robust,

  sphincs_shake256_128f_simple,
  sphincs_shake256_128s_simple,
  sphincs_shake256_192f_simple,
  sphincs_shake256_192s_simple,
  sphincs_shake256_256f_simple,
  sphincs_shake256_256s_simple
} spx_param_set_e;


char* get_param_name(spx_param_set_e paramset);
void init_params(spx_param_set_e paramset);

#endif