/*
 *  Copyright (c) 2018,
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

#include <linuxsystem.hh>
#include <unisim/util/os/linux_os/linux.tcc>
#include <unisim/util/os/linux_os/calls.tcc>
#include <iomanip>
#include <sys/utsname.h>

LinuxOS::LinuxOS( std::ostream& log,
         unisim::service::interfaces::Registers *regs_if,
         unisim::service::interfaces::Memory<uint64_t> *mem_if,
         unisim::service::interfaces::MemoryInjection<uint64_t> *mem_inject_if
         )
  : unisim::service::interfaces::LinuxOS()
  , linux_impl( log, log, log, regs_if, mem_if, mem_inject_if )
  , exited( false )
  , app_ret_status( -1 )
{
  // Set the system type of the target simulator (should be the same than the
  // binary)
  auto amd64_target = new unisim::util::os::linux_os::AMD64TS<unisim::util::os::linux_os::Linux<uint64_t,uint64_t> >( linux_impl );
  linux_impl.SetTargetSystem(amd64_target);
}

void
LinuxOS::Core( std::string const& coredump )
{
  // Loading a core file (memory + register state)
  typedef typename unisim::util::loader::elf_loader::StdElf<uint64_t,uint64_t>::Loader Loader;
  typedef typename Loader::Elf_Ehdr_type Elf_Ehdr;
  typedef typename Loader::Elf_Phdr_type Elf_Phdr;
  
  std::ifstream is(coredump.c_str(), std::ifstream::in | std::ifstream::binary);

  struct hdr { ~hdr() { free(p); } Elf_Ehdr* p; } hdr;
  hdr.p = Loader::ReadElfHeader(is);
  bool need_endian_swap = Loader::NeedEndianSwap(hdr.p);
    
  if (not hdr.p) throw "Could not read ELF header";
  
  struct phdr { ~phdr() { free(p); } Elf_Phdr* p; } phdr_table;
  
  phdr_table.p = Loader::ReadProgramHeaders(hdr.p, is);
  if (not phdr_table.p)
    throw "Can't read program headers";
  
  // Create core blob
  typedef unisim::util::blob::Blob<uint64_t> Blob;
  typedef unisim::util::blob::Segment<uint64_t> Segment;
  Blob* core_blob = new Blob;
  core_blob->Catch();
  
  core_blob->SetFilename(coredump.c_str());
  core_blob->SetArchitecture(Loader::GetArchitecture(hdr.p));
  core_blob->SetEndian(unisim::util::endian::E_LITTLE_ENDIAN);
  core_blob->SetAddressSize(Loader::GetAddressSize(hdr.p));
  core_blob->SetFileFormat(unisim::util::blob::FFMT_ELF64);
  core_blob->SetELF_PHOFF(hdr.p->e_phoff);
  core_blob->SetELF_PHENT(sizeof(Elf_Phdr));
  core_blob->SetELF_PHNUM(hdr.p->e_phnum);
  core_blob->SetELF_Flags(hdr.p->e_flags);
  
  for (int idx = 0; idx < hdr.p->e_phnum; ++idx)
    {
      Elf_Phdr* phdr = &phdr_table.p[idx];
      switch (Loader::GetSegmentType(phdr))
        {
        case PT_LOAD: /* Loadable Program Segment */
          {
            uint64_t segaddr =      phdr->p_vaddr;
            uint64_t segmem_size =  Loader::GetSegmentMemSize(phdr);
            uint64_t segfile_size = Loader::GetSegmentFileSize(phdr);
            uint64_t ph_flags =     Loader::GetSegmentFlags(phdr);
            uint64_t segalignment = Loader::GetSegmentAlignment(phdr);
            typename Segment::Type segtype = Segment::TY_LOADABLE;
			
            typename Segment::Attribute segattr = Segment::SA_NULL;
            if (ph_flags & PF_W) segattr = (typename Segment::Attribute)(segattr | Segment::SA_W);
            if (ph_flags & PF_R) segattr = (typename Segment::Attribute)(segattr | Segment::SA_R);
            if (ph_flags & PF_X) segattr = (typename Segment::Attribute)(segattr | Segment::SA_X);
			
            void* segdata = calloc(segmem_size, 1);
			
            if (not Loader::LoadSegment(hdr.p, phdr, segdata, is))
              throw "Can't load segment";

            core_blob->AddSegment(new Segment(segtype,segattr,segalignment,segaddr,segmem_size,segfile_size,segdata));
          } break;
          
        case PT_NOTE:
          {
            uintptr_t datasize = Loader::GetSegmentFileSize(phdr);
            if (datasize > 0x10000) throw "WTF";
            uint8_t notedata[datasize];
            if (not Loader::LoadSegment(hdr.p, phdr, &notedata[0], is))
              throw "Can't load segment";

            for (uint8_t *note = &notedata[0], *end = &notedata[datasize]; note < end;)
              {
                Elf_Note* notehdr = (Elf_Note*)( note );
                if (need_endian_swap)
                  {
                    unisim::util::endian::BSwap( notehdr->n_namesz );
                    unisim::util::endian::BSwap( notehdr->n_descsz );
                    unisim::util::endian::BSwap( notehdr->n_type );
                  }
                uint8_t* content = note + 12 + ((notehdr->n_namesz + 3) & -4);
                note = content + ((notehdr->n_descsz + 3) & -4);
                
                switch (notehdr->n_type)
                  {
                  default: break; // Ignoring unknown note
                  case 1: // PRSTATUS note
                    {
                      // Registers are stored in a structure (struct
                      // user_regs_struct) starting at 0x70 from base
                      // content (struct prstatus).
                      //
                      // Register order: {  }

                      uint64_t* regs = (uint64_t*)(content + 0x70);

                      struct { char const* name; unsigned size; } coredumpregs[] =
                        {
                          {"%r15",64}, {"%r14",64}, {"%r13",64}, {"%r12",64}, {"%rbp",64}, {"%rbx",64}, {"%r11",64}, {"%r10",64},
                          {"%r9", 64}, {"%r8", 64}, {"%rax",64}, {"%rcx",64}, {"%rdx",64}, {"%rsi",64}, {"%rdi",64}, {"!orig_rax",64},
                          {"%rip",64}, {"!cs", 16}, {"%eflags",32}, {"%rsp",64},
                          {"!ss", 16}, {"%fs_base",64}, {"%gs_base",64}, {"!ds",16}, {"!es",16}, {"!fs",16}, {"!gs",16}
                        };

                      for (unsigned idx = 0, end = sizeof (coredumpregs) / sizeof coredumpregs[0]; idx < end; ++idx)
                        {
                          if (need_endian_swap) unisim::util::endian::BSwap( regs[idx] );
                          char const* regname = coredumpregs[idx].name;
                          if (regname[0] == '!') continue;
                          unsigned regsize = coredumpregs[idx].size;
                          if (regsize < 64)
                            regs[idx] &= ((uint64_t(1) << regsize)-1);
                          std::cerr << std::setw(12) << regname << ": " << std::hex << regs[idx] << std::endl;
                          if (regsize < 64)
                            {
                              char const* err = 0;
                              if (unisim::service::interfaces::Register* reg = linux_impl.GetDebugRegister(regname))
                                {
                                  if (unsigned(reg->GetSize()) == regsize)
                                    reg->SetValue(&regs[idx]);
                                  else
                                    err = "bad size";
                                }
                              else
                                err = "missing";
                              if (err)
                                std::cerr << "Warning: skipping " << regname << " initialization (" << err << ").\n";
                            }
                          else
                            linux_impl.SetTargetRegister(regname, regs[idx]);
                        }                      
                    } break;
                  case 0x202: // NT_X86_XSTATE note
                    {
                      { /* FCW */
                        uint16_t& fcw = *(uint16_t*)(content);
                        if (need_endian_swap) unisim::util::endian::BSwap( fcw );
                        std::cerr << std::setw(12) << "%fcw" << ": " << std::hex << fcw << std::endl;
                        if (auto reg = linux_impl.GetDebugRegister("%fcw"))
                          reg->SetValue(&fcw);
                        else
                          throw 0;
                      }
                      /* XMM registers */
                      {
                        uint8_t* xmm_bytes = (uint8_t*)(content + 0xa0);
                        if (need_endian_swap) throw 0;
                        for (unsigned reg = 0; reg < 16; ++reg)
                          {
                            std::ostringstream buf; buf << "%xmm" << std::dec << reg;
                            char const* regname = buf.str().c_str();
                            std::cerr << std::setw(12) << regname << ':';
                            for (int idx = 16; --idx>= 0;)
                              {
                                uint8_t byte = xmm_bytes[16*reg+idx];
                                std::cerr
                                  << ' '
                                  << "0123456789abcdef"[(byte>>0)&0b1111]
                                  << "0123456789abcdef"[(byte>>4)&0b1111]
                                  ;
                              }
                            std::cerr << std::endl;
                            if (auto regd = linux_impl.GetDebugRegister(regname))
                              regd->SetValue(&xmm_bytes[16*reg]);
                          }
                      }
                      
                    } break;
                    
                  }
              }
          } break;
        }
    }

  // Load the blob into memory
  for (auto && segment : core_blob->GetSegments())
    {
      if (segment->GetType() != segment->TY_LOADABLE) continue;
      
      uint64_t start, end;
      segment->GetAddrRange(start, end);
      
      if (linux_impl.verbose_)
        std::cerr << "--> writing memory segment start = 0x" << std::hex << start << " end = 0x" << end << std::dec << std::endl;
      
      uint8_t const * data = (uint8_t const *)segment->GetData();
      if (not linux_impl.mem_if_->WriteMemory(start, data, end - start + 1))
        throw "Error while writing the segments into the target memory.";
    }
  linux_impl.is_load_ = true;

  // map std file triplet
  linux_impl.MapTargetToHostFileDescriptor(0, 0); /* stdin */
  linux_impl.MapTargetToHostFileDescriptor(1, 1); /* stdout */
  linux_impl.MapTargetToHostFileDescriptor(2, 2); /* stderr */
}

