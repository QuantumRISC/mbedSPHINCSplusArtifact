ifndef _HAL
_HAL :=

###############
# HAL library #
###############

LIBHAL_SRC := \
	hal/hal-unix.c

LIBHAL_OBJ := $(call objs,$(LIBHAL_SRC))
libhal.a: $(LIBHAL_OBJ)

LIBDEPS += libhal.a
LDLIBS += -lhal

CC := cc
CPP := cpp
AR := ar
LD := $(CC)
OBJCOPY := objcopy
SIZE := size

CFLAGS +=

LDFLAGS += \
	-L.

LINKDEPS += $(LIBDEPS)

OBJ += $(LIBHAL_OBJ)

endif
