# Installation of Binsec decoder modules

## Getting UNISIM-VP

UNISIM-VP Binsec decoder modules are build on the UNISIM-VP C++ library and shouldn't need anything else fancy.

Basically you will need:
  - GNU C++ compiler
  - GNU C++ standard library
  - GNU bash
  - GNU make
  - GNU autoconf
  - GNU automake

UNISIM-VP is available on frama-c's git, so you just have to clone its base directory (to a directory $USIMDIR of your choice).


    $ git clone git@git.frama-c.com:unisim-vp/unisim.git "$USIMDIR"/unisim

The official branch for binsec stable decoders is armsec, so better stick to it.

    $ cd "$USIMDIR"/unisim
    $ git checkout armsec

## Installing a decoder

Each decoder (currently) is a standalone C++/automake project.  You can obtain a distribution of each project using special distribution scripts.
To setup a decoder project, choose a directory (here suppose $DECODER points to it). And use the distribution script of your choice (here we use dist_armsec.sh).
    
    $ mkdir "$DECODER"
    $ cd "$DECODER"
    $ "$USIMDIR"/unisim/package/dist_armsec.sh "$PWD"
    
All decoders follow a standard configure-make-make-install process.
In the following we perform a no-option and in-place build (suit yourself if you prefer something else).
Moreover, we skip the `make install` which is not mandatory.

    $ ./configure
    $ make

That's it, the decoder is ready.

    $ armsec/unisim-armsec-1.0.9 arm 0x4000 0xe2543210
    
    (address . 0x00004000)
    (opcode . 0xe2543210)
    (size . 4)
    (mnemonic . "subs       r3, r4, #1")
    (0x00004000,0) nxt_r3<32> := (r4<32> - 0x00000001); goto 1
    (0x00004000,1) nxt_n<1> := (nxt_r3<32> <s 0x00000000); goto 2
    (0x00004000,2) nxt_z<1> := (nxt_r3<32> = 0x00000000); goto 3
    (0x00004000,3) nxt_c<1> := (r4<32> >=u 0x00000001); goto 4
    (0x00004000,4) nxt_v<1> := (nxt_n<1> xor (r4<32> <s 0x00000001)); goto 5
    (0x00004000,5) r3<32> := nxt_r3<32>; goto 6
    (0x00004000,6) n<1> := nxt_n<1>; goto 7
    (0x00004000,7) z<1> := nxt_z<1>; goto 8
    (0x00004000,8) c<1> := nxt_c<1>; goto 9
    (0x00004000,9) v<1> := nxt_v<1>; goto 10
    (0x00004000,10) goto (0x00004004,0)

# ARMSEC: the armv7 instruction decoder

ARMSEC is an ARMv7 instruction set decoder. It supports all the instruction set (including NEON, VFP and the variable-length THUMB2 encoding).
The decoder is available through the `dist_armsec.sh` distribution script and the installation steps described above.

usage: `unisim-armsec-1.0.9 arm|thumb <address> <encoding>`

# AARCH64DBA: the armv8 instruction decoder

AARCH64DBA is an ARMv8 (64 bits) instruction set decoder. It supports a significant part of the instruction set.
The decoder is available through the `dist_aarch64dba.sh` distribution script and the installation steps described above.


# AMD64DBA: the x86_64 / X86 instruction decoder

AMD64DBA is an x86_64 (or amd64) 64 bits instruction set decoder.
It can also be used has a x86 32 bits decoder, using the same compatibility mechanisms as those provided by the intel64 architecture (known as legacy x86 mode).
A significant par if the instruction set is supported, and the dba translation is evolving toward decent maturity.
The decoder is available through the `dist_amd64dba.sh` distribution script and the installation steps described above.

usage: `usage: <program> x86|intel64 <address> <encoding>`

The instruction encoding is given as an objdump-like byte sequence.

    $ ./unisim-amd64dba-0.1.0 intel64 0x415b41 "41 ff 14 dc"
    (address . 0x0000000000415b41)
    (opcode . "41 ff 14 dc")
    (mnemonic . "call *(%r12,%rbx,8)")
    (0x0000000000415b41,0) nxt_rsp<64> := (rsp<64> - 0x0000000000000008); goto 1
    (0x0000000000415b41,1) nxt_pc<64> := @[(r12<64> + (rbx<64> lshift 0x0000000000000003)),<-,256]; goto 2
    (0x0000000000415b41,2) @[nxt_rsp<64>,<-,256] := 0x0000000000415b45; goto 3
    (0x0000000000415b41,3) rsp<64> := nxt_rsp<64>; goto 4
    (0x0000000000415b41,4) goto (nxt_pc<64>) // call (0x0000000000415b45,0)

    $ ./unisim-amd64dba-0.1.0 x86 0x415b41 "ff 14 dc"
    (address . 0x00415b41)
    (opcode . "ff 14 dc")
    (mnemonic . "call *(%esp,%ebx,8)")
    (0x00415b41,0) nxt_esp<32> := (esp<32> - 0x00000004); goto 1
    (0x00415b41,1) nxt_pc<32> := @[(esp<32> + (ebx<32> lshift 0x00000003)),<-,16]; goto 2
    (0x00415b41,2) @[nxt_esp<32>,<-,16] := 0x00415b44; goto 3
    (0x00415b41,3) esp<32> := nxt_esp<32>; goto 4
    (0x00415b41,4) goto (nxt_pc<32>) // call (0x00415b44,0)

 

# MIPSELSEC: the mips instrucrtion decoder

MIPSELSEC is a mips (> isa32r2, 32 bits little-endian) instruction set decoder. It partially supports the instruction set.
The decoder is available through the `dist_mipselsec.sh` distribution script and the installation steps described above.

Note: because of DBA's notion of branches and MIPS delay slots, the decoder currently doesn't handle branches correctly.  The decoder is here to serve as feasability demo.




