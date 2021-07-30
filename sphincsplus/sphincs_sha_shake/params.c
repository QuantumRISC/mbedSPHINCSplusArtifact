#include "params.h"
#ifdef TARGET_UNIX
# include <stdio.h>
# include <stdlib.h>
#endif

/*
 * Replaces Reference Code #defines with "global variables"
 */

/* params.h */
unsigned int SPX_N;
unsigned int SPX_FULL_HEIGHT;
unsigned int SPX_D;
unsigned int SPX_FORS_HEIGHT;
unsigned int SPX_FORS_TREES;
unsigned int SPX_WOTS_W;

unsigned int SPX_ADDR_BYTES;

unsigned int SPX_WOTS_LOGW;
unsigned int SPX_WOTS_LEN1;
unsigned int SPX_WOTS_LEN2;
unsigned int SPX_WOTS_LEN;
unsigned int SPX_WOTS_BYTES;
unsigned int SPX_WOTS_PK_BYTES;

unsigned int SPX_TREE_HEIGHT;

unsigned int SPX_FORS_MSG_BYTES;
unsigned int SPX_FORS_BYTES;
unsigned int SPX_FORS_PK_BYTES;

unsigned int SPX_BYTES;
unsigned int SPX_PK_BYTES;
unsigned int SPX_SK_BYTES;

unsigned int SPX_OPTRAND_BYTES;

/* api.h */
unsigned int CRYPTO_SECRETKEYBYTES;
unsigned int CRYPTO_PUBLICKEYBYTES;
unsigned int CRYPTO_BYTES;
unsigned int CRYPTO_SEEDBYTES;

/* hash offsets */
unsigned int SPX_OFFSET_LAYER;
unsigned int SPX_OFFSET_TREE;
unsigned int SPX_OFFSET_TYPE;
unsigned int SPX_OFFSET_KP_ADDR2;
unsigned int SPX_OFFSET_KP_ADDR1;
unsigned int SPX_OFFSET_CHAIN_ADDR;
unsigned int SPX_OFFSET_HASH_ADDR;
unsigned int SPX_OFFSET_TREE_HGT;
unsigned int SPX_OFFSET_TREE_INDEX;

hash_func_e PARAMSET_HASH_FUNC;
paramset_type_e PARAMSET_TYPE;


void set_sha_offsets()
{
  SPX_OFFSET_LAYER = 0;
  SPX_OFFSET_TREE = 1;
  SPX_OFFSET_TYPE = 9;
  SPX_OFFSET_KP_ADDR2 = 12;
  SPX_OFFSET_KP_ADDR1 = 13;
  SPX_OFFSET_CHAIN_ADDR = 17;
  SPX_OFFSET_HASH_ADDR = 21;
  SPX_OFFSET_TREE_HGT = 17;
  SPX_OFFSET_TREE_INDEX = 18;

  PARAMSET_HASH_FUNC = HASH_SHA;
}

void set_shake_offsets()
{
  SPX_OFFSET_LAYER = 3;
  SPX_OFFSET_TREE = 8;
  SPX_OFFSET_TYPE = 19;
  SPX_OFFSET_KP_ADDR2 = 22;
  SPX_OFFSET_KP_ADDR1 = 23;
  SPX_OFFSET_CHAIN_ADDR = 27;
  SPX_OFFSET_HASH_ADDR = 31;
  SPX_OFFSET_TREE_HGT = 27;
  SPX_OFFSET_TREE_INDEX = 28;

  PARAMSET_HASH_FUNC = HASH_SHAKE;
}

spx_param_set_e PARAMETER_SET_LIST[] = {
#ifdef VARIANT_SHA2_ROBUST
  sphincs_sha256_128f_robust,
                                          sphincs_sha256_128s_robust,
                                          sphincs_sha256_192f_robust,
                                          sphincs_sha256_192s_robust,
                                          sphincs_sha256_256f_robust,
                                          sphincs_sha256_256s_robust,
#endif

#ifdef VARIANT_SHA2_SIMPLE
  sphincs_sha256_128f_simple,
                                          sphincs_sha256_128s_simple,
                                          sphincs_sha256_192f_simple,
                                          sphincs_sha256_192s_simple,
                                          sphincs_sha256_256f_simple,
                                          sphincs_sha256_256s_simple,
#endif

#ifdef VARIANT_SHAKE_ROBUST
  sphincs_shake256_128f_robust,
                                          sphincs_shake256_128s_robust,
                                          sphincs_shake256_192f_robust,
                                          sphincs_shake256_192s_robust,
                                          sphincs_shake256_256f_robust,
                                          sphincs_shake256_256s_robust,
#endif

#ifdef VARIANT_SHAKE_SIMPLE
  sphincs_shake256_128f_simple,
                                          sphincs_shake256_128s_simple,
                                          sphincs_shake256_192f_simple,
                                          sphincs_shake256_192s_simple,
                                          sphincs_shake256_256f_simple,
                                          sphincs_shake256_256s_simple
#endif
};
unsigned int PARAMETER_SET_LIST_SIZE = sizeof(PARAMETER_SET_LIST)/sizeof(PARAMETER_SET_LIST[0]);


