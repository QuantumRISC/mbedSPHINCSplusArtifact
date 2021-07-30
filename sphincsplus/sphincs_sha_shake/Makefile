SHELL = sh -xv

ifdef SRCDIR

VPATH = $(SRCDIR)

# Add your targets here
TARGETS = spx_streaming.hex

all: $(TARGETS)

# Only used for unix builds
USE_ASAN ?= TRUE
ENABLE_GRPOF ?= FALSE

PRINT_UNUSED_FUNCTIONS = FALSE
ifeq ($(PRINT_UNUSED_FUNCTIONS),TRUE)
    # NOTE: best use -O0 (no inline)
	LDFLAGS += -Wl,--print-gc-sections
endif

LDFLAGS += -Wl,-Map,mem.map

# either one of them
DO_STACK_MEASUREMENTS ?= TRUE
DO_SPEED_MEASUREMENTS ?= FALSE
DO_CODESIZE_BUILD ?= FALSE

# Only compile this variant (if `DO_SINGLE_VARIANT` evaluates to `TRUE`)
# Choose from:
#   SHA2_ROBUST
#   SHA2_SIMPLE
#   SHAKE_ROBUST
#   SHAKE_SIMPLE
CHOSEN_SINGLE_VARIANT ?= SHA2_SIMPLE

# Optimization level
CFLAGS += -O2

# If we measure stack or codesize, only build one variant (`DO_SINGLE_VARIANT = TRUE`)
DO_SINGLE_VARIANT = FALSE
ifeq ($(DO_STACK_MEASUREMENTS),TRUE)
	DO_SINGLE_VARIANT = TRUE
	CFLAGS += -DDO_STACK_MEASUREMENTS
endif
ifeq ($(DO_CODESIZE_BUILD),TRUE)
	DO_SINGLE_VARIANT = TRUE
endif
ifeq ($(DO_SPEED_MEASUREMENTS),TRUE)
	CFLAGS += -DDO_SPEED_MEASUREMENTS
endif
ifeq ($(TARGET),unix)
	# only relevant for STM32F4 build
	DO_SINGLE_VARIANT = FALSE
endif

ifeq ($(DO_SINGLE_VARIANT),TRUE)
	ifeq ($(CHOSEN_SINGLE_VARIANT),SHA2_ROBUST)
		CFLAGS += -DVARIANT_SHA2_ROBUST
	endif
	ifeq ($(CHOSEN_SINGLE_VARIANT),SHA2_SIMPLE)
		CFLAGS += -DVARIANT_SHA2_SIMPLE
	endif
	ifeq ($(CHOSEN_SINGLE_VARIANT),SHAKE_ROBUST)
		CFLAGS += -DVARIANT_SHAKE_ROBUST
	endif
	ifeq ($(CHOSEN_SINGLE_VARIANT),SHAKE_SIMPLE)
		CFLAGS += -DVARIANT_SHAKE_SIMPLE
	endif
else
    # Build all variants
	CFLAGS += -DVARIANT_SHA2_ROBUST
	CFLAGS += -DVARIANT_SHA2_SIMPLE
	CFLAGS += -DVARIANT_SHAKE_SIMPLE
	CFLAGS += -DVARIANT_SHAKE_ROBUST
endif

ifeq ($(TARGET),unix)
	ifeq ($(ENABLE_GRPOF),TRUE)
		CFLAGS += -pg -DGPROF
		LDFLAGS += -pg
	endif
	ifeq ($(USE_ASAN),TRUE)
		CFLAGS += -fsanitize=address
		LDFLAGS += -fsanitize=address
	endif
endif


include config.mk

# For each target define a TARGETNAME_SRC, TARGETNAME_OBJ and define any
# additional dependencies for your the target TARGETNAME.elf file (just
# define the dependencies, a generic rule for .elf target exists in
# config.mk).


SOURCES = params.c address.c rng.c wots.c utils.c fors.c sign.c hash.c hash_sha256.c thash.c thash_sha256_robust.c thash_sha256_simple.c hash_shake256.c thash_shake256_robust.c thash_shake256_simple.c wotsx1.c merkle.c utilsx1.c
HEADERS = params.h address.h rng.h wots.h utils.h fors.h api.h  hash.h thash.h


SOURCES += fips202.c
HEADERS += fips202.h

SOURCES += sha256.c
HEADERS += sha256.h

DET_SOURCES = $(SOURCES:rng.%=rng.%)
DET_HEADERS = $(HEADERS:rng.%=rng.%)

SPX_STREAMING_SRC = ${SOURCES} $(TARGET).c
SPX_STREAMING_OBJ = $(call objs,$(SPX_STREAMING_SRC))
spx_streaming.elf: $(SPX_STREAMING_OBJ) libhal.a

# Don't forget to add all objects to the OBJ variable
OBJ += $(SPX_STREAMING_OBJ)

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
	mkdir -p out_files
	+@[ -d $@ ] || mkdir -p $@
	+@$(MAKE) --no-print-directory -r -I$(CURDIR) -C $@ -f $(CURDIR)/Makefile SRCDIR=$(CURDIR) $(MAKECMDGOALS)

Makefile : ;
%.mk :: ;
% :: $(OBJDIR) ;

.PHONY: clean
clean:
	rm -rf $(OBJDIR)
	rm -rf out_files

endif
