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

# VLE code to be emulated (simple guessing program with simple serial console)
VLE_CODE  = b""
VLE_CODE += b""                 # 0x000000 <sendnum>:
VLE_CODE += b"\x18\x21\x06\xf0" # 0x000000     e_stwu  r1,-16(r1)
VLE_CODE += b"\x00\x80"         # 0x000004     se_mflr r0
VLE_CODE += b"\xd3\xf1"         # 0x000006     se_stw  r31,12(r1)
VLE_CODE += b"\x70\xf9\xe4\xcc" # 0x000008     e_lis   r7,52428
VLE_CODE += b"\xd5\x01"         # 0x00000c     se_stw  r0,20(r1)
VLE_CODE += b"\x70\xf9\xc4\xcd" # 0x00000e     e_or2i  r7,52429
VLE_CODE += b"\x7c\xe3\x38\x16" # 0x000012     mulhwu  r7,r3,r7
VLE_CODE += b"\x7c\xff\x1c\x70" # 0x000016     e_srwi  r31,r7,3
VLE_CODE += b"\x7c\xe6\x1c\x71" # 0x00001a     e_srwi. r6,r7,3
VLE_CODE += b"\x7f\xe5\x08\x70" # 0x00001e     e_slwi  r5,r31,1
VLE_CODE += b"\x6c\x3f"         # 0x000022     se_slwi r31,3
VLE_CODE += b"\x7f\xe5\xfa\x14" # 0x000024     add     r31,r5,r31
VLE_CODE += b"\x07\x3f"         # 0x000028     se_subf r31,r3
VLE_CODE += b"\x7a\x12\x00\x0a" # 0x00002a     e_beq   180009c <sendnum+0x34>
VLE_CODE += b"\x01\x63"         # 0x00002e     se_mr   r3,r6
VLE_CODE += b"\x79\xff\xff\xd1" # 0x000030     e_bl    1800068 <sendnum>
VLE_CODE += b"\xc5\x01"         # 0x000034     se_lwz  r0,20(r1)
VLE_CODE += b"\x00\x90"         # 0x000036     se_mtlr r0
VLE_CODE += b"\x1b\xff\x80\x30" # 0x000038     e_addi  r31,r31,48
VLE_CODE += b"\x70\xe0\xe0\x0e" # 0x00003c     e_lis   r7,14
VLE_CODE += b"\x00\xcf"         # 0x000040     se_extzb r31
VLE_CODE += b"\xd0\xf7"         # 0x000042     se_stw  r31,0(r7)
VLE_CODE += b"\xc3\xf1"         # 0x000044     se_lwz  r31,12(r1)
VLE_CODE += b"\x20\xf1"         # 0x000046     se_addi r1,16
VLE_CODE += b"\x00\x04"         # 0x000048     se_blr
VLE_CODE += b"\x44\x00"         # 0x00004a     se_nop
VLE_CODE += b""                 # 0x00004c <_start>:
VLE_CODE += b"\x18\x21\x06\xe0" # 0x00004c     e_stwu  r1,-32(r1)
VLE_CODE += b"\x00\x80"         # 0x000050     se_mflr r0
VLE_CODE += b"\x1b\x41\x09\x08" # 0x000052     e_stmw  r26,8(r1)
VLE_CODE += b"\xd9\x01"         # 0x000056     se_stw  r0,36(r1)
VLE_CODE += b"\x73\x80\x03\xe8" # 0x000058     e_li    r28,1000
VLE_CODE += b"\x48\x0d"         # 0x00005c     se_li   r29,0
VLE_CODE += b"\x73\xe0\xe0\x0e" # 0x00005e     e_lis   r31,14
VLE_CODE += b"\x4b\xfa"         # 0x000062     se_li   r26,63
VLE_CODE += b"\x48\xab"         # 0x000064     se_li   r27,10
VLE_CODE += b"\x7f\xdc\xea\x14" # 0x000066     add     r30,r28,r29
VLE_CODE += b"\x68\x1e"         # 0x00006a     se_srwi r30,1
VLE_CODE += b"\x01\xe3"         # 0x00006c     se_mr   r3,r30
VLE_CODE += b"\x79\xff\xff\x93" # 0x00006e     e_bl    1800068 <sendnum>
VLE_CODE += b"\xd0\xaf"         # 0x000072     se_stw  r26,0(r31)
VLE_CODE += b"\xd0\xbf"         # 0x000074     se_stw  r27,0(r31)
VLE_CODE += b"\xc0\x7f"         # 0x000076     se_lwz  r7,0(r31)
VLE_CODE += b"\x70\xc0\xe0\x0e" # 0x000078     e_lis   r6,14
VLE_CODE += b"\x00\xc7"         # 0x00007c     se_extzb r7
VLE_CODE += b"\x22\x97"         # 0x00007e     se_cmpli r7,10
VLE_CODE += b"\x7a\x12\x00\x48" # 0x000080     e_beq   1800130 <_start+0x7c>
VLE_CODE += b"\x70\x07\xa8\x3e" # 0x000084     e_cmpl16i r7,62
VLE_CODE += b"\x7a\x12\x00\x30" # 0x000088     e_beq   1800120 <_start+0x6c>
VLE_CODE += b"\x70\x07\xa8\x3c" # 0x00008c     e_cmpl16i r7,60
VLE_CODE += b"\x7a\x12\x00\x4e" # 0x000090     e_beq   1800146 <_start+0x92>
VLE_CODE += b"\x70\x07\xa8\x3d" # 0x000094     e_cmpl16i r7,61
VLE_CODE += b"\x7a\x02\xff\xde" # 0x000098     e_bne   18000de <_start+0x2a>
VLE_CODE += b"\xc9\x01"         # 0x00009c     se_lwz  r0,36(r1)
VLE_CODE += b"\x00\x90"         # 0x00009e     se_mtlr r0
VLE_CODE += b"\x4e\x27"         # 0x0000a0     se_li   r7,98
VLE_CODE += b"\x1b\x41\x08\x08" # 0x0000a2     e_lmw   r26,8(r1)
VLE_CODE += b"\xd0\x76"         # 0x0000a6     se_stw  r7,0(r6)
VLE_CODE += b"\x4f\x97"         # 0x0000a8     se_li   r7,121
VLE_CODE += b"\xd0\x76"         # 0x0000aa     se_stw  r7,0(r6)
VLE_CODE += b"\x4e\x57"         # 0x0000ac     se_li   r7,101
VLE_CODE += b"\xd0\x76"         # 0x0000ae     se_stw  r7,0(r6)
VLE_CODE += b"\x48\xa7"         # 0x0000b0     se_li   r7,10
VLE_CODE += b"\xd0\x76"         # 0x0000b2     se_stw  r7,0(r6)
VLE_CODE += b"\x21\xf1"         # 0x0000b4     se_addi r1,32
VLE_CODE += b"\x00\x04"         # 0x0000b6     se_blr
VLE_CODE += b"\xc0\x7f"         # 0x0000b8     se_lwz  r7,0(r31)
VLE_CODE += b"\x01\xed"         # 0x0000ba     se_mr   r29,r30
VLE_CODE += b"\x00\xc7"         # 0x0000bc     se_extzb r7
VLE_CODE += b"\x70\xc0\xe0\x0e" # 0x0000be     e_lis   r6,14
VLE_CODE += b"\x22\x97"         # 0x0000c2     se_cmpli r7,10
VLE_CODE += b"\x7a\x02\xff\xc0" # 0x0000c4     e_bne   18000ec <_start+0x38>
VLE_CODE += b"\x7c\xfd\xe0\x50" # 0x0000c8     subf    r7,r29,r28
VLE_CODE += b"\x22\x07"         # 0x0000cc     se_cmpli r7,1
VLE_CODE += b"\x7a\x11\xff\x98" # 0x0000ce     e_bgt   18000ce <_start+0x1a>
VLE_CODE += b"\xc9\x01"         # 0x0000d2     se_lwz  r0,36(r1)
VLE_CODE += b"\x00\x90"         # 0x0000d4     se_mtlr r0
VLE_CODE += b"\x1b\x41\x08\x08" # 0x0000d6     e_lmw   r26,8(r1)
VLE_CODE += b"\x21\xf1"         # 0x0000da     se_addi r1,32
VLE_CODE += b"\x00\x04"         # 0x0000dc     se_blr
VLE_CODE += b"\x01\xec"         # 0x0000de     se_mr   r28,r30
VLE_CODE += b"\x79\xff\xff\x96" # 0x0000e0     e_b     18000de <_start+0x2a>

