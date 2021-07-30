VPATH += ../tpm/
VPATH += ../../tpmfirmware/TPMCmd/
TPM_SRC := Platform/src/Cancel.c \
Platform/src/DebugHelpers.c Platform/src/Entropy.c \
Platform/src/LocalityPlat.c Platform/src/NVMem.c Platform/src/PPPlat.c \
Platform/src/PlatformACT.c Platform/src/PlatformData.c Platform/src/PowerPlat.c \
Platform/src/RunCommand.c  \
Platform/src/Unique.c tpm/src/X509/X509_ECC.c tpm/src/X509/X509_RSA.c tpm/src/X509/X509_spt.c tpm/src/X509/TpmASN1.c \
tpm/src/command/Asymmetric/ECC_Decrypt.c \
tpm/src/command/Asymmetric/ECC_Encrypt.c \
tpm/src/command/Asymmetric/ECC_Parameters.c \
tpm/src/command/Asymmetric/ECDH_KeyGen.c tpm/src/command/Asymmetric/ECDH_ZGen.c \
tpm/src/command/Asymmetric/EC_Ephemeral.c \
tpm/src/command/Asymmetric/RSA_Decrypt.c \
tpm/src/command/Asymmetric/RSA_Encrypt.c \
tpm/src/command/Asymmetric/ZGen_2Phase.c \
tpm/src/command/AttachedComponent/AC_GetCapability.c \
tpm/src/command/AttachedComponent/AC_Send.c \
tpm/src/command/AttachedComponent/AC_spt.c \
tpm/src/command/AttachedComponent/Policy_AC_SendSelect.c \
tpm/src/command/Attestation/Attest_spt.c tpm/src/command/Attestation/Certify.c \
tpm/src/command/Attestation/CertifyCreation.c \
tpm/src/command/Attestation/CertifyX509.c \
tpm/src/command/Attestation/GetCommandAuditDigest.c \
tpm/src/command/Attestation/GetSessionAuditDigest.c \
tpm/src/command/Attestation/GetTime.c tpm/src/command/Attestation/Quote.c \
tpm/src/command/Capability/GetCapability.c \
tpm/src/command/Capability/TestParms.c \
tpm/src/command/ClockTimer/ACT_SetTimeout.c \
tpm/src/command/ClockTimer/ACT_spt.c \
tpm/src/command/ClockTimer/ClockRateAdjust.c \
tpm/src/command/ClockTimer/ClockSet.c tpm/src/command/ClockTimer/ReadClock.c \
tpm/src/command/CommandAudit/SetCommandCodeAuditStatus.c \
tpm/src/command/Context/ContextLoad.c tpm/src/command/Context/ContextSave.c \
tpm/src/command/Context/Context_spt.c tpm/src/command/Context/EvictControl.c \
tpm/src/command/Context/FlushContext.c \
tpm/src/command/DA/DictionaryAttackLockReset.c \
tpm/src/command/DA/DictionaryAttackParameters.c \
tpm/src/command/Duplication/Duplicate.c tpm/src/command/Duplication/Import.c \
tpm/src/command/Duplication/Rewrap.c tpm/src/command/EA/PolicyAuthValue.c \
tpm/src/command/EA/PolicyAuthorize.c tpm/src/command/EA/PolicyAuthorizeNV.c \
tpm/src/command/EA/PolicyCommandCode.c tpm/src/command/EA/PolicyCounterTimer.c \
tpm/src/command/EA/PolicyCpHash.c tpm/src/command/EA/PolicyDuplicationSelect.c \
tpm/src/command/EA/PolicyGetDigest.c tpm/src/command/EA/PolicyLocality.c \
tpm/src/command/EA/PolicyNV.c tpm/src/command/EA/PolicyNameHash.c \
tpm/src/command/EA/PolicyNvWritten.c tpm/src/command/EA/PolicyOR.c \
tpm/src/command/EA/PolicyPCR.c tpm/src/command/EA/PolicyPassword.c \
tpm/src/command/EA/PolicyPhysicalPresence.c tpm/src/command/EA/PolicySecret.c \
tpm/src/command/EA/PolicySigned.c tpm/src/command/EA/PolicyTemplate.c \
tpm/src/command/EA/PolicyTicket.c tpm/src/command/EA/Policy_spt.c \
tpm/src/command/Ecdaa/Commit.c tpm/src/command/FieldUpgrade/FieldUpgradeData.c \
tpm/src/command/FieldUpgrade/FieldUpgradeStart.c \
tpm/src/command/FieldUpgrade/FirmwareRead.c \
tpm/src/command/HashHMAC/EventSequenceComplete.c \
tpm/src/command/HashHMAC/HMAC_Start.c \
tpm/src/command/HashHMAC/HashSequenceStart.c \
tpm/src/command/HashHMAC/MAC_Start.c \
tpm/src/command/HashHMAC/SequenceComplete.c \
tpm/src/command/HashHMAC/SequenceUpdate.c tpm/src/command/Hierarchy/ChangeEPS.c \
tpm/src/command/Hierarchy/ChangePPS.c tpm/src/command/Hierarchy/Clear.c \
tpm/src/command/Hierarchy/ClearControl.c \
tpm/src/command/Hierarchy/CreatePrimary.c \
tpm/src/command/Hierarchy/HierarchyChangeAuth.c \
tpm/src/command/Hierarchy/HierarchyControl.c \
tpm/src/command/Hierarchy/SetPrimaryPolicy.c tpm/src/command/Misc/PP_Commands.c \
tpm/src/command/Misc/SetAlgorithmSet.c tpm/src/command/NVStorage/NV_Certify.c \
tpm/src/command/NVStorage/NV_ChangeAuth.c \
tpm/src/command/NVStorage/NV_DefineSpace.c \
tpm/src/command/NVStorage/NV_Extend.c \
tpm/src/command/NVStorage/NV_GlobalWriteLock.c \
tpm/src/command/NVStorage/NV_Increment.c tpm/src/command/NVStorage/NV_Read.c \
tpm/src/command/NVStorage/NV_ReadLock.c \
tpm/src/command/NVStorage/NV_ReadPublic.c \
tpm/src/command/NVStorage/NV_SetBits.c \
tpm/src/command/NVStorage/NV_UndefineSpace.c \
tpm/src/command/NVStorage/NV_UndefineSpaceSpecial.c \
tpm/src/command/NVStorage/NV_Write.c tpm/src/command/NVStorage/NV_WriteLock.c \
tpm/src/command/NVStorage/NV_spt.c tpm/src/command/Object/ActivateCredential.c \
tpm/src/command/Object/Create.c tpm/src/command/Object/CreateLoaded.c \
tpm/src/command/Object/Load.c tpm/src/command/Object/LoadExternal.c \
tpm/src/command/Object/MakeCredential.c \
tpm/src/command/Object/ObjectChangeAuth.c tpm/src/command/Object/Object_spt.c \
tpm/src/command/Object/ReadPublic.c tpm/src/command/Object/Unseal.c \
tpm/src/command/PCR/PCR_Allocate.c tpm/src/command/PCR/PCR_Event.c \
tpm/src/command/PCR/PCR_Extend.c tpm/src/command/PCR/PCR_Read.c \
tpm/src/command/PCR/PCR_Reset.c tpm/src/command/PCR/PCR_SetAuthPolicy.c \
tpm/src/command/PCR/PCR_SetAuthValue.c tpm/src/command/Random/GetRandom.c \
tpm/src/command/Random/StirRandom.c tpm/src/command/Session/PolicyRestart.c \
tpm/src/command/Session/StartAuthSession.c tpm/src/command/Signature/Sign.c \
tpm/src/command/Signature/VerifySignature.c tpm/src/command/Startup/Shutdown.c \
tpm/src/command/Startup/Startup.c tpm/src/command/Symmetric/EncryptDecrypt.c \
tpm/src/command/Symmetric/EncryptDecrypt2.c \
tpm/src/command/Symmetric/EncryptDecrypt_spt.c tpm/src/command/Symmetric/HMAC.c \
tpm/src/command/Symmetric/Hash.c tpm/src/command/Symmetric/MAC.c \
tpm/src/command/Testing/GetTestResult.c \
tpm/src/command/Testing/IncrementalSelfTest.c \
tpm/src/command/Testing/SelfTest.c tpm/src/command/Vendor/Vendor_TCG_Test.c \
tpm/src/crypt/AlgorithmTests.c tpm/src/crypt/BnConvert.c tpm/src/crypt/BnMath.c \
tpm/src/crypt/BnMemory.c tpm/src/crypt/CryptCmac.c tpm/src/crypt/CryptDes.c \
tpm/src/crypt/CryptEccCrypt.c tpm/src/crypt/CryptEccData.c \
tpm/src/crypt/CryptEccKeyExchange.c tpm/src/crypt/CryptEccMain.c \
tpm/src/crypt/CryptEccSignature.c \
tpm/src/crypt/CryptHash.c tpm/src/crypt/CryptPrime.c \
tpm/src/crypt/CryptPrimeSieve.c tpm/src/crypt/CryptRand.c \
tpm/src/crypt/CryptRsa.c tpm/src/crypt/CryptSelfTest.c \
tpm/src/crypt/CryptSmac.c tpm/src/crypt/CryptSym.c tpm/src/crypt/CryptUtil.c \
tpm/src/crypt/PrimeData.c tpm/src/crypt/RsaKeyCache.c tpm/src/crypt/Ticket.c \
tpm/src/crypt/wolf/TpmToWolfDesSupport.c tpm/src/crypt/wolf/TpmToWolfMath.c \
tpm/src/crypt/wolf/TpmToWolfSupport.c tpm/src/events/_TPM_Hash_Data.c \
tpm/src/events/_TPM_Hash_End.c tpm/src/events/_TPM_Hash_Start.c \
tpm/src/events/_TPM_Init.c tpm/src/main/CommandDispatcher.c \
tpm/src/main/ExecCommand.c tpm/src/main/SessionProcess.c \
tpm/src/subsystem/CommandAudit.c tpm/src/subsystem/DA.c \
tpm/src/subsystem/Hierarchy.c tpm/src/subsystem/NvDynamic.c \
tpm/src/subsystem/NvReserved.c tpm/src/subsystem/Object.c \
tpm/src/subsystem/PCR.c tpm/src/subsystem/PP.c tpm/src/subsystem/Session.c \
tpm/src/subsystem/Time.c tpm/src/support/AlgorithmCap.c tpm/src/support/Bits.c \
tpm/src/support/CommandCodeAttributes.c tpm/src/support/Entity.c \
tpm/src/support/Global.c tpm/src/support/Handle.c tpm/src/support/IoBuffers.c \
tpm/src/support/Locality.c tpm/src/support/Manufacture.c \
tpm/src/support/Marshal.c tpm/src/support/MathOnByteBuffers.c \
tpm/src/support/Memory.c tpm/src/support/Power.c tpm/src/support/PropertyCap.c \
tpm/src/support/Response.c tpm/src/support/ResponseCodeProcessing.c \
tpm/src/support/TableDrivenMarshal.c tpm/src/support/TableMarshalData.c \
tpm/src/support/TpmFail.c tpm/src/support/TpmSizeChecks.c \
test/clientMacros.c \



