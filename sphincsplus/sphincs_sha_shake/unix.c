#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "api.h"
#include "params.h"
#include "rng.h"
#include "thash.h"
#include "sha256.h" // types

#define SPX_MLEN 32

char STREAM_SIG_OUT_FILE[255] = "sig_stream.sig";

unsigned char m[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                     0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                     0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                     0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08	};


extern spx_param_set_e PARAMETER_SET_LIST[];
extern unsigned int PARAMETER_SET_LIST_SIZE;

void get_param_name_w_prefix_and_suffix(char* outbuf, paramset_type_e paramset, char* prefix, char* suffix)
{
  memset(outbuf, 0, 255);
  strcat(outbuf, prefix);
  strcat(outbuf, get_param_name(paramset));
  strcat(outbuf, suffix);
}


int test_thash_sha256()
{
  uint32_t addr[8];
  unsigned char pub_seed[SPX_N];
  unsigned char result_orig[SPX_N];
  unsigned char result_streaming[SPX_N];

  randombytes((unsigned char*)addr, 32);
  randombytes(pub_seed, SPX_N);

  sha256_inc_state_t sha256_state;
  mgf1_sha256_state_t mgf1_sha256_state;
  thash_init_sha256(&sha256_state, &mgf1_sha256_state, pub_seed, addr);

  unsigned char input[2 * SPX_N];
  randombytes(input, sizeof(input));

  for(unsigned i = 0; i < sizeof(input)/SPX_N; i++)
  {
    thash_update_sha256(&sha256_state, &mgf1_sha256_state, input + i*SPX_N, i, pub_seed, addr);
  }

  thash_finalize_sha256(result_streaming, &sha256_state);
  thash_sha256(result_orig, input, sizeof(input)/SPX_N, pub_seed, addr);
  return memcmp(result_orig, result_streaming, SPX_N);
}

int test_thash_sha256_simple()
{
  uint32_t addr[8];
  unsigned char pub_seed[SPX_N];
  unsigned char result_orig[SPX_N];
  unsigned char result_streaming[SPX_N];

  randombytes((unsigned char*)addr, 32);
  randombytes(pub_seed, SPX_N);

  sha256_inc_state_t sha256_state;
  thash_init_sha256_simple(&sha256_state, pub_seed, addr);

  unsigned char input[2 * SPX_N];
  randombytes(input, sizeof(input));

  for(unsigned i = 0; i < sizeof(input)/SPX_N; i++)
  {
    thash_update_sha256_simple(&sha256_state, input + i*SPX_N, i, pub_seed, addr);
  }

  thash_finalize_sha256_simple(result_streaming, &sha256_state);
  thash_sha256_simple(result_orig, input, sizeof(input)/SPX_N, pub_seed, addr);
  return memcmp(result_orig, result_streaming, SPX_N);
}

int test_thash_shake256()
{
  uint32_t addr[8];
  unsigned char pub_seed[SPX_N];
  unsigned char result_orig[SPX_N];
  unsigned char result_streaming[SPX_N];

  randombytes((unsigned char*)addr, 32);
  randombytes(pub_seed, SPX_N);

  shake256_inc_state_t shake256_state;
  thash_init_shake256(&shake256_state, pub_seed, addr);

  unsigned char input[2 * SPX_N];
  randombytes(input, sizeof(input));

  for(unsigned i = 0; i < sizeof(input)/SPX_N; i++)
  {
    thash_update_shake256(&shake256_state, input + i*SPX_N, i, pub_seed, addr);
  }

  thash_finalize_shake256(result_streaming, &shake256_state);
  thash_shake256(result_orig, input, sizeof(input)/SPX_N, pub_seed, addr);
  return memcmp(result_orig, result_streaming, SPX_N);
}

int test_thash_shake256_simple()
{
  uint32_t addr[8];
  unsigned char pub_seed[SPX_N];
  unsigned char result_orig[SPX_N];
  unsigned char result_streaming[SPX_N];

  randombytes((unsigned char*)addr, 32);
  randombytes(pub_seed, SPX_N);

  shake256_inc_state_t shake256_state;
  thash_init_shake256_simple(&shake256_state, pub_seed, addr);

  unsigned char input[2 * SPX_N];
  randombytes(input, sizeof(input));

  for(unsigned i = 0; i < sizeof(input)/SPX_N; i++)
  {
    thash_update_shake256_simple(&shake256_state, input + i*SPX_N, i, pub_seed, addr);
  }

  thash_finalize_shake256_simple(result_streaming, &shake256_state);
  thash_shake256_simple(result_orig, input, sizeof(input)/SPX_N, pub_seed, addr);
  return memcmp(result_orig, result_streaming, SPX_N);
}