class Console:
    def __init__(self, base):
        self.base = base
        self.source = ''
        self.sink = ''

    def read(self, ctx, access_type, address, size, endianness, valref):
        assert access_type == 0 and size == 4 and address == self.base and endianness == 3
        if not self.source:
            self.source = sys.stdin.readline()
        c, self.source = self.source[0], self.source[1:]
        valref.contents.value = ord(c)
    
    def write(self, ctx, access_type, address, size, endianness, valref):
        assert access_type == 1 and size == 4 and address == self.base and endianness == 3
        value = valref.contents.value
        self.sink += chr(value)
        if value != 10 and value != 13:
            return
        sys.stdout.write( '%s\n' % self.sink )
        self.sink = ''
        sys.stdout.flush()



class RegMon:
    def __init__(self, reg):
        self.reg, self.value = reg, None

    def check(self, ctx, address, size):
        value = unipy.EMU_reg_read(ctx, unipy.EMU_VLE_REG_R(self.reg))
        if value != self.value:
            print(">>> r%d=%#x\n" % (self.reg,value))
        self.value = value

print("Emulate VLE code")
try:
    # Initialize emulator in VLE mode
    ctx = unipy.EMU_open_vle()

    # map 1K memory for program
    program = 0x1000
    entrypoint = program + 0x4c
    exitpoint = program + len(VLE_CODE)
    unipy.EMU_mem_init(ctx, program, 1024)

    # write machine code to be emulated to memory
    unipy.EMU_mem_write(ctx,   program, VLE_CODE)
    unipy.EMU_mem_write(ctx, exitpoint, b"\x18\x00\xd0\x00") # 2c: mov r0, r0 (nop)

    # map 1k for stack
    stackbase = 0x2000
    stacksize = 1024
    stacktop = stackbase + stacksize - 16 # save area
    unipy.EMU_mem_init(ctx, stackbase, stacksize)

    # map serial console
    console = Console(0xe0000)
    unipy.EMU_mem_init(ctx, 0xe0000, 4, perms=0, rhook=console.read, whook=console.write)

    # tracing one instruction at entrypoint with customized callback
    # unipy.EMU_hook_code(ctx, RegMon(28).check)
    # unipy.EMU_hook_code(ctx, RegMon(29).check)
    # unipy.EMU_hook_code(ctx, RegMon(30).check)

    # initialize machine registers
    unipy.EMU_reg_write(ctx, unipy.EMU_VLE_REG_R(1), stacktop)
    unipy.EMU_reg_write(ctx, unipy.EMU_VLE_REG_LR, exitpoint)

    # (de)activate disassembly
    unipy.EMU_set_disasm(ctx, False)
    
    # emulate machine code in infinite time
    unipy.EMU_start(ctx, entrypoint, exitpoint)
    unipy.EMU_close(ctx)

except unipy.EmuError as e:
    print("ERROR: %s" % e)

