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
ARM_CODE   = b"\x37\x00\xa0\xe3\x03\x10\x42\xe0" # mov r0, #0x37; sub r1, r2, r3
# memory address where emulation starts
ADDRESS    = 0x10000

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

    # map 2MB memory for this emulation
    unipy.EMU_mem_init(ctx, ADDRESS, 2 * 1024 * 1024)

    # write machine code to be emulated to memory
    unipy.EMU_mem_write(ctx,ADDRESS, ARM_CODE, len(ARM_CODE))

    # initialize machine registers
    unipy.EMU_reg_write(ctx, unipy.EMU_ARM_REG_R(0), 0x1234)
    unipy.EMU_reg_write(ctx, unipy.EMU_ARM_REG_R(2), 0x6789)
    unipy.EMU_reg_write(ctx, unipy.EMU_ARM_REG_R(3), 0x3333)
    unipy.EMU_reg_write(ctx, unipy.EMU_ARM_REG_APSR, 0xFFFFFFFF) #All application flags turned on

    # tracing all basic blocks with customized callback
    unipy.EMU_hook_BB(ctx, hook_block)

    # tracing one instruction at ADDRESS with customized callback
    unipy.EMU_hook_code(ctx, hook_code, begin=ADDRESS, end=ADDRESS)

    # emulate machine code in infinite time
    unipy.EMU_start(ctx, ADDRESS, ADDRESS + len(ARM_CODE))

    # now print out some registers
    print(">>> Emulation done. Below is the CPU context")

    r0 = unipy.EMU_reg_read(ctx, unipy.EMU_ARM_REG_R(0))
    r1 = unipy.EMU_reg_read(ctx, unipy.EMU_ARM_REG_R(1))
    print(">>> R0 = 0x%x" %r0)
    print(">>> R1 = 0x%x" %r1)

    unipy.EMU_close(ctx)

except unipy.EmuError as e:
    print("ERROR: %s" % e)



