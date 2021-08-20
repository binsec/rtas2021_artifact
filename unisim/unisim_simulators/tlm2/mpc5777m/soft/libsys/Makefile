S32DS         = /opt/NXP/S32DS_Power_v1.2/S32DS
CROSS_TOOLS   = $(S32DS)/../Cross_Tools/powerpc-eabivle-4_9
CROSS_COMPILE = $(CROSS_TOOLS)/bin/powerpc-eabivle-
CC            = $(CROSS_COMPILE)gcc
AR            = $(CROSS_COMPILE)ar
RM            = rm -f
CP            = cp

OPTFLAGS ?= -Og
CFLAGS    = -std=c99 -gdwarf-4 -gstrict-dwarf -g3 $(OPTFLAGS) -Wall -fdebug-prefix-map=$(shell readlink -f $$PWD/../libsys)=libsys

LIBSYS_SRCS    = \
src/core/cache.c \
src/core/mpu.c \
src/drv/intc.c \
src/drv/stm.c \
src/drv/swt.c \
src/drv/pit.c \
src/drv/linflexd.c \
src/drv/dmamux.c \
src/drv/edma.c \
src/drv/dspi.c \
src/drv/siul2.c \
src/drv/ebi.c \
src/drv/pbridge.c \
src/drv/xbar.c \
src/drv/smpu.c \
src/drv/m_can.c \
src/drv/sema4.c \
src/drv/console.c \
src/drv/ramdisk.c \
src/fs/lfs.c \
src/fs/lfs_util.c \
src/sys/sys.c

LIBSYS_ASMS    = \
src/core/startup.S \
src/core/vector.S \
src/core/intc_sw_handlers.S

LIBSYS_LIB     = lib/libsys.a

LIBSYS_COBJS    = $(patsubst %.c,obj/libsys/%.o,$(LIBSYS_SRCS))
LIBSYS_AOBJS    = $(patsubst %.S,obj/libsys/%.o,$(LIBSYS_ASMS))
LIBSYS_OBJS     = $(LIBSYS_COBJS) $(LIBSYS_AOBJS)
LIBSYS_DEPS     = $(patsubst %.o,%.d,$(LIBSYS_OBJS))

OBJS        =
DEPS        =

LIBSYS_COMPILER_ARGS= \
-I"$(PWD)/include" \
-I"$(CROSS_TOOLS)/include" \
-DMPC5777M \
-fmessage-length=0 \
-fsigned-char \
-ffunction-sections \
-fdata-sections \
-fstrict-volatile-bitfields \
-mcpu=e200z4 \
-specs=nosys.specs \
-mbig \
-mvle \
-mregnames \
-mhard-float \
--sysroot="$(CROSS_TOOLS)/powerpc-eabivle/newlib"

LIBSYS_ASSEMBLER_ARGS= \
-I"$(PWD)/include" \
-I"$(CROSS_TOOLS)/include" \
-DMPC5777M \
-DHW_INIT \
-mcpu=e200z4 \
-specs=nosys.specs \
-mbig \
-mvle \
-mregnames \
--sysroot="$(CROSS_TOOLS)/powerpc-eabivle/newlib"

# -DI_CACHE

LIBSYS_ARCHIVER_ARGS= \
rcs

LIBS += $(LIBSYS_LIB)
OBJS += $(LIBSYS_OBJS)
DEPS += $(LIBSYS_DEPS)

.PHONY: all
all: $(LIBS)

$(LIBSYS_LIB): $(LIBSYS_OBJS)
	$(AR) $(LIBSYS_ARCHIVER_ARGS) $@ $^

$(LIBSYS_COBJS): obj/libsys/%.o: %.c
	@mkdir -p `dirname $@`
	$(CC) $(LIBSYS_COMPILER_ARGS) $(CPPFLAGS) $(CFLAGS) -MMD -MP -o $@ -c $<

$(LIBSYS_AOBJS): obj/libsys/%.o: %.S
	@mkdir -p `dirname $@`
	$(CC) $(LIBSYS_ASSEMBLER_ARGS) $(CPPFLAGS) $(CFLAGS) -MMD -MP -o $@ -c $<

.PHONY: clean
clean:
	$(RM) $(LIBS)
	$(RM) $(OBJS)
	$(RM) $(DEPS)

-include $(DEPS)

