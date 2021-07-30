#if CC_PQC_Public
#ifndef _PQC_Public_FP_H_
#define _PQC_Public_FP_H_
// Input structure definition
typedef struct {
    TPMI_DH_OBJECT              keyHandle;
    TPMS_PQC_OBJECT             sig; // set length to zero if KEM scheme is used
} PQC_Public_In;
// Output structure definition
typedef struct {
    TPMT_HA          secret; // set length to zero if sign
    TPMS_PQC_OBJECT  object; // KEM or message
} PQC_Public_Out;
// Function prototype
TPM_RC
TPM2_PQC_Public(
    PQC_Public_In              *in,
    PQC_Public_Out             *out
);
#endif  // _PQC_Public_FP_H_
#endif  // CC_PQC_Public
