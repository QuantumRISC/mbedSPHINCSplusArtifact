VPATH += ../../sphincsplus



SPHINCS_SRC_PATH=sphincs-sha-shake
SPHINCS_SRC = $(SPHINCS_SRC_PATH)/address.c \
							$(SPHINCS_SRC_PATH)/fors.c \
							$(SPHINCS_SRC_PATH)/thash_sha256_robust.c \
							$(SPHINCS_SRC_PATH)/thash_shake256_robust.c \
							$(SPHINCS_SRC_PATH)/thash_shake256_simple.c \
							$(SPHINCS_SRC_PATH)/thash_sha256_simple.c \
							$(SPHINCS_SRC_PATH)/thash.c \
							$(SPHINCS_SRC_PATH)/utils.c \
							$(SPHINCS_SRC_PATH)/utilsx1.c \
							$(SPHINCS_SRC_PATH)/fips202.c \
							$(SPHINCS_SRC_PATH)/hash_sha256.c \
							$(SPHINCS_SRC_PATH)/hash_shake256.c \
							$(SPHINCS_SRC_PATH)/rng.c  \
							$(SPHINCS_SRC_PATH)/sign.c \
							$(SPHINCS_SRC_PATH)/sha256.c\
							$(SPHINCS_SRC_PATH)/merkle.c\
  						$(SPHINCS_SRC_PATH)/hash.c\
							$(SPHINCS_SRC_PATH)/params.c\
							$(SPHINCS_SRC_PATH)/wots.c \
							$(SPHINCS_SRC_PATH)/wotsx1.c 

#stm32f4.c
#utils.c utilsx1.c wots.c wotsx1.c


#CFLAGS+=-DUSE_HASH_SHA256 
CFLAGS+=-I../../sphincsplus/${SPHINCS_SRC_PATH}



SPHINCS_OBJ = $(call objs, $(SPHINCS_SRC))

#libsphincs.a: $(SPHINCS_OBJ)
#	ar rcs libsphincs.a $(SPHINCS_OBJ)

tpm.elf: CFLAGS+=-DOVERRIDE_STREAM_READWRITE -DNO_MAIN -DENABLE_ROBUST -DSPHINCS_SCHEME=6 -DVARIANT_SHA2_ROBUST -DVARIANT_SHA2_SIMPLE -DVARIANT_SHAKE_ROBUST -DVARIANT_SHAKE_SIMPLE
tpm.elf: $(WOLF_OBJ) $(TPM_OBJ) $(SPHINCS_OBJ)