void
LinuxOS::Setup( bool verbose )
{
  // Set up the different linuxlib parameters
  linux_impl.SetVerbose(verbose);
  
  linux_impl.SetEndianness( unisim::util::endian::E_LITTLE_ENDIAN );
  linux_impl.SetMemoryPageSize( 0x1000UL );
  struct utsname unm;
  uname(&unm);
  linux_impl.SetUname(unm.sysname,
                      unm.nodename,
                      unm.release,
                      unm.version,
                      unm.machine,
                      unm.domainname);
                   
  
  // linux_impl.SetUname("Linux" /* sysname */,
  //                     "localhost" /* nodename */,
  //                     "4.14.89-unisim" /* release */,
  //                     "#1 SMP Fri Mar 12 05:23:09 UTC 2010" /* version */,
  //                     "x86_64" /* machine */,
  //                     "localhost" /* domainname */);
  // linux_impl.SetStdinPipeFilename(stdin_pipe_filename.c_str());
  // linux_impl.SetStdoutPipeFilename(stdout_pipe_filename.c_str());
  // linux_impl.SetStderrPipeFilename(stderr_pipe_filename.c_str());
}

void
LinuxOS::ApplyHostEnvironment()
{
  linux_impl.SetApplyHostEnvironment(true);
}

void
LinuxOS::SetEnvironment( std::vector<std::string> const& envs )
{
  // Set the linuxlib option to set the target environment with the
  // host environment
  linux_impl.SetApplyHostEnvironment(false);
  linux_impl.SetEnvironment(envs);
}

void
LinuxOS::Process( std::vector<std::string> const& simargs )
{
  if (not linux_impl.SetCommandLine(simargs))
    throw 0;
    
  // Set the binary that will be simulated in the target simulator
  if (not linux_impl.AddLoadFile( simargs[0].c_str() ))
    throw 0;
  
  linux_impl.SetStackBase( 0x40000000UL );

  // now it is time to try to run the initialization of the linuxlib
  if (not linux_impl.Load())
    throw 0;
  
  if (not linux_impl.SetupTarget())
    throw 0;
}

void
LinuxOS::ExecuteSystemCall( int id )
{
  linux_impl.ExecuteSystemCall( id, exited, app_ret_status );
}

void
LinuxOS::LogSystemCall(int id)
{
    linux_impl.LogSystemCall(id);
}


void
LinuxOS::SetBrk(addr_t brk)
{
  linux_impl.brk_point_ = brk;
}
