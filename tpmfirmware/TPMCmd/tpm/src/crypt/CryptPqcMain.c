#include "Tpm.h"

#if ALG_PQC
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
    printf("Implement this function %s:%d\n",__FILE__,__LINE__);

    sensitive->sensitiveType = TPM_ALG_PQC;
    TPM2B_PRIVATE_KEY_PQC *pqc = &(sensitive->sensitive.pqc);
    pqc->t.size = 32;
    memset(pqc->t.buffer,0x42,32);

    return retVal;
}
#endif
