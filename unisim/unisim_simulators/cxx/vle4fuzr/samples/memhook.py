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

# A simple *memcpy* arm32 code
ARM_CODE  = b""
ARM_CODE += b"\x00\x30\xa0\xe3" #    0: mov     r3, #0
ARM_CODE += b"\x03\xc0\xd1\xe7" #    4: ldrb    ip, [r1, r3]
ARM_CODE += b"\x03\xc0\xc0\xe7" #    8: strb    ip, [r0, r3]
ARM_CODE += b"\x01\x30\x83\xe2" #    c: add     r3, r3, #1
ARM_CODE += b"\x02\x00\x53\xe1" #   10: cmp     r3, r2
ARM_CODE += b"\xfa\xff\xff\x1a" #   14: bne     4 <copy+0x4>
ARM_CODE += b"\x1e\xff\x2f\xe1" #   18: bx      lr

ctx = unipy.EMU_open_arm()
entrypoint = 0x1000
exitpoint = entrypoint + len(ARM_CODE)
unipy.EMU_mem_init(ctx, entrypoint, 0x1000, perms=4) # program memory (execute only)
unipy.EMU_mem_write(ctx,entrypoint, ARM_CODE)

size = 4
src = 0x2000
dst = 0x3000
unipy.EMU_mem_init(ctx, src, 0x1000, perms=1) # source memory (read only)
unipy.EMU_mem_init(ctx, dst, 0x1000, perms=2) # destination memory (write only)
unipy.EMU_mem_write(ctx,src, (''.join(chr(x+65) for x in range(size))).encode('ascii'))

# initialize machine registers
unipy.EMU_reg_write(ctx, unipy.EMU_ARM_REG_R(0), dst)
unipy.EMU_reg_write(ctx, unipy.EMU_ARM_REG_R(1), src)
unipy.EMU_reg_write(ctx, unipy.EMU_ARM_REG_R(2), size)
unipy.EMU_reg_write(ctx, unipy.EMU_ARM_REG_LR, exitpoint)
unipy.EMU_reg_write(ctx, unipy.EMU_ARM_REG_APSR, 0xFFFFFFFF) #All application flags turned on

# (de)activate disassembly
unipy.EMU_set_disasm(ctx, True)

# the hook that will be used to monitor memory transaction
def catch_mem(message,ctx,access,address,size,endianness,valref):
    assert size == 1 and endianness == 0
    op = ['read', 'write', 'fetch']
    op = op[access]
    sys.stdout.write( '%s @%08x => %02x (%s)\n' % (op, address, valref.contents.value, message) )

########################
sys.stdout.write( '\n### 1. Simple execution ###\n' )
########################

unipy.EMU_start(ctx, entrypoint, exitpoint)
sys.stdout.write( 'received: %r\n' % unipy.EMU_mem_read(ctx,dst,size) )

########################
sys.stdout.write( '\n### 2. Monitor writes ###\n' )
########################

hook = lambda *args: catch_mem("from step #2", *args)
unipy.EMU_mem_update(ctx,dst,whook=hook)
unipy.EMU_start(ctx, entrypoint, exitpoint)

########################
sys.stdout.write( '\n### 3. Monitor and modify writes ###\n' )
########################

def store_plus_one(ctx,access,address,size,endianness,valref):
    assert size == 1 and endianness == 0 and access == 1
    valref.contents.value = valref.contents.value + 1
unipy.EMU_mem_update(ctx,dst,whook=store_plus_one)
unipy.EMU_start(ctx, entrypoint, exitpoint)
sys.stdout.write( 'received: %r\n' % unipy.EMU_mem_read(ctx,dst,size) )

########################
sys.stdout.write( '\n### 4. Monitor reads and forbid writes ###\n' )
########################

hook = lambda *args: catch_mem("from step #4", *args)
unipy.EMU_mem_update(ctx,src,rhook=hook)
# Warning: the write hook installed in #2 must be removed.  If is not
# removed, it will be used as an exception handler after permission
# error.
unipy.EMU_mem_update(ctx,dst,whook=None,perms=0)
# Note: executing the code will result in runtime exception so we "try-except" the emulation
try:
    unipy.EMU_start(ctx, entrypoint, exitpoint)
except unipy.EmuError as e:
    # Small example of how to quickly retrieve exception parameters
    rwx, addr, msg = eval(e.errcode,{'MemoryException':lambda *args: args})
    sys.stdout.write( '! Catched a MemoryException with %s, %#x, %r' % ("rwx"[rwx], addr, msg) )

########################
sys.stdout.write( '\n### 5. Remove the destination page and setup a global write exception hook ###\n' )
########################

# Note: not removing the read monitor from #4
unipy.EMU_mem_erase(ctx,dst)
unipy.EMU_mem_exceptions(ctx,whook=lambda *args: catch_mem("from step #5", *args))
unipy.EMU_start(ctx, entrypoint, exitpoint)

unipy.EMU_close(ctx)


