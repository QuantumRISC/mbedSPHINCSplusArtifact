#ifndef SPX_API_H
#define SPX_API_H

#include <stddef.h>
#include <stdint.h>

#include <stdio.h>
#include "params.h"

#define CRYPTO_ALGNAME "SPHINCS+"
/*
#define CRYPTO_SECRETKEYBYTES SPX_SK_BYTES
#define CRYPTO_PUBLICKEYBYTES SPX_PK_BYTES
#define CRYPTO_BYTES SPX_BYTES
#define CRYPTO_SEEDBYTES 3*SPX_N
*/
extern unsigned int CRYPTO_SECRETKEYBYTES;
extern unsigned int CRYPTO_PUBLICKEYBYTES;
extern unsigned int CRYPTO_BYTES;
extern unsigned int CRYPTO_SEEDBYTES;


/*
 * Returns the length of a secret key, in bytes
 */
unsigned long long crypto_sign_secretkeybytes(void);

/*
 * Returns the length of a public key, in bytes
 */
unsigned long long crypto_sign_publickeybytes(void);

/*
 * Returns the length of a signature, in bytes
 */
unsigned long long crypto_sign_bytes(void);

/*
 * Returns the length of the seed required to generate a key pair, in bytes
 */
unsigned long long crypto_sign_seedbytes(void);

/*
 * Generates a SPHINCS+ key pair given a seed.
 * Format sk: [SK_SEED || SK_PRF || PUB_SEED || root]
 * Format pk: [root || PUB_SEED]
 */
int crypto_sign_seed_keypair(unsigned char *pk, unsigned char *sk,
                             const unsigned char *seed);

/*
 * Generates a SPHINCS+ key pair.
 * Format sk: [SK_SEED || SK_PRF || PUB_SEED || root]
 * Format pk: [root || PUB_SEED]
 */
int crypto_sign_keypair(unsigned char *pk, unsigned char *sk);

/**
 * Returns an array containing a detached signature.
 */
int crypto_sign_signature(uint8_t *sig, size_t *siglen,
                          const uint8_t *m, size_t mlen, const uint8_t *sk);

/**
 * Verifies a detached signature and message under a given public key.
 */
int crypto_sign_verify(const uint8_t *sig, size_t siglen,
                       const uint8_t *m, size_t mlen, const uint8_t *pk);

/**
 * Returns an array containing the signature followed by the message.
 */
int crypto_sign(unsigned char *sm, unsigned long long *smlen,
                const unsigned char *m, unsigned long long mlen,
                const unsigned char *sk);

/**
 * Verifies a given signature-message pair under a given public key.
 */
int crypto_sign_open(unsigned char *m, unsigned long long *mlen,
                     const unsigned char *sm, unsigned long long smlen,
                     const unsigned char *pk);



/*
 * Streaming API
 *
 * For testing purposes use files.
 * Can be any protocol later
 */


typedef enum
{
  READ_SIG_FLAG = 0,
  WRITE_SIG_FLAG = 1
} stream_direction_flag_t;
typedef enum
{
  STREAM_LOCAL_BUFFER,
  STREAM_SERIAL
} stream_mode_t;

typedef struct
{
#ifdef TARGET_UNIX
  FILE *fp;
#endif

  /*
   * SERIAL: real communication to outside world
   * LOCAL_BUFFER: write/read everything to/from the sig buffer ptr
   */
  stream_mode_t stream_mode;
  unsigned char *sig;
  unsigned cur_sig_len;
} streaming_ctx_t;



void init_streaming_ctx(streaming_ctx_t *ctx, stream_direction_flag_t direction_flag, stream_mode_t stream_mode, unsigned char *OPTIONAL_sig_buf);
void destroy_streaming_ctx(streaming_ctx_t *ctx);

/*
 * basic read/write operations
 */
void write_stream(streaming_ctx_t *ctx, unsigned char *data, unsigned data_len);
void read_stream(streaming_ctx_t *ctx, unsigned char *data, unsigned data_len);

int crypto_sign_signature_streaming(streaming_ctx_t *ctx,
                          const uint8_t *m, size_t mlen, const uint8_t *sk);
int crypto_sign_verify_streaming(streaming_ctx_t *stream_ctx,
                                 const uint8_t *m, size_t mlen, const uint8_t *pk);

void wots_gen_leaf_streaming(unsigned char *leaf, const unsigned char *sk_seed,
                             const unsigned char *pub_seed,
                             uint32_t addr_idx, const uint32_t tree_addr[8]);

int crypto_sign_seed_keypair_streaming(unsigned char *pk, unsigned char *sk,
                                       const unsigned char *seed);

int crypto_sign_keypair_streaming(unsigned char *pk, unsigned char *sk);

#endif
