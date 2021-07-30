SHELL = sh -xv

ifdef SRCDIR

VPATH = $(SRCDIR)

PARAMS ?= sphincs-shake256-128f
THASH ?= robust

# Add your targets here
TARGETS = spx.hex

CFLAGS += -O2 -DPARAMS=$(PARAMS) -DPARAMS_STR=\"$(PARAMS)-$(THASH)\"

all: $(TARGETS)

include config.mk

# For each target define a TARGETNAME_SRC, TARGETNAME_OBJ and define any
# additional dependencies for your the target TARGETNAME.elf file (just
# define the dependencies, a generic rule for .elf target exists in
# config.mk).


SOURCES = stm32f4.c address.c randombytes.c merkle.c wots.c wotsx1.c utils.c utilsx1.c fors.c sign.c
HEADERS = params.h address.h randombytes.h merkle.h wots.h wotsx1.h utils.h utilsx1.h fors.h api.h  hash.h thash.h

ifneq (,$(findstring shake256,$(PARAMS)))
	SOURCES += fips202.c hash_shake256.c thash_shake256_$(THASH).c
	HEADERS += fips202.h
endif
ifneq (,$(findstring haraka,$(PARAMS)))
	SOURCES += haraka.c hash_haraka.c thash_haraka_$(THASH).c
	HEADERS += haraka.h
endif
ifneq (,$(findstring sha256,$(PARAMS)))
	SOURCES += sha256.c hash_sha256.c thash_sha256_$(THASH).c
	HEADERS += sha256.h
endif


DET_SOURCES = $(SOURCES:rng.%=rng.%)
DET_HEADERS = $(HEADERS:rng.%=rng.%)

SPX_SRC = ${SOURCES} $(TARGET).c
SPX_OBJ = $(call objs,$(SPX_SRC))
spx.elf: $(SPX_OBJ) libhal.a

# Don't forget to add all objects to the OBJ variable
OBJ += $(SPX_OBJ)

# Include generated dependencies
-include $(filter %.d,$(OBJ:.o=.d))

else
#######################################
# Out-of-tree build mechanism.        #
# You shouldn't touch anything below. #
#######################################
.SUFFIXES:

OBJDIR := build

.PHONY: $(OBJDIR)
$(OBJDIR): %:
	+@[ -d $@ ] || mkdir -p $@
	+@$(MAKE) --no-print-directory -r -I$(CURDIR) -C $@ -f $(CURDIR)/Makefile SRCDIR=$(CURDIR) $(MAKECMDGOALS)

Makefile : ;
%.mk :: ;
% :: $(OBJDIR) ;

.PHONY: clean
clean:
	rm -rf $(OBJDIR)

endif
