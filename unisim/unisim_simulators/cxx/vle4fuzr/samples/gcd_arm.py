#!/usr/bin/python
#
#  Copyright (c) 2019,
#  Commissariat a l'Energie Atomique et aux Energies Alternatives (CEA)
#  All rights reserved.
#
# Authors: Yves Lhuillier (yves.lhuillier@cea.fr), Gilles Mouchard <gilles.mouchard@cea.fr>

import os, sys, unipy

if len( sys.argv ) > 1:
    unipy.bind(sys.argv[1])
else:
    unipy.bind(os.getenv("VLE4FUZR_SO"))

# code to be emulated
ARM_CODE  = b""

ARM_CODE += b"\x00\x00\x50\xe3" #    0: cmp     r0, #0
ARM_CODE += b"\x00\x00\x60\xb2" #    4: rsblt   r0, r0, #0
ARM_CODE += b"\x00\x00\x51\xe3" #    8: cmp     r1, #0
ARM_CODE += b"\x00\x10\x61\xb2" #    c: rsblt   r1, r1, #0
ARM_CODE += b"\x01\x00\x50\xe1" #   10: cmp     r0, r1
ARM_CODE += b"\x02\x00\x00\xaa" #   14: bge     24 <gcd+0x24>
ARM_CODE += b"\x00\x30\xa0\xe1" #   18: mov     r3, r0
ARM_CODE += b"\x01\x00\xa0\xe1" #   1c: mov     r0, r1
ARM_CODE += b"\x03\x10\xa0\xe1" #   20: mov     r1, r3
ARM_CODE += b"\x00\x00\x51\xe3" #   24: cmp     r1, #0
ARM_CODE += b"\x1e\xff\x2f\x01" #   28: bxeq    lr
ARM_CODE += b"\x00\x30\xa0\xe1" #   2c: mov     r3, r0
ARM_CODE += b"\x01\x00\x53\xe1" #   30: cmp     r3, r1
ARM_CODE += b"\xf8\xff\xff\xba" #   34: blt     1c <gcd+0x1c>
ARM_CODE += b"\x03\x30\x61\xe0" #   38: rsb     r3, r1, r3
ARM_CODE += b"\xfb\xff\xff\xea" #   3c: b       30 <gcd+0x30>

# memory address where emulation starts
entrypoint = 0x1000
exitpoint = entrypoint + len(ARM_CODE)

# callback for tracing basic blocks
def hook_block(ctx, address, size):
    print(">>> Tracing basic block at 0x%x, block size = 0x%x" % (address, size))

# callback for tracing instructions
def hook_code(ctx, address, size):
    print(">>> Tracing instruction at 0x%x, size = %u" % (address, size))

print("Emulate ARM code")
try:
    # Initialize emulator in ARM mode
    ctx = unipy.EMU_open_arm()

    # map 1K memory for this emulation
    unipy.EMU_mem_init(ctx, entrypoint, 1024)

    # write machine code to be emulated to memory
    unipy.EMU_mem_write(ctx,entrypoint, ARM_CODE, len(ARM_CODE))
    unipy.EMU_mem_write(ctx, exitpoint, b"\x00\x00\xa0\xe1") # 2c: mov r0, r0

    # initialize machine registers
    unipy.EMU_reg_write(ctx, unipy.EMU_ARM_REG_R(0), 0x1234)
    unipy.EMU_reg_write(ctx, unipy.EMU_ARM_REG_R(1), 0x6789)
    unipy.EMU_reg_write(ctx, unipy.EMU_ARM_REG_LR, exitpoint)
    unipy.EMU_reg_write(ctx, unipy.EMU_ARM_REG_APSR, 0xFFFFFFFF) #All application flags turned on

    # tracing all basic blocks with customized callback
    unipy.EMU_hook_BB(ctx, hook_block)

    # tracing one instruction at entrypoint with customized callback
    unipy.EMU_hook_code(ctx, hook_code, begin=entrypoint, end=entrypoint)

    # (de)activate disassembly
    unipy.EMU_set_disasm(ctx, False)
    
    # emulate machine code in infinite time
    unipy.EMU_start(ctx, entrypoint, exitpoint)

    # now print out some registers
    print(">>> Emulation done. Below is the CPU context")

    r0 = unipy.EMU_reg_read(ctx, unipy.EMU_ARM_REG_R(0))
    r1 = unipy.EMU_reg_read(ctx, unipy.EMU_ARM_REG_R(1))
    print(">>> R0 = 0x%x" %r0)
    print(">>> R1 = 0x%x" %r1)

    unipy.EMU_close(ctx)

except unipy.EmuError as e:
    print("ERROR: %s" % e)



