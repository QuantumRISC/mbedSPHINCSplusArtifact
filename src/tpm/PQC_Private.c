#include "Tpm.h"
#include "PQC_Private_fp.h"
#include <stdio.h>
#include "TpmTypes.h"
#include <hal.h>

#if CC_PQC_Private  // Conditional expansion of this file
#include "sphincsapi.h"
TPM_RC
TPM2_PQC_Private(
    PQC_Private_In      *in,            // IN: input parameter list
    PQC_Private_Out     *out            // OUT: output parameter list
    )
{
    TPM_RC                  result = 0;

    OBJECT                  *pqcKey;
    // retrieve key
    pqcKey = HandleToObject(in->keyHandle);

    TPM2B_PRIVATE_KEY_PQC *privkey = &(pqcKey->sensitive.sensitive.pqc); 
    TPMS_PQC_OBJECT *msg  = &(in->sig);
    streaming_ctx_t  * ctx  = NULL;

    // retrieve scheme data
    int scheme = privkey->t.buffer[crypto_sign_secretkeybytes()];
    printf("Scheme id is %d\n",scheme);
    // set parameters
    init_params(PARAMETER_SET_LIST[scheme]);
    printf("Scheme is %s\n",get_param_name(PARAMETER_SET_LIST[scheme]));
    uint64_t start = hal_get_time();
    uint64_t wstart = write_kcycles;
    // hand controlflow to sphincs code
    crypto_sign_signature_streaming(ctx,msg->data.t.buffer,msg->data.t.size,privkey->t.buffer);
    if (transfer_buffer_fill > 0) {
      write_stream_transfer(NULL, transfer_buffer, transfer_buffer_fill);
      transfer_buffer_fill = 0;
    }
    uint64_t wstop = write_kcycles;
    uint64_t stop = hal_get_time();
    printf("\n\n");
    hal_send_int("crypto-sign-signature-streaming-start-kcycles",start);
    hal_send_int("crypto-sign-signature-streaming-stop-kcycles",stop);
    //hal_send_int("sign-wstart-kcycles",wstart);
    //hal_send_int("sign-wstop-kcycles",wstop);
    hal_send_int("crypto-sign-signature-streaming-total-kcycles",stop-start);
    hal_send_int("crypto-sign-signature-streaming-write-kcycles",wstop-wstart);
    out->object.data.t.size = 13;
    // recognizable return value (signature is streamed)
    memset(out->object.data.t.buffer,0x40,out->object.data.t.size);
    return result;
}

#endif // CC_RSA_Encrypt
