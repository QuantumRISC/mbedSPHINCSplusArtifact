#include "Tpm.h"
#include "PQC_Private_fp.h"
#if CC_PQC_Private  // Conditional expansion of this file
TPM_RC
TPM2_PQC_Private(
    PQC_Private_In      *in,            // IN: input parameter list
    PQC_Private_Out     *out            // OUT: output parameter list
    )
{
      TPM_RC                  result = 0;

     OBJECT                  *pqcKey;
     pqcKey = HandleToObject(in->keyHandle);
     (void) pqcKey;
//    // retrieve the key
//    pqcKey = HandleToObject(in->keyHandle);
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
//    if(!IS_ATTRIBUTE(pqcKey->publicArea.objectAttributes, TPMA_OBJECT, decrypt))
//        return TPM_RCS_ATTRIBUTES + RC_RSA_Encrypt_keyHandle;
//
//    //result = PQC_Kem();
//    } else { // the command implies signature verification 
//      // TODO: look at TPM2_VerifySignature for how to check the ATTRIBUTE
//
//
//    // result = there is a crypt util function for this 
//    }
    printf("%s:%d reached\n",__FILE__,__LINE__);
    return result;
}

#endif // CC_RSA_Encrypt
