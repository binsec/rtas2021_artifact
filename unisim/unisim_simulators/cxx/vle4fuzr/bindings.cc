/*
 *  Copyright (c) 2019,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 * FUZR RENAULT CEA FILE
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr), Gilles Mouchard <gilles.mouchard@cea.fr>
 */

#include "emu.hh"
#include "arm.hh"
#include "vle.hh"
#include <unisim/kernel/kernel.hh>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <cassert>
#include <cstdint>

char const* emptyargv[] = {"",0};
static unisim::kernel::Simulator simulator(1, (char**)emptyargv);

extern "C"
{
  int emu_open_arm(void** ucengine, unsigned is_thumb)
  {
    static int instance = 0;
    std::ostringstream name;
    name << "cpu_arm" << instance++;
    
    *ucengine = new ArmProcessor( name.str().c_str(), is_thumb );
    return 0;
  }

  int emu_open_vle(void** ucengine)
  {
    *ucengine = new vle::concrete::Processor();
    return 0;
  }

  int emu_close(void* uc)
  {
    Processor* proc = (Processor*)uc;
    delete proc;
    return 0;
  }

  int emu_set_disasm(void* uc, int disasm)
  {
    Processor& proc = *(Processor*)uc;
    proc.set_disasm(disasm);
    return 0;
  }

  int emu_mem_map(void* uc, uint64_t addr, uint64_t size, unsigned perms, void* _rhook, void* _whook, void* _xhook)
  {
    Processor& proc = *(Processor*)uc;
    Processor::Page::hook_t
      rhook = (Processor::Page::hook_t)_rhook,
      whook = (Processor::Page::hook_t)_whook,
      xhook = (Processor::Page::hook_t)_xhook;
    return proc.mem_map(addr, size, perms, rhook, whook, xhook) ? 0 : -1;
  }

  int emu_mem_unmap(void* uc, uint64_t addr)
  {
    Processor& proc = *(Processor*)uc;
    return proc.mem_unmap(addr) ? 0 : -1;
  }

  int emu_mem_write(void* uc, uint64_t addr, uint8_t const* bytes, uintptr_t size)
  {
    Processor& proc = *(Processor*)uc;
    return proc.mem_write(addr,bytes,size) ? 0 : -1;
  }
  
  int emu_mem_read(void* uc, uint64_t addr, uint8_t* bytes, uintptr_t size)
  {
    Processor& proc = *(Processor*)uc;
    return proc.mem_read(addr,bytes,size) ? 0 : -1;
  }
  
  int emu_mem_chprot(void* uc, uint64_t addr, unsigned new_perms)
  {
    Processor& proc = *(Processor*)uc;
    return proc.mem_chprot(addr, new_perms) ? 0 : -1;
  }
  
  int emu_mem_chhook(void* uc, uint64_t addr, unsigned access_type, void* hook)
  {
    Processor& proc = *(Processor*)uc;
    return proc.mem_chhook(addr, access_type, (Processor::Page::hook_t)hook) ? 0 : -1;
  }
  
  int emu_mem_exc_chhook(void* uc, unsigned access_type, void* hook)
  {
    Processor& proc = *(Processor*)uc;
    return proc.mem_exc_chhook(access_type, (Processor::Page::hook_t)hook) ? 0 : -1;
  }
  
  int emu_reg_write(void* uc, char const* id, uintptr_t size, int regid, uint64_t value)
  {
    Processor& proc = *(Processor*)uc;
    return proc.reg_write(id, size, regid, value) ? 0 : -1;
  }

  int emu_reg_read(void* uc, char const* id, uintptr_t size, int regid, uint64_t* value)
  {
    Processor& proc = *(Processor*)uc;
    return proc.reg_read(id, size, regid, value) ? 0 : -1;
  }

  int emu_hook_add(void* uc, int types, void* callback, uint64_t begin, uint64_t end)
  {
    Processor& proc = *(Processor*)uc;
    return proc.add_hook(types, callback, begin, end) ? 0 : -1;
  }
  
  int emu_page_info(void* uc, Processor::Page::info_t page_info, uint64_t addr)
  {
    Processor& proc = *(Processor*)uc;
    return proc.page_info(page_info, addr) ? 0 : -1;
  }
  
  int emu_pages_info(void* uc, Processor::Page::info_t page_info)
  {
    Processor& proc = *(Processor*)uc;
    return proc.pages_info(page_info) ? 0 : -1;
  }

  int emu_start(void* uc, uint64_t begin, uint64_t until, uint64_t count)
  {
    Processor& proc = *(Processor*)uc;
    proc.bblock = true;
    
    try { proc.run(begin, until, count); }
    
    catch (Processor::Abort) { return -1; }
      
    return 0;
  }

  int emu_stop(void* uc)
  {
    Processor& proc = *(Processor*)uc;
    proc.abort("UserStop()");
    return 0;
  }

  char const* emu_get_asm(void* uc)
  {
    Processor& proc = *(Processor*)uc;
    return proc.get_asm();
  }
  
  char const* emu_get_error(void* uc)
  {
    Processor& proc = *(Processor*)uc;
    return proc.error.c_str();
  }

  int emu_mem_store(void* uc, uint64_t addr, unsigned size, uint64_t value)
  {
    Processor& proc = *(Processor*)uc;
    
    try { proc.PhysicalWriteMemory( addr, size, proc.endian_mask(size), value ); }
    
    catch (Processor::Abort) { return -1; }
      
    return 0;
  }

  int emu_mem_load(void* uc, uint64_t addr, unsigned size, uint64_t* value)
  {
    Processor& proc = *(Processor*)uc;
    
    try { proc.PhysicalReadMemory( addr, size, proc.endian_mask(size), value ); }
    
    catch (Processor::Abort) { return -1; }
      
    return 0;
  }
  
  int emu_mem_fetch(void* uc, uint64_t addr, unsigned size, uint64_t* value)
  {
    Processor& proc = *(Processor*)uc;
    
    try { proc.PhysicalFetchMemory( addr, size, proc.endian_mask(size), value ); }
    
    catch (Processor::Abort) { return -1; }
      
    return 0;
  }
  
}
