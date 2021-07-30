#ifndef    _CRYPT_PQC_MAIN_FP_H_
#define    _CRYPT_PQC_MAIN_FP_H_
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
);
#endif  // ALG_PQC
#endif  // _CRYPT_PQC_MAIN_FP_H_
