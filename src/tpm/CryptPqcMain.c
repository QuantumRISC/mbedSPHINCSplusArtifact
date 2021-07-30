#include "Tpm.h"
#include <api.h>

#if ALG_PQC
#include "sphincsapi.h"

LIB_EXPORT TPM_RC
CryptPqcGenerateKey(
    TPMT_PUBLIC         *publicArea,        // IN/OUT: The public area template for
                                            //      the new key. The public key
                                            //      area will be replaced computed
                                            //      ECC public key
    TPMT_SENSITIVE      *sensitive,         // OUT: the sensitive area will be
                                            //      updated to contain the private
                                            //      ECC key and the symmetric
                                            //      encryption key
    RAND_STATE          *rand               // IN: if not NULL, the deterministic
                                            //     RNG state
    )
{
    BOOL                     OK;
    (void) OK;
    TPM_RC                   retVal = TPM_RC_SUCCESS;
    printf("%s:%d reached\n",__FILE__,__LINE__);
    sensitive->sensitiveType = TPM_ALG_PQC;
    // this is the private key
    TPM2B_PRIVATE_KEY_PQC *sk = &(sensitive->sensitive.pqc);
    // this is the public key same rules as with the secret key
    TPM2B_PUBLIC_KEY_PQC *pk = &(publicArea->unique.pqc);
    // this is the only the the host-system can influence the sphics parameters
    int scheme = publicArea->parameters.pqcDetail;
    printf("Scheme id is %08x\n",scheme);
    // set correct parameters
    init_params(PARAMETER_SET_LIST[scheme]);
    printf("Scheme is %s\n",get_param_name(PARAMETER_SET_LIST[scheme]));
    uint64_t start = hal_get_time();
    // hand controlflow to sphincs code
    crypto_sign_keypair(pk->t.buffer, sk->t.buffer);
    uint64_t stop = hal_get_time();
    hal_send_int("crypto-sign-keypair-start-kcycles",start);
    hal_send_int("crypto-sign-keypair-stop-kcycles",stop);
    hal_send_int("crypto-sign-keypair-total-kcycles",stop-start);
    pk->t.size = crypto_sign_publickeybytes()+1;
    sk->t.size = crypto_sign_secretkeybytes()+1;
    printf("(%d,%d)\n",pk->t.size,sk->t.size);
    // write scheme to keypair so it can be referenced during signing and verification
    pk->t.buffer[crypto_sign_publickeybytes()] = scheme;
    sk->t.buffer[crypto_sign_secretkeybytes()] = scheme;
    return retVal;
}
#endif
