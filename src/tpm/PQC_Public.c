#include "Tpm.h"
#include "PQC_Public_fp.h"
#include "api.h"
#if CC_PQC_Public  // Conditional expansion of this file

#include "sphincsapi.h"

TPM_RC
TPM2_PQC_Public(
    PQC_Public_In      *in,            // IN: input parameter list
    PQC_Public_Out     *out            // OUT: output parameter list
    )
{
    TPM_RC                  result = 0;
    OBJECT                  *pqcKey;
    // retrieve the key
    pqcKey = HandleToObject(in->keyHandle);
    out->secret.hashAlg = TPM_ALG_SHA256;

    printf("reached PQC_Public\n");

    out->object.data.t.size = 30;
    memset(out->secret.digest.sha256,0xff,32);

    streaming_ctx_t ctx;
    size_t siglen = SPX_N;
     // retrieve scheme data from the key pair
    int scheme = pqcKey->publicArea.unique.pqc.t.buffer[crypto_sign_publickeybytes()];
    printf("Scheme id is %d\n",scheme);
    // set parameters acordingly
    init_params(PARAMETER_SET_LIST[scheme]);
    printf("Scheme is %s\n",get_param_name(PARAMETER_SET_LIST[scheme]));
    siglen = SPX_BYTES;

    transfer_bytes_remaining = SPX_BYTES;
    uint64_t start = hal_get_time();
    uint64_t rstart = read_kcycles;
    // hand control over to the sphincs code
    int ret = crypto_sign_verify_streaming(&ctx, siglen, in->sig.data.t.buffer, in->sig.data.t.size, pqcKey->publicArea.unique.pqc.t.buffer);
    uint64_t rstop = read_kcycles;
    uint64_t stop = hal_get_time();
    hal_send_int("crypto-sign-verify-streaming-start-kcycles",start);
    hal_send_int("crypto-sign-verify-streaming-kcycles",stop);
    hal_send_int("crypto-sign-verify-streaming-total-kcycles",stop-start);
    hal_send_int("crypto-sign-verify-streaming-read-kcycles",rstop-rstart);

    printf("crypto_sign_verify_streaming retured %d\n",ret);
    // different return values that are easy to spot in the response (signature is streamed)
    if (ret){
      memset(out->object.data.t.buffer,0xff,32);
      out->object.data.t.size = 32;
    } else {
      memset(out->object.data.t.buffer,0x40,32);
      out->object.data.t.size = 32;
    }
    return result;
}

#endif // CC_RSA_Encrypt