int test_hash_update()
{
  unsigned char data[397];
  randombytes(data, sizeof(data));

  unsigned char result_orig[SPX_SHA256_OUTPUT_BYTES];
  unsigned char result_streaming[SPX_N];

  sha256_inc_state_t state;
  sha256_inc_state_init(&state);
  sha256_inc_state_update(&state, data, sizeof(data)-67);
  sha256_inc_state_update(&state, data + sizeof(data)-67, 67);
  sha256_inc_state_finalize(&state, result_streaming);

  sha256(result_orig, data, sizeof(data));

  return memcmp(result_streaming, result_orig, SPX_N);
}


int sign_original(unsigned char *sig)
{
  int ret = 0;

  /* Make stdout buffer more responsive. */
  setbuf(stdout, NULL);

  unsigned char pk[SPX_PK_BYTES];
  unsigned char sk[SPX_SK_BYTES];
  size_t siglen;

//  ret = crypto_sign_seed_keypair_streaming(pk, sk, seed);
  ret = crypto_sign_keypair(pk, sk);
  if(ret)
  {
    goto fail;
  }

  ret = crypto_sign_signature(sig, &siglen, m, SPX_MLEN, sk);
  if(ret)
  {
    goto fail;
  }

  ret = crypto_sign_verify(sig, siglen, m, SPX_MLEN, pk);
  if(ret)
  {
    goto fail;
  }

  return 0;
  fail:
  printf("ERROR\n");
  return -1;
}


void print_hex(char* label, unsigned char* data, int len)
{
  printf("%s: ", label);
  for(int i = 0; i < len; i++)
  {
    printf("%02x", data[i]);
  }
  printf("\n");
}

int sign_streaming(paramset_type_e paramset)
{
  int ret = 0;
  streaming_ctx_t ctx;
  unsigned char pk[SPX_PK_BYTES];
  unsigned char sk[SPX_SK_BYTES];
  char out_filename[255];

  get_param_name_w_prefix_and_suffix((char*)out_filename, paramset, "out_files/", ".sig");
  strcpy(STREAM_SIG_OUT_FILE, out_filename);
  init_streaming_ctx(&ctx, WRITE_SIG_FLAG, STREAM_SERIAL, NULL);

  srand2(100); // reset RNG

  ret = crypto_sign_keypair_streaming(pk, sk);
  get_param_name_w_prefix_and_suffix((char*)out_filename, paramset, "out_files/", ".msg");
  FILE *file = fopen("out_filename", "wb");
  fwrite(m, sizeof(unsigned char), SPX_MLEN, file);
  fclose(file);


  get_param_name_w_prefix_and_suffix((char*)out_filename, paramset, "out_files/", ".pk");
  file = fopen(out_filename, "wb");
  fwrite(pk, sizeof(unsigned char), SPX_PK_BYTES, file);
  fclose(file);

  get_param_name_w_prefix_and_suffix((char*)out_filename, paramset, "out_files/", ".sk");
  file = fopen(out_filename, "wb");
  fwrite(sk, sizeof(unsigned char), SPX_SK_BYTES, file);
  fclose(file);

  if(ret)
  {
    goto fail;
  }

  ret = crypto_sign_signature_streaming(&ctx, m, SPX_MLEN, sk);
  if(ret)
  {
    goto fail;
  }


  destroy_streaming_ctx(&ctx);
  init_streaming_ctx(&ctx, READ_SIG_FLAG, STREAM_SERIAL, NULL);

  ret = crypto_sign_verify_streaming(&ctx, m, SPX_MLEN, pk);
  if(ret)
  {
    printf("Streaming verification FAILED!\n");
    return -1;
  }

  destroy_streaming_ctx(&ctx);

  return 0;

  fail:
  printf("ERROR\n");
  return -1;
}


