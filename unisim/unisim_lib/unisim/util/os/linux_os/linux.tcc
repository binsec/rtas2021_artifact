/*
 *  Copyright (c) 2011 Commissariat a l'Energie Atomique (CEA) All rights
 *  reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *   endorse or promote products derived from this software without specific
 *   prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_UTIL_OS_LINUX_LINUX_TCC__
#define __UNISIM_UTIL_OS_LINUX_LINUX_TCC__

#include <unisim/util/os/linux_os/environment.hh>
#include <unisim/util/os/linux_os/aux_table.hh>
#include <unisim/util/os/linux_os/errno.hh>
#include <unisim/util/likely/likely.hh>
#include <unisim/util/blob/blob.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/loader/elf_loader/elf32_loader.hh>
#include <unisim/util/loader/elf_loader/elf64_loader.hh>

#if !defined(linux) && !defined(__linux) && !defined(__linux__) && !defined(__APPLE_CC__) && !defined(WIN32) && !defined(_WIN32) && !defined(WIN64) && !defined(_WIN64)
#error "Unsupported host machine for Linux system call translation !"
#endif

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) | defined(_WIN64)
#include <stdlib.h>
#include <sys/stat.h>
#endif

#ifdef GetCommandLine
#undef GetCommandLine
#endif

namespace unisim {
namespace util {
namespace os {
namespace linux_os {

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::Linux(std::ostream& _debug_info_stream,
      std::ostream& _debug_warning_stream,
      std::ostream& _debug_error_stream,
      unisim::service::interfaces::Registers *regs_if, unisim::service::interfaces::Memory<ADDRESS_TYPE> *mem_if,
      unisim::service::interfaces::MemoryInjection<ADDRESS_TYPE> *mem_inject_if)
	: is_load_(false)
	, target_system(0)
	, endianness_(unisim::util::endian::E_LITTLE_ENDIAN)
	, load_files_()
	, entry_point_(0)
	, load_addr_set_(false)
	, load_addr_(0)
	, start_code_(0)
	, end_code_(0)
	, start_data_(0)
	, end_data_(0)
	, elf_stack_(0)
	, elf_brk_(0)
	, num_segments_(0)
	, stack_base_(0)
	, memory_page_size_(0)
	, brk_point_(0)
	, argc_(0)
	, argv_()
	, envc_(0)
	, envp_()
	, apply_host_environnement_(false)
	, target_envp_()
	, utsname()
	, blob_(NULL)
	, regs_if_(regs_if)
	, mem_if_(mem_if)
	, mem_inject_if_(mem_inject_if)
	, verbose_(false)
	, parse_dwarf_(false)
	, debug_dwarf_(false)
	, dwarf_to_html_output_directory_()
	, dwarf_to_xml_output_filename_()
	, debug_info_stream(_debug_info_stream)
	, debug_warning_stream(_debug_info_stream)
	, debug_error_stream(_debug_info_stream)
	, terminated_(false)
	, return_status_(0)
	, stdin_pipe_filename()
	, stdout_pipe_filename()
	, stderr_pipe_filename()
	, stdin_pipe_fd(-1)
	, stdout_pipe_fd(-1)
	, stderr_pipe_fd(-1)
{

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) | defined(_WIN64)
	_setmode(0, _O_BINARY); // force binary mode for the standard input file descriptor (0)
	_setmode(1, _O_BINARY); // force binary mode for the standard output file descriptor (1)
	_setmode(2, _O_BINARY); // force binary mode for the standard error file descriptor (2)
#endif

}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::~Linux()
{
	typename std::map<std::string, unisim::util::blob::Blob<ADDRESS_TYPE> const *>::const_iterator it;
	for(it = load_files_.begin(); it != load_files_.end(); it++)
	{
		const unisim::util::blob::Blob<ADDRESS_TYPE> *blob = (*it).second;
		blob->Release();
	}
	
	load_files_.clear();
	
	if(blob_)
	{
		blob_->Release();
		blob_ = 0;
	}
	
	if(stdin_pipe_fd != -1) close(stdin_pipe_fd);
	
	if(stdout_pipe_fd != -1) close(stdout_pipe_fd);
	
	if(stderr_pipe_fd != -1) close(stderr_pipe_fd);
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetVerbose(bool verbose) {
  verbose_ = verbose;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetParseDWARF(bool parse_dwarf) {
  parse_dwarf_ = parse_dwarf;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetDebugDWARF(bool debug_dwarf) {
  debug_dwarf_ = debug_dwarf;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetDWARFToHTMLOutputDirectory(const char *dwarf_to_html_output_directory)
{
	dwarf_to_html_output_directory_ = dwarf_to_html_output_directory;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetDWARFToXMLOutputFilename(const char *dwarf_to_xml_output_filename)
{
	dwarf_to_xml_output_filename_ = dwarf_to_xml_output_filename;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetCommandLine(std::vector<std::string> const &cmd)
{
  argc_ = cmd.size();
  argv_ = cmd;

  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
std::vector<std::string> Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetCommandLine()
{
  return argv_;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetEnvironment(std::vector<std::string> const &env)
{
  envc_ = env.size();
  envp_ = env;

  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
std::vector<std::string> Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetEnvironment()
{
  return envp_;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetApplyHostEnvironment(bool use_host_environment)
{
  apply_host_environnement_ = use_host_environment;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetApplyHostEnvironment()
{
  return apply_host_environnement_;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::AddLoadFile(char const * const filename)
{
  // NOTE: for the moment we only support one file to load, if this
  // method is called more than once it will return false.
  if (load_files_.size() != 0) return false;

  // check that filename points to something
  if (filename == NULL) {
    debug_warning_stream << "calling AddLoadFile "
        << "without handling file." << std::endl;
    return false;
  }
  
  // check that the file exists and that the elf loader can create a blob from it
  
  typename unisim::util::loader::elf_loader::StdElf<ADDRESS_TYPE,PARAMETER_TYPE>::Loader loader;
  
  loader.SetDebugInfoStream(debug_info_stream);
  loader.SetDebugWarningStream(debug_warning_stream);
  loader.SetDebugErrorStream(debug_error_stream);
  loader.SetRegistersInterface(/* prc_num */ 0, regs_if_);
  loader.SetMemoryInterface(/* prc_num */ 0, mem_if_);
  loader.SetOption(unisim::util::loader::elf_loader::OPT_VERBOSE, verbose_);
  loader.SetOption(unisim::util::loader::elf_loader::OPT_FILENAME, filename);
  loader.SetOption(unisim::util::loader::elf_loader::OPT_PARSE_DWARF, parse_dwarf_);
  loader.SetOption(unisim::util::loader::elf_loader::OPT_DEBUG_DWARF, debug_dwarf_);
  loader.SetOption(unisim::util::loader::elf_loader::OPT_DWARF_TO_HTML_OUTPUT_DIRECTORY, dwarf_to_html_output_directory_.c_str());
  loader.SetOption(unisim::util::loader::elf_loader::OPT_DWARF_TO_XML_OUTPUT_FILENAME, dwarf_to_xml_output_filename_.c_str());

  if (!loader.Load()) {
    debug_error_stream << "Could not load the given file " << "\"" << filename << "\"" << std::endl;
    return false;
  }

  unisim::util::blob::Blob<ADDRESS_TYPE> const * const blob = loader.GetBlob();
  
  if (blob == NULL) {
    debug_error_stream << "Could not create blob from" << " requested file (" << filename << ")." << std::endl;
    return false;
  }

  blob->Catch();
  
  std::string filename_str(filename);
  
  if(load_files_.find(filename_str) != load_files_.end())
    load_files_[filename_str]->Release();
  load_files_[filename_str] = blob;
  
  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetStackBase(ADDRESS_TYPE stack_base)
{
  stack_base_ = stack_base;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetMemoryPageSize(ADDRESS_TYPE memory_page_size)
{
  memory_page_size_ = memory_page_size;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetUname(
    char const * const utsname_sysname,
    char const * const utsname_nodename,
    char const * const utsname_release,
    char const * const utsname_version,
    char const * const utsname_machine,
    char const * const utsname_domainname)
{
  utsname.sysname    = utsname_sysname;
  utsname.nodename   = utsname_nodename;
  utsname.release    = utsname_release;
  utsname.version    = utsname_version;
  utsname.machine    = utsname_machine;
  utsname.domainname = utsname_domainname;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetHWCap(const char *hwcap)
{
  hwcap_ = hwcap;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetStdinPipeFilename(const char *filename)
{
  stdin_pipe_filename = filename;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetStdoutPipeFilename(const char *filename)
{
  stdout_pipe_filename = filename;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetStderrPipeFilename(const char *filename)
{
  stderr_pipe_filename = filename;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
unisim::service::interfaces::Register*
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetDebugRegister( char const* regname ) const
{
  if (not regname) return 0;
  
  if (not regs_if_) {
    debug_error_stream << "No register interface is available" << std::endl;
    return 0;
  }
  
  unisim::service::interfaces::Register* reg = regs_if_->GetRegister(regname);
  if (not reg) {
    debug_error_stream << "Can't access register " << regname << std::endl;
    return 0;
  }
  
  return reg;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::ReadTargetMemory( ADDRESS_TYPE addr, PARAMETER_TYPE& value ) const
{
  if (not this->mem_if_->ReadMemory(addr, (uint8_t *)&value, sizeof(PARAMETER_TYPE)))
    return false;
  
  value = Target2Host(this->GetEndianness(), value);
  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::WriteMemory( ADDRESS_TYPE addr, PARAMETER_TYPE value ) const
{
  value = Host2Target(this->GetEndianness(), value);
  
  return this->WriteMemory(addr, (uint8_t const*)&value, sizeof(PARAMETER_TYPE));
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::WriteMemory( ADDRESS_TYPE addr, uint8_t const * const buffer, uint32_t size ) const
{
  if (this->mem_inject_if_ == NULL) return false;
       
  if (unlikely(this->verbose_))
    {
      this->debug_info_stream
        << "OS write memory:" << std::endl
        << "\taddr = 0x" << std::hex << addr << std::dec << std::endl
        << "\tsize = " << size << std::endl
        << "\tdata =0x" << std::hex;
                       
      for (unsigned int i = 0; i < size; i++)
        {
          this->debug_info_stream << " " << (unsigned int)buffer[i];
        }
               
      this->debug_info_stream << std::dec << std::endl;
    }
  return this->mem_inject_if_->InjectWriteMemory(addr, buffer, size);
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::ReadMemory( ADDRESS_TYPE addr, PARAMETER_TYPE& value, bool inject ) const
{
  if (not ReadMemory(addr, (uint8_t *)&value, sizeof(PARAMETER_TYPE), inject))
    return false;
  
  value = Target2Host(this->GetEndianness(), value);
  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::ReadMemory( ADDRESS_TYPE addr, uint8_t* buffer, uint32_t size, bool inject ) const
{
  if ((    inject and not this->mem_inject_if_) or
      (not inject and not        this->mem_if_))
    {
      this->debug_warning_stream << "LinuxOS has no memory connection." << std::endl;
      return false;
    }
  
  if ((    inject and not this->mem_inject_if_->InjectReadMemory(addr, buffer, size)) or
      (not inject and not              this->mem_if_->ReadMemory(addr, buffer, size)))
    {
      this->debug_warning_stream
        << "failed OS read memory" << (inject ? " (debug):" : ":") << std::endl
        << "\taddr = 0x" << std::hex << addr << std::dec << std::endl
        << "\tsize = " << size
        << std::endl;
      
      return false;
    }
  
  if (unlikely(this->verbose_))
    {
      this->debug_info_stream
        << "OS read memory" << (inject ? " (debug):" : ":") << std::endl
        << "\taddr = 0x" << std::hex << addr << std::dec << std::endl
        << "\tsize = " << size << std::endl
        << "\tdata =0x" << std::hex;
        
      for (unsigned int i = 0; i < size; i++)
        {
          this->debug_info_stream << " " << (unsigned int)buffer[i];
        }
                       
      this->debug_info_stream << std::dec << std::endl;
    }
      
  return true;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::ReadString( ADDRESS_TYPE addr, std::string& str, bool inject ) const
{
  // Two pass string retrieval
  int len = 0;
  for (ADDRESS_TYPE tail = addr; ; len += 1, tail += 1)
    {
      if (len >= 0x100000) {
        debug_error_stream << "Huge string: bailing out" << std::endl;
        return false;
      }
      uint8_t buffer;
      if (not ReadMemory(tail, &buffer, 1, false))
        {
          this->debug_error_stream << "Can't read memory at 0x" << std::hex << tail << std::endl;
          return false;
        }
      if (buffer == '\0') break;
    }
  
  str.resize( len, '*' );
  
  return ReadMemory(addr, (uint8_t*)&str[0], len, inject);
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetTargetRegister( char const* regname, PARAMETER_TYPE& value ) const
{
  if (unisim::service::interfaces::Register* reg = GetDebugRegister(regname))
    {
      if (reg->GetSize() == sizeof(PARAMETER_TYPE))
        {
          reg->GetValue(&value);
          return true;
        }
        debug_error_stream << "Bad register size for " << regname << std::endl;
    }
  
  return false;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
PARAMETER_TYPE Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetTargetRegister( char const* regname ) const
{
  PARAMETER_TYPE result;
  if (not GetTargetRegister(regname, result))
    throw (unisim::service::interfaces::Register*)0;
  return result;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetTargetRegister( char const* regname, PARAMETER_TYPE value ) const
{
  if (unisim::service::interfaces::Register* reg = GetDebugRegister(regname))
    {
      if (reg->GetSize() == sizeof(PARAMETER_TYPE))
        {
          reg->SetValue(&value);
          return true;
        }
      debug_error_stream << "Bad register size for " << regname << std::endl;
    }
  
  return false;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::ClearTargetRegister( char const* regname ) const
{
  if (unisim::service::interfaces::Register* reg = GetDebugRegister(regname))
    {
      reg->Clear();
      return true;
    }
  
  return false;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::Load()
{
	// set the environment to be used
	if (apply_host_environnement_) GetHostEnvironment(envp_, &target_envp_);
	else target_envp_ = envp_;

	// create the root blob that will be used to store the image that will be
	// loaded
	unisim::util::blob::Blob<ADDRESS_TYPE>* blob =
	new unisim::util::blob::Blob<ADDRESS_TYPE>();
	blob->Catch();
	
	// load and add files to the blob
	if(verbose_)
	{
          debug_info_stream << "Loading elf files." << std::endl;
	}
	if(!LoadFiles(blob))
	{
		debug_error_stream
			<< "Could not load elf files." << std::endl;
		blob->Release();
		return false;
	}

	// create the stack footprint and add it to the blob
	uint64_t stack_size = 0;
	if(verbose_)
	{
		debug_info_stream
			<< "Creating the Linux software stack." << std::endl;
	}
	if (!CreateStack(blob, stack_size))
	{
		// TODO
		// Remove non finished state (i.e., unfinished blob, reset values, ...)
		debug_error_stream
			<< "Could not create the Linux software stack." << std::endl;
		blob->Release();
		return false;
	}

	// finish the state of the memory image depending on the system we are running
	// on
	if(verbose_)
	{
		debug_info_stream
			<< "Setting the system blob." << std::endl;
	}
        // Add target dependant Segments
	if (not target_system->SetSystemBlob(blob))
	{
		// TODO: Remove non finished state (i.e., unfinished
		//       blob, reset values, ...)
		debug_error_stream
			<< "Could not set the system blob." << std::endl;
		blob->Release();
		return false;
	}

	// Setup brk_point
	ADDRESS_TYPE top_addr = stack_base_ + stack_size - 1;
	if (verbose_)
	{
		debug_info_stream << "=== top_addr = 0x" << std::hex << top_addr << std::dec
		<< std::endl;
	}

	brk_point_ = top_addr +	(memory_page_size_ - (top_addr % memory_page_size_));

	if(verbose_)
	{
		debug_info_stream
			<< "=== stack_size_ = 0x" << std::hex << stack_size << " (" << std::dec << stack_size << ")" << std::endl
			<< "=== brk_point_ = 0x" << std::hex << brk_point_ << std::endl
			<< "=== memory_page_size_ = 0x" << memory_page_size_ << " ("
			<< std::dec << memory_page_size_ << ")" << std::endl;
	}

	if (blob_) blob_->Release();
	blob_ = blob;
	is_load_ = true;

	if(!stdin_pipe_filename.empty())
	{
		int stdin_pipe_flags = O_RDONLY;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) | defined(_WIN64)
		stdin_pipe_flags |= O_BINARY;
#endif
		stdin_pipe_fd = open(stdin_pipe_filename.c_str(), stdin_pipe_flags);
		if(stdin_pipe_fd == -1)
		{
			debug_error_stream << "Can't open \"" << stdin_pipe_filename << "\"" << std::endl;
			return false;
		}
	}
	
	if(!stdout_pipe_filename.empty())
	{
		int stdout_pipe_flags = O_WRONLY | O_CREAT | O_TRUNC;
		mode_t stdout_pipe_mode = S_IRWXU;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) | defined(_WIN64)
		stdout_pipe_flags |= O_BINARY;
#else
		stdout_pipe_mode |= S_IRWXG | S_IRWXO;
#endif
		stdout_pipe_fd = open(stdout_pipe_filename.c_str(), stdout_pipe_flags, stdout_pipe_mode);
		if(stdout_pipe_fd == -1)
		{
			debug_error_stream << "Can't open \"" << stdout_pipe_filename << "\"" << std::endl;
			return false;
		}
	}
	
	if(!stderr_pipe_filename.empty())
	{
		int stderr_pipe_flags = O_WRONLY | O_CREAT | O_TRUNC;
		mode_t stderr_pipe_mode = S_IRWXU;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) | defined(_WIN64)
		stderr_pipe_flags |= O_BINARY;
#else
		stderr_pipe_mode |= S_IRWXG | S_IRWXO;
#endif
		stderr_pipe_fd = open(stderr_pipe_filename.c_str(), stderr_pipe_flags, stderr_pipe_mode);
		if(stderr_pipe_fd == -1)
		{
			debug_error_stream << "Can't open \"" << stderr_pipe_filename << "\"" << std::endl;
			return false;
		}
	}

	MapTargetToHostFileDescriptor(0, (stdin_pipe_fd == -1) ? 0 : stdin_pipe_fd); // map target stdin file descriptor to either host stdin file descriptor or host input file descriptor
	MapTargetToHostFileDescriptor(1, (stdout_pipe_fd == -1) ? 1 : stdout_pipe_fd); // map target stdout file descriptor to either host stdout file descriptor or host output file descriptor
	MapTargetToHostFileDescriptor(2, (stderr_pipe_fd == -1) ? 2 : stderr_pipe_fd); // map target stdout file descriptor to either host stderr file descriptor or host output file descriptor

	return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::IsLoad() {
  return blob_ != NULL;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetupTarget()
{
  if ((mem_if_ == NULL) or (mem_inject_if_ == NULL) or (regs_if_ == NULL))
    {
      debug_error_stream << "The linux system interfaces (memory/register) were not assigned" << std::endl;
      return false;
    }

  if (blob_ == NULL)
    {
      debug_error_stream << "The linux system was not loaded." << std::endl;
      return false;
    }
  
  typedef unisim::util::blob::Segment<ADDRESS_TYPE> Segment;
  typedef std::vector<const Segment*> SegmentVector;
  typedef typename SegmentVector::const_iterator SegmentVectorIterator;
  
  SegmentVector segments = blob_->GetSegments();
  for (SegmentVectorIterator it = segments.begin(); (it != segments.end()); it++)
    {
      if ((*it)->GetType() != Segment::TY_LOADABLE) continue;
      
      uint8_t const * data = (uint8_t const *)(*it)->GetData();
      ADDRESS_TYPE start, end;
      (*it)->GetAddrRange(start, end);
      
      if(verbose_) {
        debug_info_stream << "--> writing memory segment start = 0x"
                << std::hex << start << " end = 0x" << end << std::dec
                << std::endl;
      }
      
      if (not mem_if_->WriteMemory(start, data, end - start + 1))
        {
          debug_error_stream << "Error while writing the segments into the target memory." << std::endl;
          return false;
        }
    }

  return target_system->SetupTarget();
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LogSystemCall(int id)
{
  int translated_id = id;
  
  SysCall* sc = target_system->GetSystemCall( translated_id );

  debug_info_stream << "Syscall(id=" << std::dec << translated_id;
  if (translated_id != id)
    debug_info_stream << ", " << "unstranslated id=" << id;
  debug_info_stream << "): ";

  if (sc)
    {
      debug_error_stream << sc->TraceCall( *this );
      sc->Release();
    }
  else
    debug_error_stream << "unknown";
  
  debug_error_stream << std::endl;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
std::string
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SysCall::TraceCall( Linux& lin ) const
{
  std::ostringstream oss;
  oss << GetName();
  Describe( lin, oss );
  return oss.str();
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SysCall::Execute( Linux& lin, int syscall_id ) const
{
  if (unlikely(lin.verbose_)) {
    lin.debug_warning_stream << this->GetName() << " is not implemented" << std::endl;
    lin.debug_info_stream << this->TraceCall(lin) << std::endl;
  }
  lin.SetSystemCallStatus(-LINUX_EINVAL, true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetSystemCallStatus(int64_t ret, bool error)
{
  if (unlikely(verbose_))
    debug_info_stream << (error ? "err" : "ret") << " = 0x" << std::hex << ret << std::dec << std::endl;
  
  target_system->SetSystemCallStatus(ret, error);
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::ExecuteSystemCall(int id, bool& terminated, int& return_status)
{
  if (not is_load_ or not target_system)
    {
      debug_error_stream
        << "unisim::util::os::linux_os::Linux.ExecuteSystemCall: "
        << "Trying to execute system call with id " << id << " while the linux "
        << "emulation has not been correctly configured."
        << std::endl;
      return;
    }
  
  int translated_id = id;
  
  SysCall* sc = target_system->GetSystemCall( translated_id );
  
  if (not sc) {
    debug_error_stream << "Unknown syscall(id = " << translated_id << ", untranslated id = " << id << ")" << std::endl;
    // FIXME: shouldn't we end the simulation (terminated = true) ?
    target_system->SetSystemCallStatus(-LINUX_ENOSYS, true);
    return;
  }
  
  if (unlikely(verbose_))
    {
      debug_info_stream
        << "Executing syscall(name = " << sc->GetName() << ", "
        << "id = " << translated_id << ", " << "unstranslated id = " << id << ")"
        << std::endl;
    }
  
  try {
    sc->Execute( *this, translated_id );
  } catch (LSCExit const scx) {
    terminated = true;
    return_status = scx.status;
  }
  sc->Release();
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LoadFiles( unisim::util::blob::Blob<ADDRESS_TYPE> *blob )
{
  // Get the main executable blob
  // NOTE: current implementation of the linux os library only supports one
  // blob, the main executable blob
  unisim::util::blob::Blob<ADDRESS_TYPE> const * const main_blob =
      GetMainBlob();

  if (main_blob == NULL) return false;

  if (main_blob->GetCapability() & unisim::util::blob::CAP_FILE_FORMAT) {
    blob->SetFileFormat(main_blob->GetFileFormat());
  }
  if(main_blob->GetCapability() & unisim::util::blob::CAP_FILENAME) {
    blob->SetFilename(main_blob->GetFilename());
  }
  if(main_blob->GetCapability() & unisim::util::blob::CAP_ENTRY_POINT) {
    blob->SetEntryPoint(main_blob->GetEntryPoint());
  }
  if(main_blob->GetCapability() & unisim::util::blob::CAP_ARCHITECTURE) {
    blob->SetArchitecture(main_blob->GetArchitecture());
  }
  if(main_blob->GetCapability() & unisim::util::blob::CAP_STACK_BASE) {
    blob->SetStackBase(main_blob->GetStackBase());
  }
  if(main_blob->GetCapability() & unisim::util::blob::CAP_ENDIAN) {
    blob->SetEndian(main_blob->GetEndian());
  }
  if(main_blob->GetCapability() & unisim::util::blob::CAP_FILE_ENDIAN) {
    blob->SetFileEndian(main_blob->GetFileEndian());
  }
  if(main_blob->GetCapability() & unisim::util::blob::CAP_ADDRESS_SIZE) {
    blob->SetAddressSize(main_blob->GetAddressSize());
  }
  if(main_blob->GetCapability() & unisim::util::blob::CAP_ELF_PHOFF) {
    blob->SetELF_PHOFF(main_blob->GetELF_PHOFF());
  }
  if(main_blob->GetCapability() & unisim::util::blob::CAP_ELF_PHENT) {
    blob->SetELF_PHENT(main_blob->GetELF_PHENT());
  }

  // compute the different structure addresses from the given blob
  if (!ComputeStructuralAddresses(*main_blob))
    return false;

  if (!FillBlobWithFileBlob(*main_blob, blob))
    return false;

  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
unisim::util::blob::Blob<ADDRESS_TYPE> const * const Linux<ADDRESS_TYPE,
    PARAMETER_TYPE>:: GetMainBlob() const {
  /* NOTE only one file is supported, so we just get the first one if any */
  typename std::map<std::string,
      unisim::util::blob::Blob<ADDRESS_TYPE> const *>::const_iterator
      it = load_files_.begin();
  if (it == load_files_.end()) return NULL;
  else return it->second;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::ComputeStructuralAddresses(
    unisim::util::blob::Blob<ADDRESS_TYPE> const &blob) {
  int loaded_segments = 0;
  typedef unisim::util::blob::Segment<ADDRESS_TYPE> Segment;
  typedef std::vector<const Segment *> SegmentVector;
  typedef typename SegmentVector::const_iterator SegmentVectorIterator;
  const SegmentVector &segments = blob.GetSegments();

  // Reset application structure addresses
  entry_point_ = blob.GetEntryPoint();
  load_addr_ = ~((ADDRESS_TYPE)0);
  start_code_ = ~((ADDRESS_TYPE)0);
  end_code_ = 0;
  start_data_ = 0;
  end_data_ = 0;
  elf_stack_ = ~((ADDRESS_TYPE)0);
  elf_brk_ = 0;
  num_segments_ = 0;

  for (SegmentVectorIterator it = segments.begin(); it != segments.end(); it++) {
    typename unisim::util::blob::Segment<ADDRESS_TYPE>::Type type =
        (*it)->GetType();
    // ignore the segment if it is not a loadable
    if (type != unisim::util::blob::Segment<ADDRESS_TYPE>::TY_LOADABLE)
      continue;

    loaded_segments++;

    ADDRESS_TYPE segment_addr = (*it)->GetAddr();
    ADDRESS_TYPE segment_end_addr = segment_addr + (*it)->GetSize();
    if (load_addr_ > segment_addr) {
      // TODO
      // WARNING
      // We are not considering the offset in the elf file. It would be better
      // to set load_addr = segment_addr - segment_offset
      load_addr_ = segment_addr;
    }
    if (segment_addr < start_code_)
      start_code_ = segment_addr;
    if (start_data_ < segment_addr)
      start_data_ = segment_addr;
    if ((*it)->GetAttr() & Segment::SA_X) {
      if (end_code_ < segment_end_addr)
        end_code_ = segment_end_addr;
    }
    if (end_data_ < segment_end_addr)
      end_data_ = segment_end_addr;
    if (segment_end_addr > elf_brk_)
      elf_brk_ = segment_end_addr;
  }

  if (loaded_segments == 0) return false;
  num_segments_ = segments.size();
  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::FillBlobWithFileBlob(
    unisim::util::blob::Blob<ADDRESS_TYPE> const &file_blob,
    unisim::util::blob::Blob<ADDRESS_TYPE> *blob) {
  typedef unisim::util::blob::Segment<ADDRESS_TYPE> Segment;
  typedef std::vector<const Segment*> SegmentVector;
  typedef typename SegmentVector::const_iterator SegmentVectorIterator;
  typedef unisim::util::blob::Section<ADDRESS_TYPE> Section;
  typedef std::vector<const Section*> SectionVector;
  typedef typename SectionVector::const_iterator SectionVectorIterator;
  const SegmentVector &file_segments = file_blob.GetSegments();
  const SectionVector &file_sections = file_blob.GetSections();

  // copy only segments that have data to be loaded
  for (SegmentVectorIterator it = file_segments.begin();
       it != file_segments.end(); it++) {
    typename unisim::util::blob::Segment<ADDRESS_TYPE>::Type type =
        (*it)->GetType();
    // ignore the segment if it is not a loadable
    if (type != unisim::util::blob::Segment<ADDRESS_TYPE>::TY_LOADABLE)
      continue;

    blob->AddSegment((*it));
  }

  // copy all the sections
  for (SectionVectorIterator it = file_sections.begin();
       it != file_sections.end(); it++) {
    blob->AddSection((*it));
  }
  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::CreateStack(unisim::util::blob::Blob<ADDRESS_TYPE>* blob, uint64_t& stack_size) const
{
  typedef std::vector<ADDRESS_TYPE> AddressVector;
  typedef typename AddressVector::iterator AddressVectorIterator;
  typedef typename AddressVector::reverse_iterator AddressVectorReverseIterator;
  typedef unisim::util::blob::Section<ADDRESS_TYPE> Section;
  typedef unisim::util::blob::Segment<ADDRESS_TYPE> Segment;

  // make sure a blob is being handled
  if (blob == NULL) {
    debug_error_stream
        << "no blob handled to method." << std::endl;
    return false;
  }
  // make sure argv has been defined, at least for the application to execute
  if ((argc_ == 0) || (argv_.size() == 0)) {
    debug_error_stream
        << "argc and/or size(argv) is/are 0." << std::endl;
    return false;
  }
  
  ADDRESS_TYPE aux_table_size = 0;
  SetAuxTable(0, aux_table_size);
  aux_table_size = -aux_table_size;
  
  int pass;
  stack_size = 0;
  Section* env_section = 0;
  Section* argv_section = 0;
  Section* aux_table_section = 0;
  Section* envp_section = 0;
  Section* argvp_section = 0;
  Section* argc_section = 0;
  Section* stack_pointer_section = 0;
  uint8_t *stack_data = 0;
  // First pass for computing the size of the initialized stack
  // Second pass for actually initializing the stack
  for(pass = 1; pass <= 2; pass++)
  {
    // Create the stack
    ADDRESS_TYPE sp = (pass == 2) ? stack_size : 0;
    
    stack_data = (pass == 2) ? (uint8_t *) calloc(stack_size, 1) : 0;
    
    // Fill the stack
    ADDRESS_TYPE cur_length;
    // - copy envp
    // variable to keep the stack environment entries addresses
    std::vector<ADDRESS_TYPE> sp_envp;
    ADDRESS_TYPE env_top = sp;
    for (std::vector<std::string>::const_iterator it = target_envp_.begin();
        it != target_envp_.end(); it++) {
      cur_length = (*it).length() + 1;
      sp = sp - cur_length;
      sp_envp.push_back(sp);
      if(pass == 2) memcpy(stack_data + sp, (*it).c_str(), cur_length);
    }
    ADDRESS_TYPE env_bottom = sp;
    if(pass == 2) {
      uint8_t *env_section_data = (uint8_t *)malloc(env_top - env_bottom);
      memcpy(env_section_data, stack_data + env_bottom, env_top - env_bottom);
      env_section =
          new Section(Section::TY_PROGBITS, Section::SA_AW,
                      ".unisim.linux_os.stack.env", 1, 0, stack_base_ + env_bottom,
                      env_top - env_bottom,env_section_data);
    }
    // - copy argv
    // variable to keep the stack argument entries addresses
    std::vector<ADDRESS_TYPE> sp_argv;
    ADDRESS_TYPE argv_top = sp;
    for (std::vector<std::string>::const_iterator it = argv_.begin();
        it != argv_.end(); it++) {
      cur_length = (*it).length() + 1;
      sp = sp - cur_length;
      sp_argv.push_back(sp);
      if(pass == 2) memcpy(stack_data + sp, (*it).c_str(), cur_length);
    }
    ADDRESS_TYPE argv_bottom = sp;
    if(pass == 2) {
      uint8_t *argv_section_data = (uint8_t *)malloc(argv_top - argv_bottom);
      memcpy(argv_section_data, stack_data + argv_bottom, argv_top - argv_bottom);
      argv_section =
          new Section(Section::TY_PROGBITS, Section::SA_AW,
                      ".unisim.linux_os.stack.argv", 1, 0,
                      stack_base_ + argv_bottom, argv_top - argv_bottom,
                      argv_section_data);
    }
    // force 16 byte alignment
    sp = sp & ~(ADDRESS_TYPE)0x0f;
    // compute the required stack size for the aux table entries and the pointers
    // to the different environment and argument values
    ADDRESS_TYPE sp_content_size = 0;
    // aux table including AT_NULL entry
    sp_content_size += aux_table_size;
    // number of environment entries
    sp_content_size += target_envp_.size() * sizeof(ADDRESS_TYPE);
    //   and its termination
    sp_content_size += sizeof(ADDRESS_TYPE);
    // number of argument entries (argc)
    sp_content_size += argv_.size() * sizeof(ADDRESS_TYPE);
    //   and its termination
    sp_content_size += sizeof(ADDRESS_TYPE);
    // entry for argc itself
    sp_content_size += sizeof(ADDRESS_TYPE);
    if ( sp_content_size & 0x0f ) // force aligned start
      sp = sp - (16 - (sp_content_size & 0x0f));

    ADDRESS_TYPE aux_table_top = sp;
    SetAuxTable((pass == 2) ? stack_data : 0, sp);
    ADDRESS_TYPE aux_table_bottom = sp;
    if(pass == 2) {
      uint8_t *aux_table_section_data = (uint8_t *)malloc(aux_table_top - aux_table_bottom);
      memcpy(aux_table_section_data, stack_data + aux_table_bottom, aux_table_top - aux_table_bottom);
      aux_table_section =
          new Section(Section::TY_PROGBITS, Section::SA_AW,
                      ".unisim.linux_os.stack.aux_table", 1, 0,
                      stack_base_ + aux_table_bottom,
                      aux_table_top - aux_table_bottom,
                      aux_table_section_data);
    }
    // Now we have to put the pointers to the different argv and envp
    ADDRESS_TYPE envp_top = sp;
    ADDRESS_TYPE envp_value = 0;
    sp -= sizeof(envp_value);
    if(pass == 2) memcpy(stack_data + sp, &envp_value, sizeof(envp_value));
    for (AddressVectorIterator it = sp_envp.begin();
        it != sp_envp.end();
        ++it) {
      sp -= sizeof(envp_value);
      envp_value = Host2Target(endianness_, (*it) + stack_base_);
      if(pass == 2) memcpy(stack_data + sp, &envp_value, sizeof(envp_value));
    }
    ADDRESS_TYPE envp_bottom = sp;
    if(pass == 2) {
      uint8_t *envp_section_data = (uint8_t *)malloc(envp_top - envp_bottom);
      memcpy(envp_section_data, stack_data + envp_bottom, envp_top - envp_bottom);
      envp_section =
          new Section(Section::TY_PROGBITS, Section::SA_AW,
                      ".unisim.linux_os.stack.environment_pointers",
                      1, 0, stack_base_ + envp_bottom,
                      envp_top - envp_bottom,
                      envp_section_data);
    }
    
    ADDRESS_TYPE argvp_top = sp;
    ADDRESS_TYPE argvp_value = 0;
    sp -= sizeof(argvp_value);
    if(pass == 2) memcpy(stack_data + sp, &argvp_value, sizeof(argvp_value));
    for (AddressVectorReverseIterator it = sp_argv.rbegin();
        it != sp_argv.rend();
        ++it) {
      sp -= sizeof(argvp_value);
      argvp_value = Host2Target(endianness_, (*it) + stack_base_);
      if(pass == 2) memcpy(stack_data + sp, &argvp_value, sizeof(argvp_value));
    }
    ADDRESS_TYPE argvp_bottom = sp;
    if(pass == 2) {
      uint8_t *argvp_section_data = (uint8_t *)malloc(argvp_top - argvp_bottom);
      memcpy(argvp_section_data, stack_data + argvp_bottom, argvp_top - argvp_bottom);
      argvp_section =
          new Section(Section::TY_PROGBITS, Section::SA_AW,
                      ".unisim.linux_os.stack.argument_pointers",
                      1, 0, stack_base_ + argvp_bottom,
                      argvp_top - argvp_bottom,
                      argvp_section_data);
    }
    // and finally we put argc into the stack
    ADDRESS_TYPE argc_top = sp;
    ADDRESS_TYPE argc_value = Host2Target(endianness_, ADDRESS_TYPE(argc_));
    sp -= sizeof(argc_value);
    if(pass == 2) memcpy(stack_data + sp, &argc_value, sizeof(argc_value));
    ADDRESS_TYPE argc_bottom = sp;
    if(pass == 2) {
      uint8_t *argc_section_data = (uint8_t *)malloc(argc_top - argc_bottom);
      memcpy(argc_section_data, stack_data + argc_bottom, argc_top - argc_bottom);
      argc_section =
          new Section(Section::TY_PROGBITS, Section::SA_AW,
                      ".unisim.linux_os.stack.argument_counter",
                      1, 0, stack_base_ + argc_bottom,
                      argc_top - argc_bottom,
                      argc_section_data);
    }
    // create an empty section to keep the stack pointer
    if(pass == 2) {
      stack_pointer_section =
          new Section(Section::TY_NULL, Section::SA_NULL,
                      ".unisim.linux_os.stack.stack_pointer",
                      0, 0, stack_base_ + sp,
                      0, NULL);
    }
    
    if(pass == 1) stack_size = -sp;
  }
  
  // create a segment for the stack
  Segment* stack_segment =
      new Segment(Segment::TY_LOADABLE, Segment::SA_RW,
                  4, stack_base_, stack_size, stack_size, stack_data);

  // add the stack segment and the different sections to the blob
  blob->AddSegment(stack_segment);
  blob->AddSection(env_section);
  blob->AddSection(argv_section);
  blob->AddSection(aux_table_section);
  blob->AddSection(envp_section);
  blob->AddSection(argvp_section);
  blob->AddSection(argc_section);
  blob->AddSection(stack_pointer_section);

  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetAuxTable(
    uint8_t* stack_data, ADDRESS_TYPE &sp) const {

  ADDRESS_TYPE aux_table_symbol;
  ADDRESS_TYPE aux_table_value;
  unisim::util::blob::Blob<ADDRESS_TYPE> const * const main_blob =
      GetMainBlob();

  aux_table_symbol = AT_NULL;
  aux_table_value = 0;
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_PHDR;
  /* TODO
   * WARNING
   * load_addr does not consider the segments offsets in the elf file
   * (see previous warning).
   * The elf library should provide information on the size of the elf header.
   */
  
  aux_table_value = load_addr_ + main_blob->GetELF_PHOFF(); // start address of ELF program headers
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_PHENT;
  /* TODO
   * WARNING
   * The elf library should provide information on the size of the program
   * header.
   */
  aux_table_value = main_blob->GetELF_PHENT(); // size of the program header
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_PHNUM;
  aux_table_value = main_blob->GetELF_PHNUM(); // Number of program headers
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_PAGESZ;
  aux_table_value = memory_page_size_;
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  /* TODO
   * Add support for interpreter.
   */
  aux_table_symbol = AT_BASE;
  aux_table_value = 0; // 0 to signal no interpreter
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_FLAGS;
  aux_table_value = 0;
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_ENTRY;
  aux_table_value = entry_point_;
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_UID;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) | defined(_WIN64)
  aux_table_value = (ADDRESS_TYPE) 1000;
#else
  aux_table_value = (ADDRESS_TYPE)getuid();
#endif
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_EUID;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) | defined(_WIN64)
  aux_table_value = (ADDRESS_TYPE) 1000;
#else
  aux_table_value = (ADDRESS_TYPE)geteuid();
#endif
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_GID;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) | defined(_WIN64)
  aux_table_value = (ADDRESS_TYPE) 1000;
#else
  aux_table_value = (ADDRESS_TYPE)getgid();
#endif
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_EGID;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) | defined(_WIN64)
  aux_table_value = (ADDRESS_TYPE) 1000;
#else
  aux_table_value = (ADDRESS_TYPE)getegid();
#endif
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);
  
  if (target_system->GetAT_HWCAP( aux_table_value ))
    
  {
    aux_table_symbol = AT_HWCAP;
    sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);
  }

  aux_table_symbol = AT_CLKTCK;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) | defined(_WIN64)
  aux_table_value = (ADDRESS_TYPE) 250;
#else
  aux_table_value = (ADDRESS_TYPE)sysconf(_SC_CLK_TCK);
#endif
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
ADDRESS_TYPE Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetAuxTableEntry( uint8_t * stack_data, ADDRESS_TYPE sp, ADDRESS_TYPE entry, ADDRESS_TYPE value) const
{
  ADDRESS_TYPE target_entry = Host2Target(endianness_, entry);
  ADDRESS_TYPE target_value = Host2Target(endianness_, value);
  sp = sp - sizeof(target_value);
  uint8_t *addr;
  if(stack_data) {
    addr = stack_data + sp;
    memcpy(addr, &target_value, sizeof(target_value));
  }
  sp = sp - sizeof(target_entry);
  if(stack_data) {
    addr = stack_data + sp;
    memcpy(addr, &target_entry, sizeof(target_entry));
  }
  return sp;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int32_t Linux<ADDRESS_TYPE, PARAMETER_TYPE>::AllocateFileDescriptor()
{
	if(!target_fildes_free_list.empty())
	{
		int32_t fildes = target_fildes_free_list.front();
		target_fildes_free_list.pop();
		return fildes;
	}
	
	std::map<int32_t, int>::reverse_iterator iter = target_to_host_fildes.rbegin();
	
	if(iter != target_to_host_fildes.rend())
	{
		return (*iter).first + 1;
	}
	
	return 0;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::FreeFileDescriptor(int32_t fd)
{
	target_fildes_free_list.push(fd);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::MapTargetToHostFileDescriptor(int32_t target_fd, int host_fd)
{
	target_to_host_fildes.insert(std::pair<int32_t, int>(target_fd, host_fd));
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::UnmapTargetToHostFileDescriptor(int32_t target_fd)
{
	std::map<int32_t, int>::iterator iter = target_to_host_fildes.find(target_fd);
	
	if(iter != target_to_host_fildes.end())
	{
		target_to_host_fildes.erase(iter);
	}
}

} // end of namespace linux
} // end of namespace os
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_OS_LINUX_LINUX_TCC__
