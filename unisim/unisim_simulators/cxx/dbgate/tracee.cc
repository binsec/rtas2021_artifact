/*
 *  Copyright (c) 2019,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#include <tracee.hh>
// /* UNISIM */
// #include <unisim/util/dbgate/dbgate.hh>
#include <unisim/util/loader/elf_loader/elf_loader.hh>
#include <unisim/util/loader/elf_loader/elf32_loader.hh>
#include <unisim/util/loader/elf_loader/elf64_loader.hh>
// #include <unisim/util/debug/elf_symtab/elf_symtab.hh>
// /* C++ */
// #include <iostream>
// #include <string>
// #include <vector>
// #include <set>
// #include <map>
// #include <algorithm>
// #include <sstream>
#include <fstream>
// /* C */
// #include <cassert>
// #include <cstring>
// #include <csignal>
// /* POSIX */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <unistd.h>

namespace
{
  template <typename ADDR>
  struct ClassDriver : public Tracee::Driver
  {
    typedef ADDR addr_t;

    ClassDriver() : Tracee::Driver(), methods() {}

    virtual void Decode(Tracee&, char const* execpath)  override
    {
      typename unisim::util::loader::elf_loader::StdElf<addr_t,addr_t>::Loader loader;
      typedef typename decltype(loader)::Symbol Symbol;
      loader.SetFileName(execpath);
      loader.Load();

      for (DBGateMethodID method; method.next();)
        {
          if (Symbol const* symbol = loader.FindSymbolByName(method.c_str(), Symbol::SYM_FUNC))
            {
              addr_t addr = symbol->GetAddress();
              // addr_t size = symbol->GetSize();
              // std::cerr << method.c_str() << ": <0x" << std::hex << addr << ", " << std::dec << size << '>' << std::endl;
              methods[method.idx()] = addr;
            }
          else
            {
              std::cerr << "Can't find dbgate symbol: " << method.c_str() << std::endl;
              return;
            }
        }
    }
    
    virtual addr_t GetInsnAddr(Tracee const& tracee) const = 0;
    virtual addr_t GetParam(Tracee const& tracee, int pos) const = 0;
    virtual void SetResult(Tracee const& tracee, addr_t res) const = 0;
    virtual void ReturnInt(Tracee const& tracee, int res) const { SetResult(tracee, addr_t(res)); Return(tracee); }
    
    virtual void PrintInsnAddr( Tracee const& tracee, std::ostream& sink ) const override
    {
      sink << "0x" << std::hex << GetInsnAddr(tracee) << std::dec;
    }

    int GetInt(Tracee const& tracee, int pos) const { return int(GetParam(tracee, pos)); }

    std::string GetString( Tracee const& tracee, int adpos ) const
    {
      std::string result;
      addr_t addr = GetParam(tracee, adpos);
      
      for (addr_t word, pos = 0; ; word >>= 8, pos = (pos + 1) % sizeof (addr_t))
        {
          if (pos == 0)
            {
              word = tracee.ptrace(PTRACE_PEEKTEXT, addr, 0);
#if BYTE_ORDER == BIG_ENDIAN
              BSwap(word);
#endif
              addr += 8;
            }
          if (char chr = word) result += chr;
          else                 break;
        }
      return result;
    }
    
    std::string GetBuffer( Tracee const& tracee, int adpos, int szpos ) const
    {
      std::string result;
      addr_t addr = GetParam(tracee, adpos);
      addr_t size = GetParam(tracee, szpos);
      
      for (addr_t word, pos = 0; ; word >>= 8, pos = (pos + 1) % sizeof (addr_t))
        {
          if (0 == size--) break;
          if (pos == 0)
            {
              word = tracee.ptrace(PTRACE_PEEKTEXT, addr, 0);
              addr += 8;
            }
          result += (char)word;
        }
      return result;
    }
    
    addr_t methods[DBGateMethodID::end];
  };