char* sphincs_sha256_128f_robust_str = "sphincs-sha256-128f-robust";
char* sphincs_sha256_128s_robust_str = "sphincs-sha256-128s-robust";
char* sphincs_sha256_192f_robust_str = "sphincs-sha256-192f-robust";
char* sphincs_sha256_192s_robust_str = "sphincs-sha256-192s-robust";
char* sphincs_sha256_256f_robust_str = "sphincs-sha256-256f-robust";
char* sphincs_sha256_256s_robust_str = "sphincs-sha256-256s-robust";
char* sphincs_sha256_128f_simple_str = "sphincs-sha256-128f-simple";
char* sphincs_sha256_128s_simple_str = "sphincs-sha256-128s-simple";
char* sphincs_sha256_192f_simple_str = "sphincs-sha256-192f-simple";
char* sphincs_sha256_192s_simple_str = "sphincs-sha256-192s-simple";
char* sphincs_sha256_256f_simple_str = "sphincs-sha256-256f-simple";
char* sphincs_sha256_256s_simple_str = "sphincs-sha256-256s-simple";
char* sphincs_shake256_128f_robust_str = "sphincs-shake256-128f-robust";
char* sphincs_shake256_128s_robust_str = "sphincs-shake256-128s-robust";
char* sphincs_shake256_192f_robust_str = "sphincs-shake256-192f-robust";
char* sphincs_shake256_192s_robust_str = "sphincs-shake256-192s-robust";
char* sphincs_shake256_256f_robust_str = "sphincs-shake256-256f-robust";
char* sphincs_shake256_256s_robust_str = "sphincs-shake256-256s-robust";
char* sphincs_shake256_128f_simple_str = "sphincs-shake256-128f-simple";
char* sphincs_shake256_128s_simple_str = "sphincs-shake256-128s-simple";
char* sphincs_shake256_192f_simple_str = "sphincs-shake256-192f-simple";
char* sphincs_shake256_192s_simple_str = "sphincs-shake256-192s-simple";
char* sphincs_shake256_256f_simple_str = "sphincs-shake256-256f-simple";
char* sphincs_shake256_256s_simple_str = "sphincs-shake256-256s-simple";

char* get_param_name(spx_param_set_e paramset)
{
  switch(paramset)
  {
    case sphincs_sha256_128f_robust:
      return sphincs_sha256_128f_robust_str;

    case sphincs_sha256_128s_robust:
      return sphincs_sha256_128s_robust_str;

    case sphincs_sha256_192f_robust:
      return sphincs_sha256_192f_robust_str;

    case sphincs_sha256_192s_robust:
      return sphincs_sha256_192s_robust_str;

    case sphincs_sha256_256f_robust:
      return sphincs_sha256_256f_robust_str;

    case sphincs_sha256_256s_robust:
      return sphincs_sha256_256s_robust_str;

    case sphincs_sha256_128f_simple:
      return sphincs_sha256_128f_simple_str;

    case sphincs_sha256_128s_simple:
      return sphincs_sha256_128s_simple_str;

    case sphincs_sha256_192f_simple:
      return sphincs_sha256_192f_simple_str;

    case sphincs_sha256_192s_simple:
      return sphincs_sha256_192s_simple_str;

    case sphincs_sha256_256f_simple:
      return sphincs_sha256_256f_simple_str;

    case sphincs_sha256_256s_simple:
      return sphincs_sha256_256s_simple_str;

    case sphincs_shake256_128f_robust:
      return sphincs_shake256_128f_robust_str;

    case sphincs_shake256_128s_robust:
      return sphincs_shake256_128s_robust_str;

    case sphincs_shake256_192f_robust:
      return sphincs_shake256_192f_robust_str;

    case sphincs_shake256_192s_robust:
      return sphincs_shake256_192s_robust_str;

    case sphincs_shake256_256f_robust:
      return sphincs_shake256_256f_robust_str;

    case sphincs_shake256_256s_robust:
      return sphincs_shake256_256s_robust_str;

    case sphincs_shake256_128f_simple:
      return sphincs_shake256_128f_simple_str;

    case sphincs_shake256_128s_simple:
      return sphincs_shake256_128s_simple_str;

    case sphincs_shake256_192f_simple:
      return sphincs_shake256_192f_simple_str;

    case sphincs_shake256_192s_simple:
      return sphincs_shake256_192s_simple_str;

    case sphincs_shake256_256f_simple:
      return sphincs_shake256_256f_simple_str;

    case sphincs_shake256_256s_simple:
      return sphincs_shake256_256s_simple_str;

    default:
#ifdef TARGET_UNIX
      printf("get_param_name(): Parameter Set not found");
#endif
      return "";
  }
}

