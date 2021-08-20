###################################
# UNIPY UniSIM-VP python bindings #
# with support for                #
# - ARMv7 (arm32 and thumb)       #
# - PPC (vle)                     #
###################################

import ctypes

emuerr = ctypes.c_int
emu_engine = ctypes.c_void_p

_so = None

def bind( shared_object ):
    if not shared_object.endswith('.so'):
        raise Exception('bad cannot locate fuzr shared object (either define VLE4FUZR_SO or pass an argument)')
    global _so
    _so = ctypes.cdll.LoadLibrary(shared_object)
    if _so is None:
        raise ImportError("ERROR: fail to load the dynamic library.")

    # Specification of lib functions interfaces
    _so.emu_open_arm       .argtypes = (ctypes.POINTER(emu_engine),ctypes.c_uint)
    _so.emu_open_vle       .argtypes = (ctypes.POINTER(emu_engine),)
    _so.emu_start          .argtypes = (emu_engine, ctypes.c_uint64, ctypes.c_uint64, ctypes.c_uint64)
    _so.emu_stop           .argtypes = (emu_engine,)
    _so.emu_close          .argtypes = (emu_engine,)
    _so.emu_reg_read       .argtypes = (emu_engine, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t, ctypes.c_int, ctypes.POINTER(ctypes.c_uint64))
    _so.emu_reg_write      .argtypes = (emu_engine, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t, ctypes.c_int, ctypes.c_uint64)
    _so.emu_mem_map        .argtypes = (emu_engine, ctypes.c_uint64, ctypes.c_uint64, ctypes.c_uint, ctypes.c_void_p, ctypes.c_void_p, ctypes.c_void_p)
    _so.emu_mem_unmap      .argtypes = (emu_engine, ctypes.c_uint64)
    _so.emu_mem_write      .argtypes = (emu_engine, ctypes.c_uint64, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t)
    _so.emu_mem_read       .argtypes = (emu_engine, ctypes.c_uint64, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t)
    _so.emu_mem_store      .argtypes = (emu_engine, ctypes.c_uint64, ctypes.c_uint, ctypes.c_uint64)
    _so.emu_mem_load       .argtypes = (emu_engine, ctypes.c_uint64, ctypes.c_uint, ctypes.POINTER(ctypes.c_uint64))
    _so.emu_mem_fetch      .argtypes = (emu_engine, ctypes.c_uint64, ctypes.c_uint, ctypes.POINTER(ctypes.c_uint64))
    _so.emu_mem_chprot     .argtypes = (emu_engine, ctypes.c_uint64, ctypes.c_uint)
    _so.emu_mem_chhook     .argtypes = (emu_engine, ctypes.c_uint64, ctypes.c_uint, ctypes.c_void_p)
    _so.emu_mem_exc_chhook .argtypes = (emu_engine, ctypes.c_uint, ctypes.c_void_p)
    _so.emu_set_disasm     .argtypes = (emu_engine, ctypes.c_int)
    _so.emu_hook_add       .argtypes = (emu_engine, ctypes.c_int, ctypes.c_void_p, ctypes.c_uint64, ctypes.c_uint64)
    _so.emu_page_info      .argtypes = (emu_engine, ctypes.c_void_p, ctypes.c_uint64)
    _so.emu_pages_info     .argtypes = (emu_engine, ctypes.c_void_p)
    _so.emu_get_error      .argtypes = (emu_engine,)
    _so.emu_get_error      .restype = ctypes.c_char_p
    _so.emu_get_asm        .argtypes = (emu_engine,)
    _so.emu_get_asm        .restype = ctypes.c_char_p

EMU_ERR_OK = 0

class EmuError(Exception):
    def __init__(self, errcode):
        self.errcode = errcode

    def __str__(self):
        return 'EmuError(%r)' % self.errcode

def _EmuCheck(ctx, status):
    if status == EMU_ERR_OK:
        return
    err = _so.emu_get_error(ctx)
    raise EmuError( err.decode('ascii') )

_EMU_HOOK_POOL = {}

