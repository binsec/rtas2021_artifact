include config.mk

QEMU_OPTIONS= -d in_asm,int,cpu_reset,pcall,cpu -no-reboot -no-shutdown
QEMU_OPTIONS += -display gtk
QEMU_OPTIONS += -machine q35 # More recent hardware.
# QEMU_OPTIONS += -machine accel=kvm -cpu 'Nehalem' # Better CPU, but no logging anymore
# Support for TSC Deadline. But no log anymore..
#QEMU_OPTIONS += -cpu max -machine pc,kernel_irqchip=on,accel=kvm

LD_FLAGS= -nostdlib -ffreestanding
CFLAGS = -ffreestanding $(OPT_FLAGS) -Wall -Wextra -std=gnu11 -fno-pic -foptimize-sibling-calls # -mregparm=3
CFLAGS += -fno-asynchronous-unwind-tables # Disable generation of eh_frames.
CFLAGS += -fno-stack-protector
CFLAGS += -Wno-attributes
# CFLAGS += -msse2 # Better atomic operations, but does not work by default with Qemu.
# CFLAGS += -fwhole-program		  # Aggressive link-time optimisation.
CFLAGS += -g 			          # Debug annotations.

CFLAGS_NO_CLANG:=$(CFLAGS)
ifeq ($(CC),clang)
  CFLAGS += -no-integrated-as -march=i586
endif

ifeq ($(USE_NIX),true)
  GCC_CMD := nix-shell --arg clang false --run "gcc $(M32) $(CFLAGS_NO_CLANG) -o $@ -c $*"
else
  GCC_CMD := $(CC)
endif

#QEMU_GDB=-s -S

KERNEL_FILES := low_level.c error.c high_level.c terminal.c lib/fprint.c pit_timer.c per_cpu.c # vga.c

CFLAGS += -D$(SCHEDULER)

ifeq ($(DYNAMIC_TASK_CREATION),yes)
  CFLAGS += -DDYNAMIC_TASK_CREATION
else ifneq ($(DYNAMIC_TASK_CREATION),no)
  $(error "DYNAMIC_TASK_CREATION should be yes or no")
endif

ifeq ($(DEBUG_PRINTING),yes)
  CFLAGS += -DDEBUG_PRINTING
else ifneq ($(DEBUG_PRINTING),no)
  $(error "DEBUG_PRINTING should be yes or no")
endif

ifneq ($(INJECT_BUG),no)
  CFLAGS += -D$(INJECT_BUG)
endif


ifeq ($(SCHEDULER),ROUND_ROBIN_SCHEDULING)
	KERNEL_FILES:=$(KERNEL_FILES) round_robin_scheduler.c
else
	KERNEL_FILES:=$(KERNEL_FILES) priority_scheduler.c
endif

M32 ?= -m32

USE_NIX ?= false

all: system_manual.qemu
# all: system_manual.exe system_10tasks.exe system_100tasks.exe # system_1000tasks.exe


################ Singlefile handling ################
# This allows compiling all the kernel as one file, handy for e.g. creduce.
ifeq ($(findstring $(SINGLEFILE),yes no preprocessed),)
$(error SINGLEFILE should be defined to yes,no,or preprocessed)
endif
ifneq ($(SINGLEFILE),no)
ORIG_KERNEL_FILES := $(KERNEL_FILES)
singlefile.c:	$(ORIG_KERNEL_FILES)
	cat $+ > singlefile.c
ifeq ($(SINGLEFILE),preprocessed)
singlefile.i: singlefile.c
	$(CC) -m32 -E $(CFLAGS) singlefile.c > singlefile.i
KERNEL_FILES := singlefile.i
else
KERNEL_FILES := singlefile.c
endif
endif


################ Task definitions ################

# The system that we build is parameterized by the tasks that we execute with.
TASKS_manual = TASK(task0,"task0.bin",10) TASK(task1,"task1.bin",20)

TASKS_ntasks = $(foreach i,$(shell seq $(1)),TASK(task$(i),"task0.bin",$(i)))
TASKS_1tasks := $(call TASKS_ntasks, 1)
TASKS_2tasks := $(call TASKS_ntasks, 2)
TASKS_5tasks := $(call TASKS_ntasks, 5)
TASKS_10tasks := $(call TASKS_ntasks, 10)
TASKS_50tasks := $(call TASKS_ntasks, 50)
TASKS_100tasks := $(call TASKS_ntasks, 100)
TASKS_500tasks := $(call TASKS_ntasks, 500)
TASKS_1000tasks := $(call TASKS_ntasks, 1000)
TASKS_2000tasks := $(call TASKS_ntasks, 2000)
TASKS_5000tasks := $(call TASKS_ntasks, 5000)
TASKS_10000tasks := $(call TASKS_ntasks, 10000)
TASKS_100000tasks := $(call TASKS_ntasks, 100000)

# These could be computed from tasks using a function.
DEPS_manual := task0.bin task1.bin
DEPS_1tasks := task0.bin
DEPS_2tasks := task0.bin
DEPS_5tasks := task0.bin
DEPS_10tasks := task0.bin
DEPS_50tasks := task0.bin
DEPS_100tasks := task0.bin
DEPS_500tasks := task0.bin
DEPS_1000tasks := task0.bin
DEPS_2000tasks := task0.bin
DEPS_5000tasks := task0.bin
DEPS_10000tasks := task0.bin
DEPS_100000tasks := task0.bin