void init_params(spx_param_set_e paramset)
{
  switch(paramset)
  {
    /* sha256 */
    case sphincs_sha256_128f_robust:
      SPX_N = 16;
      SPX_FULL_HEIGHT = 66;
      SPX_D = 22;
      SPX_FORS_HEIGHT = 6;
      SPX_FORS_TREES = 33;
      SPX_WOTS_W = 16;
      set_sha_offsets();
      PARAMSET_TYPE = ROBUST;
      break;
    case sphincs_sha256_128f_simple:
      SPX_N = 16;
      SPX_FULL_HEIGHT = 66;
      SPX_D = 22;
      SPX_FORS_HEIGHT = 6;
      SPX_FORS_TREES = 33;
      SPX_WOTS_W = 16;
      set_sha_offsets();
      PARAMSET_TYPE = SIMPLE;
      break;

    case sphincs_sha256_128s_robust:
      SPX_N = 16;
      SPX_FULL_HEIGHT = 63;
      SPX_D = 7;
      SPX_FORS_HEIGHT = 12;
      SPX_FORS_TREES = 14;
      SPX_WOTS_W = 16;
      set_sha_offsets();
      PARAMSET_TYPE = ROBUST;
      break;
    case sphincs_sha256_128s_simple:
      SPX_N = 16;
      SPX_FULL_HEIGHT = 63;
      SPX_D = 7;
      SPX_FORS_HEIGHT = 12;
      SPX_FORS_TREES = 14;
      SPX_WOTS_W = 16;
      set_sha_offsets();
      PARAMSET_TYPE = SIMPLE;
      break;

    case sphincs_sha256_192f_robust:
      SPX_N = 24;
      SPX_FULL_HEIGHT = 66;
      SPX_D = 22;
      SPX_FORS_HEIGHT = 8;
      SPX_FORS_TREES = 33;
      SPX_WOTS_W = 16;
      set_sha_offsets();
      PARAMSET_TYPE = ROBUST;
      break;
    case sphincs_sha256_192f_simple:
      SPX_N = 24;
      SPX_FULL_HEIGHT = 66;
      SPX_D = 22;
      SPX_FORS_HEIGHT = 8;
      SPX_FORS_TREES = 33;
      SPX_WOTS_W = 16;
      set_sha_offsets();
      PARAMSET_TYPE = SIMPLE;
      break;

    case sphincs_sha256_192s_robust:
      SPX_N = 24;
      SPX_FULL_HEIGHT = 63;
      SPX_D = 7;
      SPX_FORS_HEIGHT = 14;
      SPX_FORS_TREES = 17;
      SPX_WOTS_W = 16;
      set_sha_offsets();
      PARAMSET_TYPE = ROBUST;
      break;
    case sphincs_sha256_192s_simple:
      SPX_N = 24;
      SPX_FULL_HEIGHT = 63;
      SPX_D = 7;
      SPX_FORS_HEIGHT = 14;
      SPX_FORS_TREES = 17;
      SPX_WOTS_W = 16;
      set_sha_offsets();
      PARAMSET_TYPE = SIMPLE;
      break;

    case sphincs_sha256_256f_robust:
      SPX_N = 32;
      SPX_FULL_HEIGHT = 68;
      SPX_D = 17;
      SPX_FORS_HEIGHT = 9;
      SPX_FORS_TREES = 35;
      SPX_WOTS_W = 16;
      set_sha_offsets();
      PARAMSET_TYPE = ROBUST;
      break;
    case sphincs_sha256_256f_simple:
      SPX_N = 32;
      SPX_FULL_HEIGHT = 68;
      SPX_D = 17;
      SPX_FORS_HEIGHT = 9;
      SPX_FORS_TREES = 35;
      SPX_WOTS_W = 16;
      set_sha_offsets();
      PARAMSET_TYPE = SIMPLE;
      break;

    case sphincs_sha256_256s_robust:
      SPX_N = 32;
      SPX_FULL_HEIGHT = 64;
      SPX_D = 8;
      SPX_FORS_HEIGHT = 14;
      SPX_FORS_TREES = 22;
      SPX_WOTS_W = 16;
      set_sha_offsets();
      PARAMSET_TYPE = ROBUST;
      break;
    case sphincs_sha256_256s_simple:
      SPX_N = 32;
      SPX_FULL_HEIGHT = 64;
      SPX_D = 8;
      SPX_FORS_HEIGHT = 14;
      SPX_FORS_TREES = 22;
      SPX_WOTS_W = 16;
      set_sha_offsets();
      PARAMSET_TYPE = SIMPLE;
      break;

      /* shake */

    case sphincs_shake256_128f_robust:
      SPX_N = 16;
      SPX_FULL_HEIGHT = 66;
      SPX_D = 22;
      SPX_FORS_HEIGHT = 6;
      SPX_FORS_TREES = 33;
      SPX_WOTS_W = 16;
      set_shake_offsets();
      PARAMSET_TYPE = ROBUST;
      break;
    case sphincs_shake256_128f_simple:
      SPX_N = 16;
      SPX_FULL_HEIGHT = 66;
      SPX_D = 22;
      SPX_FORS_HEIGHT = 6;
      SPX_FORS_TREES = 33;
      SPX_WOTS_W = 16;
      set_shake_offsets();
      PARAMSET_TYPE = SIMPLE;
      break;

    case sphincs_shake256_128s_robust:
      SPX_N = 16;
      SPX_FULL_HEIGHT = 63;
      SPX_D = 7;
      SPX_FORS_HEIGHT = 12;
      SPX_FORS_TREES = 14;
      SPX_WOTS_W = 16;
      set_shake_offsets();
      PARAMSET_TYPE = ROBUST;
      break;
    case sphincs_shake256_128s_simple:
      SPX_N = 16;
      SPX_FULL_HEIGHT = 63;
      SPX_D = 7;
      SPX_FORS_HEIGHT = 12;
      SPX_FORS_TREES = 14;
      SPX_WOTS_W = 16;
      set_shake_offsets();
      PARAMSET_TYPE = SIMPLE;
      break;


    case sphincs_shake256_192f_robust:
      SPX_N = 24;
      SPX_FULL_HEIGHT = 66;
      SPX_D = 22;
      SPX_FORS_HEIGHT = 8;
      SPX_FORS_TREES = 33;
      SPX_WOTS_W = 16;
      set_shake_offsets();
      PARAMSET_TYPE = ROBUST;
      break;
    case sphincs_shake256_192f_simple:
      SPX_N = 24;
      SPX_FULL_HEIGHT = 66;
      SPX_D = 22;
      SPX_FORS_HEIGHT = 8;
      SPX_FORS_TREES = 33;
      SPX_WOTS_W = 16;
      set_shake_offsets();
      PARAMSET_TYPE = SIMPLE;
      break;

    case sphincs_shake256_192s_robust:
      SPX_N = 24;
      SPX_FULL_HEIGHT = 63;
      SPX_D = 7;
      SPX_FORS_HEIGHT = 14;
      SPX_FORS_TREES = 17;
      SPX_WOTS_W = 16;
      set_shake_offsets();
      PARAMSET_TYPE = ROBUST;
      break;
    case sphincs_shake256_192s_simple:
      SPX_N = 24;
      SPX_FULL_HEIGHT = 63;
      SPX_D = 7;
      SPX_FORS_HEIGHT = 14;
      SPX_FORS_TREES = 17;
      SPX_WOTS_W = 16;
      set_shake_offsets();
      PARAMSET_TYPE = SIMPLE;
      break;

    case sphincs_shake256_256f_robust:
      SPX_N = 32;
      SPX_FULL_HEIGHT = 68;
      SPX_D = 17;
      SPX_FORS_HEIGHT = 9;
      SPX_FORS_TREES = 35;
      SPX_WOTS_W = 16;
      set_shake_offsets();
      PARAMSET_TYPE = ROBUST;
      break;
    case sphincs_shake256_256f_simple:
      SPX_N = 32;
      SPX_FULL_HEIGHT = 68;
      SPX_D = 17;
      SPX_FORS_HEIGHT = 9;
      SPX_FORS_TREES = 35;
      SPX_WOTS_W = 16;
      set_shake_offsets();
      PARAMSET_TYPE = SIMPLE;
      break;

    case sphincs_shake256_256s_robust:
      SPX_N = 32;
      SPX_FULL_HEIGHT = 64;
      SPX_D = 8;
      SPX_FORS_HEIGHT = 14;
      SPX_FORS_TREES = 22;
      SPX_WOTS_W = 16;
      set_shake_offsets();
      PARAMSET_TYPE = ROBUST;
      break;
    case sphincs_shake256_256s_simple:
      SPX_N = 32;
      SPX_FULL_HEIGHT = 64;
      SPX_D = 8;
      SPX_FORS_HEIGHT = 14;
      SPX_FORS_TREES = 22;
      SPX_WOTS_W = 16;
      set_shake_offsets();
      PARAMSET_TYPE = SIMPLE;
      break;
  }
  SPX_ADDR_BYTES = 32;

  if(SPX_WOTS_W == 256)
  {
    SPX_WOTS_LOGW = 8;
  }
  else if(SPX_WOTS_W == 16)
  {
    SPX_WOTS_LOGW = 4;
  }
  else
  {
#ifdef TARGET_UNIX
    printf("error SPX_WOTS_W assumed 16 or 256\n");
    exit(-1);
#endif
  }

  SPX_WOTS_LEN1 = (8 * SPX_N / SPX_WOTS_LOGW);

  if(SPX_WOTS_W == 256)
  {
    if(SPX_N <= 1)
    {
      SPX_WOTS_LEN2 = 1;
    }
    if(SPX_N <= 256)
    {
      SPX_WOTS_LEN2 = 2;
    }
    else
    {
#ifdef TARGET_UNIX
      printf("error Did not precompute SPX_WOTS_LEN2 for n outside {2, .., 256}\n");
      exit(-1);
#endif
    }
  }
  else if(SPX_WOTS_W == 16)
  {
    if(SPX_N <= 8)
    {
      SPX_WOTS_LEN2 = 2;
    }
    else if(SPX_N <= 136)
    {
      SPX_WOTS_LEN2 = 3;
    }
    else if(SPX_N <= 256)
    {
      SPX_WOTS_LEN2 = 4;
    }
    else
    {
#ifdef TARGET_UNIX
      printf("error Did not precompute SPX_WOTS_LEN2 for n outside {2, .., 256}\n");
      exit(-1);
#endif
    }
  }

  SPX_WOTS_LEN = (SPX_WOTS_LEN1 + SPX_WOTS_LEN2);
  SPX_WOTS_BYTES = (SPX_WOTS_LEN * SPX_N);
  SPX_WOTS_PK_BYTES = SPX_WOTS_BYTES;

  SPX_TREE_HEIGHT = (SPX_FULL_HEIGHT / SPX_D);

  if(SPX_TREE_HEIGHT * SPX_D != SPX_FULL_HEIGHT)
  {
#ifdef TARGET_UNIX
    printf("error SPX_D should always divide SPX_FULL_HEIGHT\n");
    exit(-1);
#endif
  }

  SPX_FORS_MSG_BYTES = ((SPX_FORS_HEIGHT * SPX_FORS_TREES + 7) / 8);
  SPX_FORS_BYTES = ((SPX_FORS_HEIGHT + 1) * SPX_FORS_TREES * SPX_N);
  SPX_FORS_PK_BYTES = SPX_N;


  SPX_BYTES = (SPX_N + SPX_FORS_BYTES + SPX_D * SPX_WOTS_BYTES + SPX_FULL_HEIGHT * SPX_N);
  SPX_PK_BYTES = (2 * SPX_N);
  SPX_SK_BYTES = (2 * SPX_N + SPX_PK_BYTES);

  SPX_OPTRAND_BYTES = 32;


  /* api.h */
  CRYPTO_SECRETKEYBYTES = SPX_SK_BYTES;
  CRYPTO_PUBLICKEYBYTES = SPX_PK_BYTES;
  CRYPTO_BYTES = SPX_BYTES;
  CRYPTO_SEEDBYTES = 3*SPX_N;
}
