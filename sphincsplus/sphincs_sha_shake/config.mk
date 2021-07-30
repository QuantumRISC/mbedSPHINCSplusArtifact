ifndef _CONFIG
_CONFIG :=

##############################
# Include retained variables #
##############################

RETAINED_VARS :=

-include .config.mk

.config.mk:
	@echo "# These variables are retained and can't be changed without a clean" > $@
	@$(foreach var,$(RETAINED_VARS),echo "$(var) := $($(var))" >> $@; echo "LAST_$(var) := $($(var))" >> $@;)

###############
# Some Macros #
###############
objs = $(addsuffix .o,$(1))

##############################
# Hardware Abstraction Layer #
##############################

-include targets.mk

ifeq (,$(wildcard $(SRCDIR)/hal/$(PLATFORM).mk))
$(error Unknown platform!)
endif

include hal/$(PLATFORM).mk

RETAINED_VARS += PLATFORM

Q ?= @

HOST_CC := cc
HOST_AR := ar
HOST_LD := $(HOST_CC)

SYSROOT ?= $(shell $(CC) --print-sysroot)

CFLAGS += \
	--sysroot=$(SYSROOT)

################
# Optimization #
################

DEBUG ?=
OPT_SIZE ?=
LTO ?=

RETAINED_VARS += DEBUG OPT_SIZE LTO

#ifeq ($(DEBUG),1)
#CFLAGS += -O0 -g3
#else ifeq ($(OPT_SIZE),1)
#CFLAGS += -Os -g3
#else
#CFLAGS += -O3 -g3
#endif

ifeq ($(LTO),1)
CFLAGS += -flto
LDFLAGS += -flto
endif

################
# Common Flags #
################

CPPFLAGS += \
	-I$(SRCDIR)/hal

CFLAGS += \
	-Wall -Wextra -Wshadow \
	-MMD \
	-fno-common \
	-ffunction-sections \
	-fdata-sections \
	$(CPPFLAGS)

LDFLAGS += \
	-Wl,--gc-sections
HOST_CPPFLAGS +=

HOST_CFLAGS += \
	-Wall -Wextra -Wshadow \
	-MMD \
	$(HOST_CPPFLAGS)

################
# Common rules #
################

ELFNAME = $*

_ELFNAME_%.o:
	@echo "  GEN     $@"
	$(Q)echo "const char _elf_name[] = \"$(ELFNAME)\";" | \
		$(CC) -x c -c -o $@ $(filter-out -g3,$(CFLAGS)) -

%.elf: $(LINKDEPS) _ELFNAME_%.elf.o
	@echo "  LD      $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(LD) $(LDFLAGS) -o $@ $(filter %.o,$^) $(LDLIBS)

%.a:
	@echo "  AR      $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(AR) rcs $@ $(filter %.o,$^)

%.bin: %.elf
	@echo "  OBJCOPY $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(OBJCOPY) -Obinary $^ $@

%.hex: %.elf
	@echo "  OBJCOPY $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(OBJCOPY) -Oihex $^ $@

%.c.o: %.c
	@echo "  CC      $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(CC) -c -o $@ $(CFLAGS) $<

%.c.S: %.c
	@echo "  CC      $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(CC) -S -o $@ $(CFLAGS) $<

%.c.i: %.c
	@echo "  CC      $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(CC) -E -o $@ $(CFLAGS) $<

%.S.o: %.S
	@echo "  AS      $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(CC) -c -o $@ $(CFLAGS) $<

##############
# Host Rules #
##############

host/%.c.o: %.c
	@echo "  HOST_CC $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(HOST_CC) -c -o $@ $(HOST_CFLAGS) $<

define host_link =
	@echo "  HOST_LD $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(HOST_LD) $(HOST_LDFLAGS) -o $@ $(filter host/%.o,$^) $(HOST_LDLIBS)
endef

.SECONDARY:

define VAR_CHECK =
ifneq ($$(origin LAST_$(1)),undefined)
ifneq "$$($(1))" "$$(LAST_$(1))"
$$(error "You changed the $(1) variable, you must run make clean!")
endif
endif
endef

$(foreach VAR,$(RETAINED_VARS),$(eval $(call VAR_CHECK,$(VAR))))

endif