################ User_image and tasks compilation ################

# We use this temporary file to overcome the fact that we cannot pass huge command line arguments.
.PRECIOUS: task_%.def
tasks_%.def:
	$(file > $@,#define TASKS $(or $(TASKS_$*),$(error undefined variable "TASKS_$*")))


.SECONDEXPANSION:
system_desc_%.o: tasks_%.def system_desc.c $$(DEPS_%)
#	$(CC) -c $(M32) $(CFLAGS) '-DTASKS=$(or $(TASKS_$*),$(error undefined variable "TASKS_$*"))' -fno-common system_desc.c -o $@
	$(CC) -c $(M32) $(CFLAGS) -include tasks_$*.def -fno-common system_desc.c -o $@
# Note: we use -fno-common to force allocation of initialized data at the right place.


################ Kernel + system compilation ################

.PRECIOUS: %.exe %.o
.PHONY: %.qemu
%.qemu: %.exe
	qemu-system-i386 $(QEMU_OPTIONS) $(QEMU_GDB) -kernel $< 2>&1 | tee out | tail -n 500
#	qemu-system-i386 $(QEMU_OPTIONS) $(QEMU_GDB) -kernel myos.exe -initrd task.bin 2>&1 | tee out | tail -n 500

.PRECIOUS: system_desc_%.o
system_%.exe: $(KERNEL_FILES) system_desc_%.o copy64.o
	sed -e s/system_desc/system_desc_$*/g kernel.ld.tpl > kernel_$*.ld
	$(CC) $(M32) $(LD_FLAGS) -Wl,-Tkernel_$*.ld -o $@ $(CFLAGS) $^ -lgcc
	if grub-file --is-x86-multiboot $@; then echo multiboot confirmed; else  echo the file is not multiboot; fi
	objdump -M intel -D $@ > system_$*.objdump

copy64.o: copy64.c
	if [ $(USE_NIX) = true ]; then \
	  nix-shell --arg clang false --run "gcc $(M32) $(CFLAGS_NO_CLANG) -o $@ -c $<"; \
	  else gcc $(M32) $(CFLAGS_NO_CLANG) -o $@ -c $<; \
	  fi

#system_certikos.exe: certikos_root_task.bin system_desc_certikos.c

################ Generation of the code of tasks. ################

certikos_root_task.exe: certikos_root_task.c lib/fprint.c user_task.ld
	$(CC) $(M32) $(LD_FLAGS) -Wl,-Tuser_task.ld -Wl,--noinhibit-exec -DTASK_NUMBER=0 -o $@ $(CFLAGS) certikos_root_task.c lib/fprint.c -lgcc

task0.exe: task.c lib/fprint.c user_task.ld
	$(CC) $(M32) $(LD_FLAGS) -Wl,-Tuser_task.ld -Wl,--noinhibit-exec -DTASK_NUMBER=0 -o $@ $(CFLAGS) task.c lib/fprint.c -lgcc
task1.exe: task.c lib/fprint.c user_task.ld
	$(CC) $(M32) $(LD_FLAGS) -Wl,-Tuser_task.ld -Wl,--noinhibit-exec -DTASK_NUMBER=1 -o $@ $(CFLAGS) task.c lib/fprint.c -lgcc
task2.exe: task.c lib/fprint.c user_task.ld
	$(CC) $(M32) $(LD_FLAGS) -Wl,-Tuser_task.ld -Wl,--noinhibit-exec -DTASK_NUMBER=2 -o $@ $(CFLAGS) task.c lib/fprint.c -lgcc

%.bin: %.exe
	objcopy -Obinary -j.all $*.exe $*.bin


################ Generic ################

.PHONY: clean
clean:
	rm -f *.exe *.bin *.o singlefile.c singlefile.i kernel*.ld *.objdump

# Note: xorriso and mtools should be installed for grub-mkrescure to work.
# myos.iso:
# 	TMP=`mktemp -d`; mkdir -p $$TMP/boot/grub && cp grub.cfg $$TMP/boot/grub/grub.cfg && cp myos.bin $$TMP/boot/myos.bin && grub-mkrescue --verbose -o myos.iso $$TMP && rm -R $$TMP
# 	bochs -q 'boot:cdrom' 'ata0-slave: type=cdrom, path="myos.iso", status=inserted'
#	qemu-system-i386 -cdrom myos.iso

#	cp grub-rescue-floppy.img a.img
#	mdir -i a.img ::/
# disk images: we have some with grub-rescue-pc.
#	bochs -q 'boot:a' 'floppya: 1_44=a.img, status=inserted'
#  ata0-master: type=disk, path="c.img", mode=flat
#	qemu-system-i386 -d int -kernel myos.bin
#	bochs -q 'boot:c' 'ata0-master: type=disk, path="c.img", mode=flat'
#	bochs -q 'boot:c' 'ata0-master: type=disk, path="CorePlus.img", mode=flat'
