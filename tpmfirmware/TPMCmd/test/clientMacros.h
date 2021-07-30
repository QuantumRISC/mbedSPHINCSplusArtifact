#ifndef _CLIENT_MACROS_H
#define _CLIENT_MACROS_H
#include <stdint.h>
#include "BaseTypes.h"
#include "Tpm.h"
#include "TpmAlgorithmDefines.h"
#include "TpmTypes.h"
#include "ExecCommand_fp.h"
#include "TpmTypes.h"
#include "Marshal_fp.h"
#include "PQC_Public_fp.h"
#include "PQC_Private_fp.h"
#include "CreatePrimary_fp.h"
#include "CreateLoaded_fp.h"
#include "Create_fp.h"
#include "Load_fp.h"
#include "Startup_fp.h"
#include "Tpm.h"
#include "_TPM_Init_fp.h"
void phex(char *label, uint8_t *data, int len);
// 8001[size][command_code][parameters]
void preparePQC_PUBLIC(PQC_Public_In *in, uint8_t *req, int32_t *size);
uint32_t TPMS_AUTH_COMMAND_Marshal(TPMS_AUTH_COMMAND *auth,uint8_t **req, int32_t *size);
void TPMS_SENSITIVE_CREATE_Marshal(TPMS_SENSITIVE_CREATE *in, uint8_t **req, int32_t *size);
void TPM2B_SENSITIVE_CREATE_Marshal(TPM2B_SENSITIVE_CREATE *in, uint8_t **req, int32_t *size);
// 8002[size][command_code][parameters][auth]
void prepareTPM_Create(Create_In *in, uint8_t *req, int32_t *size, TPMS_AUTH_COMMAND *auth);
void prepareTPM_CreateLoaded(CreateLoaded_In *source, uint8_t *buffer, int32_t *size, TPMS_AUTH_COMMAND *auth);
void prepareTPM_Load(Load_In *in, uint8_t *data, int32_t *size, TPMS_AUTH_COMMAND *auth);
void prepareTPM_Startup(Startup_In *in, uint8_t *req, int32_t *size);
uint32_t CP();
int parseTPM_Create(Create_Out *target, uint8_t *data, int32_t size);

#endif