  template <typename ADDR>
  struct AnyX86Driver : ClassDriver<ADDR>
  {
    typedef typename ClassDriver<ADDR>::addr_t addr_t;
    virtual void InsertBreakPoint( Tracee const& tracee, DBGateMethodID method ) override
    {
      uintptr_t method_addr = this->methods[method.idx()];
      addr_t word = tracee.ptrace(PTRACE_PEEKTEXT, method_addr, 0);
      saved_bytes[method.idx()] = word;
      tracee.ptrace(PTRACE_POKETEXT, method_addr, (word & -0x100) | 0xcc);
    }
    virtual bool AtBreakPoint( Tracee const& tracee, DBGateMethodID method ) const override
    {
      return this->GetInsnAddr(tracee) == (this->methods[method.idx()] + 1);
    }
    uint8_t saved_bytes[DBGateMethodID::end];
  };
}

uintptr_t
Tracee::ptrace(enum __ptrace_request request, uintptr_t addr, uintptr_t data) const
{
  return ::ptrace(request, pid, addr, data);
}

Tracee::Tracee( char** argv )
  : pid(-1)
  , driver()
{
  /*** Search for executable ***/
  char const* filename = argv[0];
  bool triedbypath = false;
  struct
  {
    std::string path;
    bool check( char const* _path ) { path = _path; return check(); }
    bool check()
    {
      struct stat stb;
      if (stat(path.c_str(), &stb) == 0 and S_ISREG(stb.st_mode) and (stb.st_mode & 0111))
        return true; // XXX: Check about SETUID/GID ?
      path.clear();
      return false;
    }
  } exec;
    
  if (strchr(filename,'/'))
    {
      exec.check( filename );
      triedbypath = true;
    }
  else
    {
      for (char const* path = getenv("PATH"), *npath; path and *path; path = npath)
        {
          if (char const* colon = strchr(path, ':'))
            {
              exec.path.assign(path, colon);
              npath = colon + 1;
            }
          else
            {
              exec.path.assign(path);
              npath = 0;
            }

          if (not exec.path.size() or exec.path == ".")
            {
              if (triedbypath) continue;
              triedbypath = true;
              exec.path.assign( filename );
            }
          else
            {
              if (exec.path.back() != '/')
                exec.path += '/';
              exec.path += filename;
            }
          if (exec.check())
            break;
        }
    }

  if (not exec.path.size())
    {
      /* Trying debugger-style (as if '.' was in the PATH). */
      if (triedbypath or not exec.check( filename ))
        { std::cerr << "error: " << filename << " not found.\n"; return; }
        
      /* XXX: maybe issue a warning ? */
    }

  /*** Executable format identification ***/

  {
    // Using a elf32 loader while limiting ourself to the common fields of elf headers (32/64)
    typedef typename unisim::util::loader::elf_loader::StdElf<uint32_t,uint32_t>::Loader Loader;
    typedef typename Loader::Elf_Ehdr_type Elf_Ehdr;
    Elf_Ehdr header;


    std::ifstream source( exec.path.c_str() );
    if (source.read( (char*)&header, sizeof header ).fail() or strncmp( (char const*)&header.e_ident[0], ELFMAG, SELFMAG ) != 0)
      { std::cerr << "error: " << exec.path.c_str() << " is not a valid ELF file.\n"; return; }

    if (Loader::NeedEndianSwap(&header))
      Loader::SwapElfHeader(&header);
      
    if ((header.e_ident[EI_VERSION] != 1) or (header.e_version != 1))
      { std::cerr << "error: " << exec.path.c_str() << " ELF version mismatch.\n"; return; }

    if (header.e_type != ET_EXEC)
      { std::cerr << "error: " << exec.path.c_str() << " is not an executable.\n"; return; }

    /* ABI and ELF Class will be check according to the  architecture basis. */

    switch (header.e_machine)
      {
      case EM_386:
        if (header.e_ident[EI_CLASS] != ELFCLASS32)
          { std::cerr << "error: " << exec.path.c_str() << " expected e_ident[EI_CLASS] == ELFCLASS32.\n"; return; }
        struct EM_386_Driver : public AnyX86Driver<uint32_t>
        {
          enum { EAX = 6, EIP = 12, ESP = 15 };
          virtual addr_t GetInsnAddr(Tracee const& tracee) const override { return tracee.ptrace(PTRACE_PEEKUSER, 4*EIP, 0); }
          virtual addr_t GetParam( Tracee const& tracee, int pos ) const override { throw 0; }
          virtual void SetResult(Tracee const& tracee, addr_t res) const override { tracee.ptrace(PTRACE_POKEUSER, 4*EAX, res); }
          virtual void Return(Tracee const& tracee) const override
          {
            addr_t sp = tracee.ptrace(PTRACE_PEEKUSER, ESP, 0);
            addr_t ra = tracee.ptrace(PTRACE_PEEKDATA, sp, 0);
            tracee.ptrace(PTRACE_POKEUSER, EIP, ra);
            tracee.ptrace(PTRACE_POKEUSER, ESP, sp+4);
          }
        };
        driver = new EM_386_Driver();
        break;
          
      case EM_X86_64:
        /*** AMD x86 64 processors and compatibles ***/
        
        if (header.e_ident[EI_CLASS] != ELFCLASS64)
          { std::cerr << "error: " << exec.path.c_str() << " expected e_ident[EI_CLASS] == ELFCLASS64.\n"; return; }
        
        struct EM_X86_64_Driver : public AnyX86Driver<uint64_t>
        {
          enum { RAX = 80, RDX = 96, RSI = 104, RDI = 112, RIP = 128, RSP = 152 }; 
          static uintptr_t arg(int pos) { switch (pos) { case 0: return RDI; case 1: return RSI; case 2: return RDX; } throw 0; }
          virtual addr_t GetInsnAddr(Tracee const& tracee) const override { return tracee.ptrace(PTRACE_PEEKUSER, RIP, 0); }
          virtual addr_t GetParam(Tracee const& tracee, int pos) const override { return tracee.ptrace(PTRACE_PEEKUSER, arg(pos), 0); }
          virtual void SetResult(Tracee const& tracee, addr_t res) const override { tracee.ptrace(PTRACE_POKEUSER, RAX, res); }
          virtual void Return(Tracee const& tracee) const override
          {
            addr_t sp = tracee.ptrace(PTRACE_PEEKUSER, RSP, 0);
            addr_t ra = tracee.ptrace(PTRACE_PEEKDATA, sp, 0);
            tracee.ptrace(PTRACE_POKEUSER, RIP, ra);
            tracee.ptrace(PTRACE_POKEUSER, RSP, sp+8);
          }

        };
        driver = new EM_X86_64_Driver();
        break;
          
      default:
        std::cerr << "error: " << exec.path.c_str() << " unrecognized machine type.\n";
        return;
      }
  }
    
  /*** Locate dbgate methods ***/
  driver->Decode(*this, exec.path.c_str());

  /*** Start tracee ***/
  pid = fork();
    
  if (pid < 0)
    {
      perror("fork");
      return;
    }

  if (pid == 0)
    {
      // We are the tracee
      ::ptrace(PTRACE_TRACEME);
      // kill(getpid(), SIGSTOP);
      execv(exec.path.c_str(),argv);
      perror("execv");
      abort();
    }

  // We are the tracer.
  
  int status;
  // waitpid(pid, &status, 0); // Wait for stopped process
  wait(&status);

  // We should now be stopped at entrypoint. XXX: sanity check ?
  // std::cerr << "entrypoint: " << printinsnaddr() << std::endl;

  for (DBGateMethodID method; method.next();)
    driver->InsertBreakPoint(*this, method);
}