def _get_managed_c_hook( signature, function ):
    global _EMU_HOOK_POOL
    key = (signature, function)
    hook = _EMU_HOOK_POOL.get(key)
    if hook is None:
        if function is not None:
            function = signature(function)
        hook = ctypes.cast(function, signature)
        _EMU_HOOK_POOL[key] = hook
    return hook

# Public APIs (EMU_*) to interact with the emulator library
#
# In all the following APIs, ctx is the instance of the emulator as
# returned by EMU_open_${ARCH} (See architecture specific emulator
# constructor at the end of the file).
#
# Public function starts with EMU_, and everything (function,
# variables, classes...) not starting with EMU_ is internal and should most
# probabaly not be used directly.

def EMU_close(ctx):
    # Close the emulator @ctx and release associated resources.
    status = _so.emu_close(ctx)
    _EmuCheck(ctx, status)

def EMU_reg_read(ctx, reg_id):
    # Read from the @reg_id register (See architecture specific
    # EMU_${ARCH}_REG_* for lists of register identifiers).
    reg = ctypes.c_uint64(0)
    rstr, rnum = reg_id
    status = _so.emu_reg_read(ctx, rstr.encode('ascii'), len(rstr), rnum, ctypes.byref(reg))
    _EmuCheck(ctx, status)
    return reg.value

def EMU_reg_write(ctx, reg_id , value):
    # Write @value to the @reg_id register
    rstr, rnum = reg_id
    status = _so.emu_reg_write(ctx, rstr.encode('ascii'), len(rstr), rnum, value)
    _EmuCheck(ctx, status)

# Hook type for memory hooks: hook( ctx, access_type, address, size, endianness, valref )
# The hook is called with:
# - @ctx:         the emulator
# - @access_type: the access type (0: read, 1: write: 2: fetch) 
# - @address:     the memory access address
# - @size:        the memory access size
# - @endianness:  the memory access endianness as a byte ordering mask
# - @valref:      the memory access value as a reference (from ctypes module, reads/writes through valref.contents.value)
# A return value of any non-zero number will abort the hooked memory access

MEM_HOOK_CBTYPE = ctypes.CFUNCTYPE(ctypes.c_void_p, emu_engine, ctypes.c_uint, ctypes.c_uint64, ctypes.c_uint, ctypes.c_uint, ctypes.POINTER(ctypes.c_uint64))

# INFO:
#  - permissions: bit flags {1=read, 2=write, 4=execute}
#  - pages created with permissions 0 have no storage allocated, and won't accept any later permissions modification (EMU_mem_chprot)
#  - hooks are called whether the corresponding access is allowed or not (by given permissions).
#  - if an access is not allowed and the hook exits, it is called as exception handler (value reference is valid in case of a write).
#  - if a read or a fetch is allowed and the corresponding hooks exists, it is called after the value is read from memory
#  - if a write is allowed and the corresponding hooks exists, it is called before the value is written
#  - hooks have a value reference arguments to change the destination value (source is not affected).

def EMU_mem_init(ctx, address, size, **opts):
    # Create a page of size @size starting at @address with attributes @opts.
    # @opts are optional keyword arguments (perms=..., rhook=..., whook=, xhook=) with:
    #    - permissions 'perms', defaulting to 7 (rwx)
    #    - read write and fetch hooks 'rhook', 'whook' and 'xhook', all defaulting to None.
    #      Hooks should comply with the MEM_HOOK_CBTYPE signature
    perms = opts.get('perms',7)
    hooks = (_get_managed_c_hook(MEM_HOOK_CBTYPE, opts.get(atp + 'hook', None)) for atp in 'rwx')
    status = _so.emu_mem_map(ctx, address, size, perms, *hooks)
    _EmuCheck(ctx, status)

