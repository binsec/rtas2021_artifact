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

# VLE code to be emulated (greatest common divisor)
VLE_CODE =  b""
VLE_CODE += b"\x7c\x67\xfe\x70" #    0: srawi   r7,r3,31
VLE_CODE += b"\x7c\xe3\x1a\x78" #    4: xor     r3,r7,r3
VLE_CODE += b"\x06\x73"         #    8: se_sub  r3,r7
VLE_CODE += b"\x7c\x87\xfe\x70" #    a: srawi   r7,r4,31
VLE_CODE += b"\x7c\xe4\x22\x78" #    e: xor     r4,r7,r4
VLE_CODE += b"\x06\x74"         #   12: se_sub  r4,r7
VLE_CODE += b"\x01\x37"         #   14: se_mr   r7,r3
VLE_CODE += b"\x0c\x43"         #   16: se_cmp  r3,r4
VLE_CODE += b"\x7a\x00\x00\x08" #   18: e_bge   20 <gcd+0x20>
VLE_CODE += b"\x01\x43"         #   1c: se_mr   r3,r4
VLE_CODE += b"\x01\x74"         #   1e: se_mr   r4,r7
VLE_CODE += b"\x01\x37"         #   20: se_mr   r7,r3
VLE_CODE += b"\x2a\x04"         #   22: se_cmpi r4,0
VLE_CODE += b"\x7a\x12\x00\x10" #   24: e_beq   34 <gcd+0x34>
VLE_CODE += b"\x0c\x47"         #   28: se_cmp  r7,r4
VLE_CODE += b"\x7a\x10\xff\xf2" #   2a: e_blt   1c <gcd+0x1c>
VLE_CODE += b"\x06\x47"         #   2e: se_sub  r7,r4
VLE_CODE += b"\x79\xff\xff\xf8" #   30: e_b     28 <gcd+0x28>
VLE_CODE += b"\x00\x04"         #   34: se_blr

# memory address where emulation starts
entrypoint = 0x1000
exitpoint = entrypoint + len(VLE_CODE)

# callback for tracing basic blocks
def hook_block(ctx, address, size):
    print(">>> Tracing basic block at 0x%x, block size = 0x%x" % (address, size))

# callback for tracing instructions
def hook_code(ctx, address, size):
    print(">>> Tracing instruction at 0x%x, size = %u" % (address, size))

print("Emulate VLE code")
try:
    # Initialize emulator in VLE mode
    ctx = unipy.EMU_open_vle()

    # map 1K memory for this emulation
    unipy.EMU_mem_init(ctx, entrypoint, 1024)

    # write machine code to be emulated to memory
    unipy.EMU_mem_write(ctx,entrypoint, VLE_CODE, len(VLE_CODE))
    unipy.EMU_mem_write(ctx, exitpoint, b"\x18\x00\xd0\x00") # 2c: mov r0, r0

    # initialize machine registers
    unipy.EMU_reg_write(ctx, unipy.EMU_VLE_REG_R(3), 0x1234)
    unipy.EMU_reg_write(ctx, unipy.EMU_VLE_REG_R(4), 0x6789)
    unipy.EMU_reg_write(ctx, unipy.EMU_VLE_REG_LR, exitpoint)

    # # tracing all basic blocks with customized callback
    # unipy.EMU_hook_BB(ctx, hook_block)

    # # tracing one instruction at entrypoint with customized callback
    # unipy.EMU_hook_code(ctx, hook_code, begin=entrypoint, end=entrypoint)

    # (de)activate disassembly
    unipy.EMU_set_disasm(ctx, True)
    
    # emulate machine code in infinite time
    unipy.EMU_start(ctx, entrypoint, exitpoint)

    # now print out some registers
    print(">>> Emulation done. Below is the CPU context")

    r3 = unipy.EMU_reg_read(ctx, unipy.EMU_VLE_REG_R(3))
    r4 = unipy.EMU_reg_read(ctx, unipy.EMU_VLE_REG_R(4))
    print(">>> R3 = 0x%x" %r3)
    print(">>> R4 = 0x%x" %r4)

    unipy.EMU_close(ctx)

except unipy.EmuError as e:
    print("ERROR: %s" % e)



