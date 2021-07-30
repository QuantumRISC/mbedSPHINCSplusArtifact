#include "Tpm.h"
#include "PQC_Public_fp.h"
#if CC_PQC_Public  // Conditional expansion of this file
TPM_RC
TPM2_PQC_Public(
    PQC_Public_In      *in,            // IN: input parameter list
    PQC_Public_Out     *out            // OUT: output parameter list
    )
{
    TPM_RC                  result = 0;
//    OBJECT                  *pqcKey;
//    // retrieve the key
    //pqcKey = HandleToObject(in->keyHandle);
//
//    //pqcKey->publicArea
//
//    // selected key must be a PQC key
//    if(pqcKey->publicArea.type != TPM_ALG_PQC)
//        return TPM_RCS_KEY;
//
//    if(PQC_isKem(pqcKey->publicArea.parameters.pqc)) {
//
//    // we need to decide wether the public operation is decapsulate or verify somehow
//    // selected key must have the decryption attribute
//    if(!IS_ATTRIBUTE(rsaKey->publicArea.objectAttributes, TPMA_OBJECT, decrypt))
//        return TPM_RCS_ATTRIBUTES + RC_RSA_Encrypt_keyHandle;
//
//    //result = PQC_Kem();
//    } else { // the command implies signature verification 
//      // TODO: look at TPM2_VerifySignature for how to check the ATTRIBUTE
//
//
//    // result = there is a crypt util function for this 
//    }
    out->secret.hashAlg = TPM_ALG_SHA256;
    memset(out->secret.digest.sha256,0xff,32);
    out->object.data.t.size = 30;
    //phex("pqckey",pqcKey,sizeof(OBJECT));
    return result;
}

#endif // CC_RSA_Encrypt
