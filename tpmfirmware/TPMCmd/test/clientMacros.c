#include "clientMacros.h"
#include <stdio.h>
void phex(char *label, uint8_t *data, int len) {
  printf("%s:", label);
  for (int i = 0; i < len; i++)
    printf("%02x", data[i]);
  printf("\n");
}
// 8001[size][command_code][parameters]
void preparePQC_PUBLIC(PQC_Public_In *in, uint8_t *req, int32_t *size) {
  uint16_t command_tag = 0x8001; // TPM_ST_SESSIONS;
  TPM_CC code = TPM_CC_PQC_Public;
  printf("command_code = %lu\n", code);
  uint8_t *sizepos = req + 2;
  TPMI_ST_COMMAND_TAG_Marshal(&command_tag, &req, size);
  req += 4;
  // set command code
  TPM_CC_Marshal(&code, &req, size);
  // TPMI_DH_OBJECT keyHandle
  TPMI_DH_OBJECT_Marshal(&(in->keyHandle), &req, size);
  // TPMS_PQC_OBJECT
  TPMS_PQC_OBJECT_Marshal(&(in->sig), &req, size);
  // set size
  uint32_t cmdsize = (UINT32)(req - sizepos) + 2;
  UINT32_Marshal(&cmdsize, &sizepos, size);
  *size = cmdsize;
}

void preparePQC_PRIVATE(PQC_Private_In *in, uint8_t *req, int32_t *size, TPMS_AUTH_COMMAND *auth) {
  uint16_t command_tag = TPM_ST_SESSIONS; // private key operations need authentication!
  TPM_CC code = TPM_CC_PQC_Private;
  printf("command_code = %lu\n", code);
  uint8_t *sizepos = req + 2;
  TPMI_ST_COMMAND_TAG_Marshal(&command_tag, &req, size);
  req += 4;
  // set command code
  TPM_CC_Marshal(&code, &req, size);
  // TPMI_DH_OBJECT keyHandle
  TPMI_DH_OBJECT_Marshal(&(in->keyHandle), &req, size);
  uint8_t *authsizepos = req;
  req += 4;
  *size -= 4;
  uint32_t auth_size = TPMS_AUTH_COMMAND_Marshal(auth, &req, size);
  memset(authsizepos,0xff,4); 
  int32_t sizesize = 4;
  UINT32_Marshal(&auth_size, &authsizepos,&sizesize);
  // TPMS_PQC_OBJECT
  TPMS_PQC_OBJECT_Marshal(&(in->sig), &req, size);
  // set size
  uint32_t cmdsize = (UINT32)(req - sizepos) + 2;
  UINT32_Marshal(&cmdsize, &sizepos, size);
  *size = cmdsize;
}

uint32_t TPMS_AUTH_COMMAND_Marshal(TPMS_AUTH_COMMAND *auth,uint8_t **req, int32_t *size) {
  uint8_t *start = *req;
  TPMI_SH_AUTH_SESSION_Marshal(&(auth->sessionHandle),req,size);
  TPM2B_NONCE_Marshal(&(auth->nonce),req,size);
  TPMA_SESSION_Marshal(&(auth->sessionAttributes),req,size);
  TPM2B_AUTH_Marshal(&(auth->hmac), req, size);
  uint8_t *stop = *req;
  return stop - start;
}


void TPMS_SENSITIVE_CREATE_Marshal(TPMS_SENSITIVE_CREATE *in, uint8_t **req, int32_t *size) {
  TPM2B_AUTH_Marshal(&(in->userAuth), req, size);
  TPM2B_SENSITIVE_DATA_Marshal(&(in->data), req, size);
}

void TPM2B_SENSITIVE_CREATE_Marshal(TPM2B_SENSITIVE_CREATE *in, uint8_t **req, int32_t *size) {
    UINT16_Marshal(&(in->size), req, size);
    TPMS_SENSITIVE_CREATE_Marshal(&(in->sensitive),req,size);
}

// 8002[size][command_code][parameters][auth]
void prepareTPM_Create(Create_In *in, uint8_t *req, int32_t *size, TPMS_AUTH_COMMAND *auth) {
  uint16_t command_tag = TPM_ST_SESSIONS;
  TPM_CC code = TPM_CC_Create;
  uint8_t *sizepos = req + 2;
  TPMI_ST_COMMAND_TAG_Marshal(&command_tag, &req, size);
  req += 4;
  TPM_CC_Marshal(&code, &req, size);
  TPMI_DH_OBJECT_Marshal(&(in->parentHandle), &req, size);
  uint8_t *authsizepos = req;
  req += 4;
  *size -= 4;
  uint32_t auth_size = TPMS_AUTH_COMMAND_Marshal(auth, &req, size);
  memset(authsizepos,0xff,4);
  int32_t sizesize = 4;
  UINT32_Marshal(&auth_size, &authsizepos,&sizesize);
  // inSensitive
  TPMS_SENSITIVE_CREATE_Marshal(&(in->inSensitive), &req, size);
  // inPublic
  req -= 2;
  TPM2B_PUBLIC_Marshal(&(in->inPublic), &req, size);
  // outsideInfo
  TPM2B_DATA_Marshal(&(in->outsideInfo), &req, size);
  // creationPCR
  TPML_PCR_SELECTION_Marshal(&(in->creationPCR), &req, size);
  // auth
  uint32_t cmdsize = (UINT32)(req - sizepos) + 2;
  UINT32_TO_BYTE_ARRAY(cmdsize, sizepos);
  *size = cmdsize;
}

