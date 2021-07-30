#if CC_PQC_Private
#ifndef _PQC_Private_FP_H_
#define _PQC_Private_FP_H_
// Input structure definition
typedef struct {
    TPMI_DH_OBJECT              keyHandle;
    TPMS_PQC_OBJECT             sig; // message or kem
} PQC_Private_In;
// Output structure definition
typedef struct {
    TPMT_HA          secret; // if KEM
    TPMS_PQC_OBJECT  object; // nothing or Signature
} PQC_Private_Out;
// Function prototype
TPM_RC
TPM2_PQC_Private(
    PQC_Private_In              *in,
    PQC_Private_Out             *out
);
#endif  // _PQC_Private_FP_H_
#endif  // CC_PQC_Private