TPM_SRC += Clock.c Main.c tis.c CryptPqcMain.c stream.c PQC_Private.c tpm/PQC_Public.c 
VPATH += ../../tpmfirmware/external/wolfssl/wolfcrypt/src/
WOLF_SRC= aes.c arc4.c asm.c asn.c blake2b.c blake2s.c camellia.c chacha20_poly1305.c chacha.c cmac.c coding.c compress.c cpuid.c cryptocb.c curve25519.c curve448.c des3.c dh.c dsa.c ecc.c ecc_fp.c ed25519.c ed448.c error.c evp.c fe_448.c fe_low_mem.c fe_operations.c ge_448.c ge_low_mem.c ge_operations.c hash.c hc128.c hmac.c idea.c integer.c logging.c md2.c md4.c md5.c memory.c misc.c pkcs12.c pkcs7.c poly1305.c pwdbased.c rabbit.c random.c rc2.c ripemd.c rsa.c sha256.c sha3.c sha512.c sha.c signature.c sp_arm32.c sp_arm64.c sp_armthumb.c sp_c32.c sp_c64.c sp_cortexm.c sp_dsp32.c sp_int.c sp_x86_64.c srp.c tfm.c wc_dsp.c wc_encrypt.c wc_pkcs11.c wc_port.c wolfevent.c wolfmath.c
TPM_OBJ = $(call objs, $(TPM_SRC))
WOLF_OBJ = $(call objs, $(WOLF_SRC))
OBJ += $(TPM_OBJ)
CFLAGS+=-DWITH_SPI 
CFLAGS+=-DUSE_DA_USED=NO
CFLAGS+= -I../../tpmfirmware/TPMCmd/tpm/include/Wolf/ -I../../tpmfirmware/TPMCmd/Platform/include/ -I../../tpmfirmware/TPMCmd/tpm/include/ -I../../tpmfirmware/TPMCmd/Platform/include/prototypes/ -I../../tpmfirmware/TPMCmd/tpm/include/prototypes/ -I../../tpmfirmware/TPMCmd/test/
CFLAGS+=-DHASH_LIB=Wolf -DSYM_LIB=Wolf  -DMATH_LIB=Wolf -I../../tpmfirmware/external/wolfssl/ -I../../tpmfirmware/TPMCmd/tpm/include/prototypes/
CFLAGS+=-DWC_NO_HARDEN -DNO_WOLFSSL_DIR -DNO_WRITEV -DNO_STDIO_FILESYSTEM
ecc.c.o: CFLAGS+=-DECC_SHAMIR -DHAVE_ECC
integer.c.o:  CFLAGS+=-DECC_SHAMIR -DHAVE_ECC
tfm.c.o: CFLAGS+=-DECC_SHAMIR -DHAVE_ECC
sha512.c.o: CFLAGS+=-DWOLFSSL_SHA384
CFLAGS+=-fstack-usage
CFLAGS+=-DHAVE_TIME_T_TYPE
#CFLAGS+=-DTIME_OVERRIDES
CFLAGS+=-DWOLF_SHA384
CFLAGS+=-DWOLFSSL_AES_NO_UNROLL
CFLAGS+=-DLIB_EXPORT= -include stdint.h -DINT32=int32_t -DBYTE=uint8_t

CFLAGS += -DSINGLE_THREADED       \
                        -DNO_WOLFSSL_CLIENT     \
                        -DNO_WOLFSSL_SERVER     \
                        -DOPENSSL_EXTRA         \
                        -DNO_FILESYSTEM         \
                        -DWOLFSSL_USER_SETTINGS \
                        -DSTRING_USER           \
                        -DCTYPE_USER            \
                        -DCERTIFYX509_DEBUG=NO  \
												-Wno-unused-function    \
												-DNO_INLINE

