ifndef _HAL
_HAL :=

export DEVICE ?=

ifeq ($(DEVICE),)
$(error No DEVICE specified for linker script generator)
endif

RETAINED_VARS += DEVICE


###############
# HAL library #
###############

LIBHAL_SRC := \
	hal/hal-opencm3.c

LIBHAL_OBJ := $(call objs,$(LIBHAL_SRC))
libhal.a: $(LIBHAL_OBJ)

LIBDEPS += libhal.a
LDLIBS += -lhal

######################
# libopencm3 Library #
######################

export OPENCM3_DIR := $(CURDIR)/libopencm3-master

libopencm3-stamp: hal/libopencm3-master.tar.gz
	$(Q)tar xf $<
	@echo "  BUILD   libopencm3"
	$(Q)$(MAKE) -C $(OPENCM3_DIR) TARGETS=$(OPENCM3_TARGETS)
	$(Q)touch $@

$(OPENCM3_DIR)/mk/genlink-config.mk $(OPENCM3_DIR)/mk/genlink-rules.mk: libopencm3-stamp

include $(OPENCM3_DIR)/mk/genlink-config.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk

CROSS_PREFIX ?= arm-none-eabi
CC := $(CROSS_PREFIX)-gcc
CPP := $(CROSS_PREFIX)-cpp
AR := $(CROSS_PREFIX)-ar
LD := $(CC)
OBJCOPY := $(CROSS_PREFIX)-objcopy
SIZE := $(CROSS_PREFIX)-size

CFLAGS += \
	$(ARCH_FLAGS) \
	--specs=nano.specs \
	--specs=nosys.specs

LDFLAGS += \
	-Wl,--wrap=_read \
	-Wl,--wrap=_write \
	-L. \
	--specs=nano.specs \
	--specs=nosys.specs \
	-nostartfiles \
	-ffreestanding \
	-T$(LDSCRIPT) \
	$(ARCH_FLAGS)

LINKDEPS += $(LDSCRIPT) $(LIBDEPS)

OBJ += $(LIBHAL_OBJ)

endif
