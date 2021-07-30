#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../api.h"
#include "../params.h"
#include "../randombytes.h"
#include "../sha256.h"
#include "../hash.h"

#define SPX_MLEN 32

unsigned char m[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                     0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                     0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                     0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08	};

void read_from_file(char* filename, unsigned char *out, unsigned len)
{
  FILE *f = fopen(filename, "rb");
  if(!f)
  {
    fprintf(stderr, "Failed to open (file: %s)\n", filename);
    exit(1);
  }
  fread(out, 1, len, f);
  fclose(f);

}

void write_to_file(char* filename, unsigned char *in, unsigned len)
{
  FILE *f = fopen(filename, "wb");
  fwrite(in, 1, len, f);
  fclose(f);
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

int gen_msg_random_test()
{

  unsigned char R[32] = {0};
  unsigned char sk_prf[32] = {0};
  unsigned char optrand[32] = {0};

  gen_message_random(R, sk_prf, optrand, m, SPX_MLEN);

  print_hex("gen_msg_random(0,0,0)", R, SPX_N);

  return 0;
}

int main()
{
  int ret = 0;
  int i;
  size_t siglen;

  /* Make stdout buffer more responsive. */
  setbuf(stdout, NULL);

  unsigned char sk_ref[SPX_SK_BYTES];
  unsigned char pk_ref[SPX_PK_BYTES];
  unsigned char sig_ref[SPX_BYTES];
  
  unsigned char sk_ext[SPX_SK_BYTES];
  unsigned char pk_ext[SPX_PK_BYTES];
  unsigned char sig_ext[SPX_BYTES];

  srand2(100);

  crypto_sign_keypair(pk_ref, sk_ref);
  crypto_sign_signature(sig_ref, &siglen, m, SPX_MLEN, sk_ref);
  ret = crypto_sign_verify(sig_ref, SPX_BYTES, m, SPX_MLEN, pk_ref);
  if(ret)
  {
    printf("INTERNAL SIGNATURE VERIFICATION FAILED!\n");
    goto fail;
  }

  printf("check that external sig can be verified ...\n");

  read_from_file(SIG_DIR PARAMSETNAME ".pk", pk_ext, SPX_PK_BYTES);
  read_from_file(SIG_DIR PARAMSETNAME ".sk", sk_ext, SPX_SK_BYTES);
  read_from_file(SIG_DIR PARAMSETNAME ".sig", sig_ext, SPX_BYTES);

  ret = crypto_sign_verify(sig_ext, SPX_BYTES, m, SPX_MLEN, pk_ext);
  if(ret)
  {
    printf("EXTERNAL SIGNATURE VERIFICATION FAILED!\n");
    goto fail;
  }
  
  printf("\tverify signature: %d\n", ret);
  
  printf("check that ref impl generates same keys and sig ...\n");
  ret = memcmp(pk_ext, pk_ref, SPX_PK_BYTES);
  if(ret)
  {
    printf("\tPK does not match!\n");
    print_hex("\tpk_ref", pk_ref, SPX_PK_BYTES);
    print_hex("\tpk_ext", pk_ext, SPX_PK_BYTES);
    goto fail;
  }
  printf("\tPK matches!\n");
  ret = memcmp(sk_ext, sk_ref, SPX_SK_BYTES);
  if(ret)
  {
    printf("\tSK does not match!\n");
    print_hex("\tsk_ref", sk_ref, SPX_SK_BYTES);
    print_hex("\tsk_ext", sk_ext, SPX_SK_BYTES);
    goto fail;
  }
  printf("\tSK matches!\n");
  ret = memcmp(sig_ext, sig_ref, SPX_BYTES);
  if(ret)
  {
    printf("\tSig does not match!\n");
    print_hex("\tsig_ref(0..31)", sig_ref, 32);
    print_hex("\tsig_ext(0..31)", sig_ext, 32);

    printf("\n");
    print_hex("\tsig_ref(32..63)", sig_ref+32, 32);
    print_hex("\tsig_ext(32..63)", sig_ext+32, 32);

    printf("\n");
    print_hex("\tsig_ref(64..95)", sig_ref+64, 32);
    print_hex("\tsig_ext(64..95)", sig_ext+64, 32);


    goto fail;
  }
  printf("\tSig matches!\n");


  printf("Success.\n");

  fail:
  if(ret)
  {
    printf("\nTHERE WAS AN ERROR!\n");
  }
  return ret;
}
