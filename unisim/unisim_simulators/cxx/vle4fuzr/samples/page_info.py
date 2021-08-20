#!/usr/bin/python
#
#  Copyright (c) 2019,
#  Commissariat a l'Energie Atomique et aux Energies Alternatives (CEA)
#  All rights reserved.
#
# Authors: Yves Lhuillier (yves.lhuillier@cea.fr), Gilles Mouchard <gilles.mouchard@cea.fr>

import os, sys, unipy, random

if len( sys.argv ) > 1:
    unipy.bind(sys.argv[1])
else:
    unipy.bind(os.getenv("VLE4FUZR_SO"))
 
def print_page(beg, end, perms, hooks):
    perms_rwx = ''.join( ' rw x'[perms & bit] for bit in (1,2,4) )
    hooks_rwx = ''.join( ' rw x'[hooks & bit] for bit in (1,2,4) )
    sys.stdout.write( '%08x - %08x perms(%s), hooks(%s)\n' % (beg, end, perms_rwx, hooks_rwx) )
   
# Initialize emulator (in ARM mode, though no code will be executed)
ctx = unipy.EMU_open_arm()

sys.stdout.write('### Single page allocation ###\n')

unipy.EMU_mem_init(ctx, 0x1000, 0x1000)
unipy.EMU_page_info(ctx, 0x1800, print_page)

sys.stdout.write('### Random page allocation ###\n')

PAGECOUNT=24

# map random memory pages
for idx in range(PAGECOUNT):
    base = 0x2000 + 0x1000*random.randrange(PAGECOUNT)
    # Trying to alloc a 4k page
    try:
        unipy.EMU_mem_init(ctx, base, 0x1000, perms=(base>>12)&7)
        sys.stdout.write('Allocated 4k @%08x\n' % base)
    except unipy.EmuError as e:
        pass

sys.stdout.write( '### Memory mapping display ###\n' )
class MemMap:
    def __init__(self):
        self.bases = []
        
    def page(self, beg, end, perms, hook):
        print_page(beg, end, perms, hook)
        self.bases.append(beg)

mm = MemMap()
unipy.EMU_pages_info(ctx, mm.page)

sys.stdout.write( '### Other ###\n' )
sys.stdout.write( 'page count: %r\n' % len(mm.bases) )

sys.stdout.write( '### Cleaning ###\n' )
for base in mm.bases:
    unipy.EMU_mem_erase(ctx, base)

unipy.EMU_close(ctx)
