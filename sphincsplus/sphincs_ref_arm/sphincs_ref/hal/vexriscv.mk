ifndef _HAL
_HAL :=

RISCV_NAME=riscv64-unknown-elf
CC=$(RISCV_NAME)-gcc
AR := $(RISCV_NAME)-ar
LD := $(CC)
OBJCOPY=$(RISCV_NAME)-objcopy
CPP := $(RISCV_NAME)-cpp
SIZE := $(RISCV_NAME)-size
LIBDEPS += libhal.a
LDLIBS += -lhal

###############
# HAL library #
# ###############
LIBHAL_SRC := \
	hal/hal-vexriscv.c \
	hal/hal-vexriscv-start.S

LIBHAL_OBJ := $(call objs,$(LIBHAL_SRC))
libhal.a: $(LIBHAL_OBJ)
libhal.a: CFLAGS+=

MABI := ilp32
MARCH := rv32im
CFLAGS += -march=$(MARCH) -mabi=$(MABI)
LDFLAGS +=  -march=$(MARCH) -mabi=$(MABI) 
LDSCRIPT = ../hal/pqvexriscvsim.ld
LDFLAGS += -fstrict-volatile-bitfields -fno-strict-aliasing
LDFLAGS += -lnosys -specs=nano.specs -mcmodel=medany -nostartfiles -ffreestanding -Wl,-Bstatic,-T,$(LDSCRIPT),--print-memory-usage

LDFLAGS += \
	-L.

LINKDEPS += $(LIBDEPS)

OBJ += $(LIBHAL_OBJ)

endif