DBGateMethodID
Tracee::nextcall() const
{
  for (;;)
    {
      if (this->ptrace(PTRACE_CONT, 0, 0) < 0)
        {
          perror( "Tracee::nextcall {ptrace}" );
          throw 0;
        }
  
      int status;
      wait(&status);
  
      if (WIFEXITED(status))
        return DBGateMethodID::end;
  
      if (not WIFSTOPPED(status))
        {
          std::cerr << "Unhandled exit status: " << std::hex << std::endl;
          throw 0;
        }

      int stopsig = WSTOPSIG(status);

      if (stopsig == SIGCHLD)
        {
          std::cerr << "warning: process has forked...\n";
          continue;
        }

      if (stopsig == SIGTRAP)
        {
          for (DBGateMethodID method; method.next();)
            if (driver->AtBreakPoint(*this,method))
              return method;
      
          printinsnaddr( std::cerr << "Unknown breakpoint: " ) << std::endl;
          throw 0;
          return DBGateMethodID::end;
        }

      std::cerr << "error: unhandled signal " << stopsig << std::endl;
      throw 0;
    }
  
  return DBGateMethodID::end;
}

std::ostream&
Tracee::printinsnaddr( std::ostream& sink ) const
{
  driver->PrintInsnAddr(*this, sink);
  return sink;
}
