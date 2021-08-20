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
txt = 0x1000
exitpoint = txt + len(ARM_CODE)
unipy.EMU_mem_init(ctx, txt, 0x1000, perms=4) # program memory (execute only)
unipy.EMU_mem_write(ctx,txt, ARM_CODE)

size = 4
src = 0x2000
dst = 0x3000
unipy.EMU_mem_init(ctx, src, 0x1000, perms=1) # source memory (read only)
unipy.EMU_mem_init(ctx, dst, 0x1000, perms=2) # destination memory (write only)
unipy.EMU_mem_write(ctx, src, (''.join(chr(x+65) for x in range(size))).encode('ascii'))

# the hook that will be used to perform early return at Nth invocation
class ReturnTrigger:
    def __init__(self, count, msg):
        self.count, self.msg = count, msg
        
    def hook(self, *args):
        sys.stdout.write( '  Hit #%d: %r @%r\n' % (self.count, self.msg, args) )
        self.count -= 1
        if self.count > 0: return
        addr = unipy.EMU_reg_read(ctx, unipy.EMU_ARM_REG_LR)
        unipy.EMU_reg_write(ctx, unipy.EMU_ARM_REG_PC, addr)
        

def run():
    # initialize machine registers
    unipy.EMU_reg_write(ctx, unipy.EMU_ARM_REG_R(0), dst)
    unipy.EMU_reg_write(ctx, unipy.EMU_ARM_REG_R(1), src)
    unipy.EMU_reg_write(ctx, unipy.EMU_ARM_REG_R(2), size)
    unipy.EMU_reg_write(ctx, unipy.EMU_ARM_REG_LR, exitpoint)
    unipy.EMU_reg_write(ctx, unipy.EMU_ARM_REG_APSR, 0xFFFFFFFF) #All application flags turned on
    unipy.EMU_mem_write(ctx, dst, (' ' * size).encode('ascii'))
    unipy.EMU_start(ctx, txt, exitpoint)
    sys.stdout.write( 'received: %r\n' % unipy.EMU_mem_read(ctx,dst,size) )
    
# (de)activate disassembly
unipy.EMU_set_disasm(ctx, True)

########################
sys.stdout.write( '\n### 1. xhook ###\n' )
########################

unipy.EMU_mem_update(ctx,txt,xhook=ReturnTrigger(8,"xhook").hook)
run()
unipy.EMU_mem_update(ctx,txt,xhook=None)

########################
sys.stdout.write( '\n### 2. rhook ###\n' )
########################

unipy.EMU_mem_update(ctx,src,rhook=ReturnTrigger(3,"rhook").hook)
run()
unipy.EMU_mem_update(ctx,src,rhook=None)

########################
sys.stdout.write( '\n### 3. whook ###\n' )
########################

unipy.EMU_mem_update(ctx,dst,whook=ReturnTrigger(3,"whook").hook)
run()
unipy.EMU_mem_update(ctx,dst,whook=None)

unipy.EMU_close(ctx)