int main()
{
  int ret;

#ifdef GPROF

  /*
   * Intention: Verify that modified streaming code does not introduce some large unintended overhead
   */
  init_params(sphincs_sha256_256s_robust);
  unsigned char *sig = malloc(SPX_BYTES);


  for(int i = 0; i < 10; i++)
  {
    ret = sign_streaming(sig);

    if(ret != 0)
    {
      printf("ERROR!\n");
      if(sig)
      {
        free(sig);
      }
      return -1;
    }
  }

  free(sig);
  printf("Success.\n");

  // do not execute the rest of the code when profiling
  return 0;
#endif

  unsigned char *sig_original = NULL;
  unsigned char *sig_streaming = NULL;

  for(size_t paramset = 0; paramset < PARAMETER_SET_LIST_SIZE; paramset++)
  {
    init_params(PARAMETER_SET_LIST[paramset]);

//    gen_msg_random_test();


    sig_original = malloc(SPX_BYTES);
    sig_streaming = malloc(SPX_BYTES);

    srand2(100); // reset RNG

    printf("parameter set: %s\n=====\n\n", get_param_name(PARAMETER_SET_LIST[paramset]));

    if(PARAMSET_HASH_FUNC == HASH_SHA)
    {
      printf("Testing hash update function\n");
      ret = test_hash_update();
      if(ret)
      {
        printf("FAILED!\n");
        goto fail;
      }
      printf("Success.\n======\n\n");

      if(PARAMSET_TYPE == ROBUST)
      {
        printf("Testing THASH function (sha256 robust)\n");
        ret = test_thash_sha256();
        if(ret)
        {
          printf("FAILED!\n");
          goto fail;
        }
        printf("Success.\n======\n\n");
      }
      else
      {
        printf("Testing THASH function (sha256 simple)\n");
        ret = test_thash_sha256_simple();
        if(ret)
        {
          printf("FAILED!\n");
          goto fail;
        }
        printf("Success.\n======\n\n");
      }
    }
    else
    {
      if(PARAMSET_TYPE == ROBUST)
      {
        printf("Testing THASH function (shake256)\n");
        ret = test_thash_shake256();
        if(ret)
        {
          printf("FAILED!\n");
          goto fail;
        }
        printf("Success.\n======\n\n");
      }
      else
      {
        printf("Testing THASH function (shake256 simple)\n");
        ret = test_thash_shake256_simple();
        if(ret)
        {
          printf("FAILED!\n");
          goto fail;
        }
        printf("Success.\n======\n\n");
      }
    }

    srand2(100); // reset RNG

    printf("Signing with original code ... ");
    ret = sign_original(sig_original);
    if(ret)
    {
      goto fail;
    }
    printf("Success.\n");


    srand2(100); // reset RNG

    printf("Signing with streaming code ... ");
    ret = sign_streaming(PARAMETER_SET_LIST[paramset]);
    if(ret)
    {
      goto fail;
    }
    printf("Success.\n");

    printf("Comparing both signatures ... ");
    FILE *fp;
    fp = fopen(STREAM_SIG_OUT_FILE, "rb");
    size_t read_bytes = fread(sig_streaming, sizeof(unsigned char), SPX_BYTES , fp);
    if(read_bytes != SPX_BYTES)
    {
      printf("sig size not as expected!\n");
      goto fail;
    }

    ret = memcmp(sig_streaming, sig_original, SPX_BYTES);
    if(ret == 0)
    {
      printf("Success! Streaming and original sig are identical!\n");
    }
    else
    {
      printf("FAILED!\n");
      goto fail;
    }

    printf("Verifying that crypto_sign_keypair_streaming() and crypto_sign_keypair() produce the same output ... ");

    unsigned char pk1[SPX_PK_BYTES], pk2[SPX_SK_BYTES];
    unsigned char sk1[SPX_SK_BYTES], sk2[SPX_SK_BYTES];
    int ret1, ret2;

    srand2(100); // reset RNG
    ret1 = crypto_sign_keypair(pk1, sk1);

    srand2(100); // reset RNG
    ret2 = crypto_sign_keypair_streaming(pk2, sk2);

    if(ret1 || ret2)
    {
      printf("Error, sign failed\n");
      ret = -1;
      goto fail;
    }

    if((memcmp(sk1, sk2, SPX_SK_BYTES) == 0) && (memcmp(pk1, pk2, SPX_PK_BYTES) == 0))
    {
      printf("Success\n");
    }
    else {
      printf("FAILED!\n");
      ret = -1;
      goto fail;
    }

    if(sig_original) { free(sig_original); sig_original = NULL; }
    if(sig_streaming) { free(sig_streaming); sig_streaming = NULL; }
  }

  printf("\nRun successful\n");



  fail:
  if(sig_original) { free(sig_original); }
  if(sig_streaming) { free(sig_streaming); }

  return ret;
}