def EMU_mem_update(ctx, address, **opts):
    # Update attributes @opts of page at @address.  New attributes are
    # given using the same format as in EMU_mem_init (keyword
    # arguments).  The only difference is that missing arguments won't
    # be updated (no default value).  Note: to clear a hook,
    # explicitely pass None (e.g. rhook=None).
    perms = opts.get('perms')
    if perms is not None:
        status = _so.emu_mem_chprot(ctx, address, perms)
        _EmuCheck(ctx, status)

    for access_type in range(3):
        hook = 'rwx'[access_type] + 'hook'
        if hook not in opts:
            continue
        status = _so.emu_mem_chhook(ctx, address, access_type, _get_managed_c_hook(MEM_HOOK_CBTYPE, opts[hook]))
        _EmuCheck(ctx, status)

def EMU_mem_exceptions(ctx, **opts):
    # Update global memory exception hooks.  New hooks are given using
    # the same format as in EMU_mem_update (keyword
    # arguments). Exception hooks, are called upon unmapped memory
    # accesses.
    for access_type in range(3):
        hook = 'rwx'[access_type] + 'hook'
        if hook not in opts:
            continue
        status = _so.emu_mem_exc_chhook(ctx, access_type, _get_managed_c_hook(MEM_HOOK_CBTYPE, opts[hook]))
        _EmuCheck(ctx, status)
    
    
def EMU_mem_erase(ctx, address):
    # Erase the page containing the byte at @address
    status = _so.emu_mem_unmap(ctx, address)
    _EmuCheck(ctx, status)

def EMU_mem_write(ctx, address, data, size=None):
    # Write @size bytes of @data to memory starting at @address. If
    # size is not specified or is None, all bytes of @data are copied.
    # Note: data should be a bytearray or equivalent (b'...'; no
    # encoded string)
    if size is None:
        size = len(data)
    status = _so.emu_mem_write(ctx, address, data, size)
    _EmuCheck(ctx, status)

def EMU_mem_read(ctx, address, size):
    # Read @size bytes of data from memory at @address
    data = ctypes.create_string_buffer(size)
    status = _so.emu_mem_read(ctx, address, data, size)
    _EmuCheck(ctx, status)
    return bytearray(data)

def EMU_mem_store(ctx, address, size, value):
    # Write the @size bytes @value to memory at @address (through processor)
    status = _so.emu_mem_store(ctx, address, size, value)
    _EmuCheck(ctx, status)

def EMU_mem_load(ctx, address, size):
    # Read @size bytes from memory at @address (through processor) and return value
    value = ctypes.c_uint64(0)
    status = _so.emu_mem_load(ctx, address, size, ctypes.byref(value))
    _EmuCheck(ctx, status)
    return value.value

def EMU_mem_fetch(ctx, address, size):
    # Read @size bytes from memory at @address (through processor) and return value
    value = ctypes.c_uint64(0)
    status = _so.emu_mem_fetch(ctx, address, size, ctypes.byref(value))
    _EmuCheck(ctx, status)
    return value.value

def EMU_start(ctx, begin, until, count=0):
    # Emulate from @begin, and stop when reaching address @until or after @count instructions
    status = _so.emu_start(ctx, begin, until, count)
    _EmuCheck(ctx, status)

def EMU_stop(ctx):
    # Stop emulation ASAP (callable from inside hook).
    status = _so.emu_stop(ctx)
    _EmuCheck(ctx, status)

def EMU_set_disasm(ctx, disasm):
    # Activate (or deactivate) instruction disassembly according to
    # @disasm
    status = _so.emu_set_disasm(ctx, disasm)
    _EmuCheck(ctx, status)

# Hook type for code hooks: hook( ctx, addr, info ):
# The hook is called with:
# - @ctx:         the emulator
# - @address:     the current instruction address
# - @info:        context-dependent info (0 for BLOCK, instruction size for CODE and interrupt number for INTR)
# No return value

CODE_HOOK_CBTYPE = ctypes.CFUNCTYPE(None, emu_engine, ctypes.c_uint64, ctypes.c_uint)

class _CodeHook:
    def __init__(self, cb, cbargs):
        self.cb, self.cbargs = cb, cbargs

    def action(self, ctx, address, size):
        self.cb( ctx, address, size, **self.cbargs )

