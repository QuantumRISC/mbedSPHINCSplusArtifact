ifndef _TARGETS
_TARGETS :=

TARGET ?=

ifeq ($(TARGET),unix)
PLATFORM := unix
DEVICE := x86_64
CFLAGS += -DTARGET_UNIX=1
endif

ifeq ($(TARGET),sim)
PLATFORM := opencm3
DEVICE := lm3s6965
OPENCM3_TARGETS := lm3s
endif

ifeq ($(TARGET),pqvexriscvsim)
PLATFORM := vexriscv
CFLAGS += -DTARGET_VEXRISCV=1
endif

ifeq ($(TARGET),stm32f4)
PLATFORM := opencm3
DEVICE := stm32f407vg
OPENCM3_TARGETS := stm32/f4
CFLAGS += -DTARGET_STM32F4=1
endif

RETAINED_VARS += TARGET PLATFORM

ifneq ($(TARGET),)
$(info Picking PLATFORM=$(PLATFORM) and DEVICE=$(DEVICE) for target $(TARGET))
endif

endif