void prepareTPM_CreateLoaded(CreateLoaded_In *source, uint8_t *req, int32_t *size, TPMS_AUTH_COMMAND *auth) {
  uint16_t command_tag = TPM_ST_SESSIONS;
  TPM_CC code = TPM_CC_CreateLoaded;
  uint8_t *sizepos = req + 2;
  TPMI_ST_COMMAND_TAG_Marshal(&command_tag, &req, size);
  req += 4;
  TPM_CC_Marshal(&code, &req, size);
  TPMI_DH_OBJECT_Marshal(&(source->parentHandle), &req, size);
  uint8_t *authsizepos = req;
  req += 4;
  //*size -= 4;
  uint32_t auth_size = TPMS_AUTH_COMMAND_Marshal(auth, &req, size);
  memset(authsizepos,0xff,4);
  int32_t sizesize = 4;
  UINT32_Marshal(&auth_size, &authsizepos,&sizesize);

  TPM2B_SENSITIVE_CREATE_Marshal(&(source->inSensitive), &req, size);
  TPM2B_TEMPLATE_Marshal(&(source->inPublic), &req, size);
  // inSensitive

  // inPublic
  //req -= 2;
  //TPM2B_PUBLIC_Marshal(&(source->inPublic), &req, size);


  // cmd size set
  uint32_t cmdsize = (UINT32)(req - sizepos) + 2;
  UINT32_TO_BYTE_ARRAY(cmdsize, sizepos);
  *size = cmdsize;
}

void prepareTPM_Load(Load_In *in, uint8_t *req, int32_t *size, TPMS_AUTH_COMMAND *auth) {
  uint16_t command_tag = TPM_ST_SESSIONS;
  TPM_CC code = TPM_CC_Load;
  uint8_t *sizepos = req + 2;
  TPMI_ST_COMMAND_TAG_Marshal(&command_tag, &req, size);
  req += 4;
  TPM_CC_Marshal(&code, &req, size);
  TPMI_DH_OBJECT_Marshal(&(in->parentHandle), &req, size);
  uint8_t *authsizepos = req;
  req += 4;
  *size -= 4;
  uint32_t auth_size = TPMS_AUTH_COMMAND_Marshal(auth, &req, size);
  memset(authsizepos,0xff,4);
  int32_t sizesize = 4;
  UINT32_Marshal(&auth_size, &authsizepos,&sizesize);
//  // inSensitive
  TPM2B_PRIVATE_Marshal(&(in->inPrivate), &req, size);
//  // inPublic
  TPM2B_PUBLIC_Marshal(&(in->inPublic), &req, size);
//  // size
  uint32_t cmdsize = (UINT32)(req - sizepos) + 2;
  UINT32_TO_BYTE_ARRAY(cmdsize, sizepos);
  *size = cmdsize;
}

void TPM2B_CREATION_DATA_Unmarshal(TPM2B_CREATION_DATA *target,uint8_t **req,int32_t *size) {
  UINT16_Unmarshal(&(target->size),req,size);
  *size=*size + 2 - target->size;
  //TPMS_CREATION_DATA_Unmarshal(target->creationData,req,size);
}
int parseTPM_Create(Create_Out *target, uint8_t *data, int32_t size) {
  int32_t s_size = 0;
  data += 2;
  UINT32_Unmarshal(&s_size,&data,&size);
  TPM2B_PRIVATE_Unmarshal(&(target->outPrivate),&data,&size);
  TPM2B_PUBLIC_Unmarshal(&(target->outPublic),&data,&size,0);
  TPM2B_CREATION_DATA_Unmarshal(&(target->creationData),&data,&size);
  TPM2B_DIGEST_Unmarshal(&(target->creationHash),&data,&size);
  TPMT_TK_CREATION_Unmarshal(&(target->creationTicket),&data,&size);
  return s_size - size;
}

void prepareTPM_Startup(Startup_In *in, uint8_t *req, int32_t *size) {
  uint16_t command_tag =  0x8001;//TPM_ST_SESSIONS;
  uint8_t *sizepos = req + 2;
  TPM_CC code = TPM_CC_Startup;
  TPMI_ST_COMMAND_TAG_Marshal(&command_tag,&req,size);
  req += 4;
  TPM_CC_Marshal(&code, &req, size);
  UINT16_Marshal(&(in->startupType),&req,size);
  uint32_t cmdsize = (UINT32)(req - sizepos)+2;
  UINT32_Marshal(&cmdsize,&sizepos,size);
  *size = cmdsize;
}

uint32_t CP() {

  CreatePrimary_Out out;
  out.objectHandle = 0;
  // TPMA_OBJECT_USERWITHAUTH |       TPMA_OBJECT_RESTRICTED |  /* TODO what
  // TPMA_OBJECT_SIGN_ENCRYPT |       TPMA_OBJECT_FIXEDTPM |
  // TPMA_OBJECT_FIXEDPARENT |       TPMA_OBJECT_SENSITIVEDATAORIGIN
  CreatePrimary_In in = {
      .primaryHandle = 0x40000000,
      .inPublic =
          {.size = 0,
           .publicArea = {.type = TPM_ALG_SYMCIPHER,
                          .nameAlg = TPM_ALG_SHA256,
                          .objectAttributes =
                              TPMA_OBJECT_userWithAuth |
                              TPMA_OBJECT_restricted | TPMA_OBJECT_decrypt |
                              TPMA_OBJECT_fixedTPM | TPMA_OBJECT_fixedParent |
                              TPMA_OBJECT_sensitiveDataOrigin,
                          .authPolicy = {{
                              .size = 0,
                          }},
                          .parameters.symDetail.sym = {.algorithm = TPM_ALG_AES,
                                                       .keyBits = 256,
                                                       .mode = TPM_ALG_CFB},
                          .unique.sym.t = {.size = 32, .buffer = {0}}}},
  };
  TPM_RC rc = TPM2_CreatePrimary(&in, &out);
  printf("RC is %lu\n", rc);
  return out.objectHandle;
}