def _get_code_hook(cb, cbargs):
    return _get_managed_c_hook(CODE_HOOK_CBTYPE, _CodeHook(cb, cbargs).action)

_EMU_HOOK_INTR = 1
_EMU_HOOK_CODE = 4
_EMU_HOOK_BLOCK = 8

# EMU_hook_*(ctx, callback, begin=1, end=0, **cbargs)
#
# Add a hook @callback active whenever current instruction is in the
# @begin and @end address range.  Additional optional keyword
# arguments are given back to the hook @callback when called.  Hooks
# should comply with the MEM_HOOK_CBTYPE signature.

def EMU_hook_code(ctx, callback, begin=1, end=0, **cbargs):
    # Add a hook @callback before each instruction execution (if conditions are met).
    status = _so.emu_hook_add(ctx, _EMU_HOOK_CODE, _get_code_hook(callback, cbargs), begin, end)
    _EmuCheck(ctx, status)

def EMU_hook_BB(ctx, callback, begin=1, end=0, **cbargs):
    # Add a hook @callback before each basic-block execution (if conditions are met).
    status = _so.emu_hook_add(ctx, _EMU_HOOK_BLOCK, _get_code_hook(callback, cbargs), begin, end)
    _EmuCheck(ctx, status)

def EMU_hook_excpt(ctx, callback, begin=1, end=0, **cbargs):
    # Add a hook @callback before each interrupting instruction (if conditions are met).
    status = _so.emu_hook_add(ctx, _EMU_HOOK_INTR, _get_code_hook(callback, cbargs), begin, end)
    _EmuCheck(ctx, status)

# Hook type for page info hooks: hook( first, last, perms, hooks ):
# The hook is called with:
# - @first:    the page first byte address
# - @last:     the page last byte address
# - @perms:    the page permissions (rwx bitset)
# - @hooks:    the present hooks (rwx bitset)
# No return value

EMU_PAGE_INFO_CBTYPE = ctypes.CFUNCTYPE(None, ctypes.c_uint64, ctypes.c_uint64, ctypes.c_uint, ctypes.c_uint)

def EMU_page_info(ctx, addr, callback):
    # Call the information @callback function with information from
    # page at @addr.  Hook should comply with the EMU_PAGE_INFO_CBTYPE
    # signature.
    cb = ctypes.cast(EMU_PAGE_INFO_CBTYPE(callback), EMU_PAGE_INFO_CBTYPE)
    return _so.emu_page_info(ctx, cb, addr) == 0

def EMU_pages_info(ctx, callback):
    # Call the information @callback function with information from
    # each page (scanning the whole memory from higher to lower
    # addresses).  Hook should comply with the EMU_PAGE_INFO_CBTYPE
    # signature.
    cb = ctypes.cast(EMU_PAGE_INFO_CBTYPE(callback), EMU_PAGE_INFO_CBTYPE)
    status = _so.emu_pages_info(ctx, cb)
    _EmuCheck(ctx, status)

######################
# ARM specific stuff #
######################

def EMU_open_arm(is_thumb=False):
    # Create an arm emulator in arm32 mode when @is_thumb is False or
    # in thumb2 mode when @is_thumb is True.
    ctx = emu_engine()
    status = _so.emu_open_arm(ctypes.byref(ctx), int(is_thumb))
    _EmuCheck(ctx, status)
    return ctx

EMU_ARM_REG_APSR = ('apsr',0)
def EMU_ARM_REG_R(idx):
    return ('gpr',idx)
EMU_ARM_REG_SP = ('gpr',13)
EMU_ARM_REG_LR = ('gpr',14)
EMU_ARM_REG_PC = ('gpr',15)

######################
# VLE specific stuff #
######################

def EMU_open_vle():
    # Create a vle emulator
    ctx = emu_engine()
    status = _so.emu_open_vle(ctypes.byref(ctx))
    _EmuCheck(ctx, status)
    return ctx

def EMU_VLE_REG_R(idx):
    return ('gpr',idx)

EMU_VLE_REG_LR = ('lr',0)

