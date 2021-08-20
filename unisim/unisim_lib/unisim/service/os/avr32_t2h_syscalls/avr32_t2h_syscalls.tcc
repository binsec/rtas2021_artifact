/*
 *  Copyright (c) 2014,
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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */
 
#ifndef __UNISIM_SERVICE_OS_AVR32_T2H_SYSCALLS_AVR32_T2H_SYSCALLS_TCC__
#define __UNISIM_SERVICE_OS_AVR32_T2H_SYSCALLS_AVR32_T2H_SYSCALLS_TCC__

#include "unisim/kernel/kernel.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/service/interfaces/memory_injection.hh"
#include "unisim/util/endian/endian.hh"
#include "unisim/util/likely/likely.hh"
#include "unisim/service/os/avr32_t2h_syscalls/avr32_t2h_syscalls.hh"

#include <map>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#include <windows.h>
#include <time.h>
#endif

namespace unisim {
namespace service {
namespace os {
namespace avr32_t2h_syscalls {

using std::string;
using std::stringstream;
using std::hex;
using std::dec;
using std::endl;
using std::flush;
using std::map;
using unisim::util::endian::Host2BigEndian;
using unisim::kernel::Service;
using unisim::kernel::Object;
using unisim::kernel::Client;
using unisim::kernel::ServiceImport;
using unisim::kernel::ServiceExport;
using unisim::kernel::variable::Parameter;
using unisim::kernel::logger::Logger;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;
using unisim::service::interfaces::MemoryInjection;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::SymbolTableLookup;

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
// see http://mathieuturcotte.ca/textes/windows-gettimeofday
struct timezone {
  int tz_minuteswest;     /* minutes west of Greenwich */
  int tz_dsttime;         /* type of DST correction */
};
int gettimeofday(struct timeval* p, struct timezone* tz) {
  ULARGE_INTEGER ul; // As specified on MSDN.
  FILETIME ft;

  // Returns a 64-bit value representing the number of
  // 100-nanosecond intervals since January 1, 1601 (UTC).
  GetSystemTimeAsFileTime(&ft);

  // Fill ULARGE_INTEGER low and high parts.
  ul.LowPart = ft.dwLowDateTime;
  ul.HighPart = ft.dwHighDateTime;
  // Convert to microseconds.
  ul.QuadPart /= 10ULL;
  // Remove Windows to UNIX Epoch delta.
  ul.QuadPart -= 11644473600000000ULL;
  // Modulo to retrieve the microseconds.
  p->tv_usec = (long) (ul.QuadPart % 1000000LL);
  // Divide to retrieve the seconds.
  p->tv_sec = (long) (ul.QuadPart / 1000000LL);

  tz->tz_minuteswest = 0;
  tz->tz_dsttime = 0;

  return 0;
}
#endif

template <class MEMORY_ADDR>
AVR32_T2H_Syscalls<MEMORY_ADDR>::AVR32_T2H_Syscalls(const char *name, Object *parent)
	: Object(name, parent)
	, Service<unisim::service::interfaces::AVR32_T2H_Syscalls>(name, parent)
	, Client<MemoryInjection<MEMORY_ADDR> >(name, parent)
	, Client<Registers>(name, parent)
	, Client<Blob<MEMORY_ADDR> >(name, parent)
	, avr32_t2h_syscalls_export("avr32-t2h-syscalls-export", this)
	, memory_injection_import("memory-injection-import", this)
	, registers_import("registers-import", this)
	, blob_import("blob-import", this)
	, logger(*this)
	, reg_pc(0)
	, reg_sp(0)
	, reg_syscall_num(0)
	, reg_params()
	, reg_return_status(0)
	, reg_errno(0)
	, t2h_syscall_table()
	, terminated(false)
	, exit_status(0)
	, pc_register_name("pc")
	, sp_register_name("sp")
	, syscall_num_register_name("r8")
	, return_status_register_name("r12")
	, errno_register_name("r11")
	, param_register_names()
	, argc(0)
	, argv()
	, stdin_pipe_filename()
	, stdout_pipe_filename()
	, stderr_pipe_filename()
	, stdin_pipe_fd(-1)
	, stdout_pipe_fd(-1)
	, stderr_pipe_fd(-1)
	, verbose_all(false)
	, verbose_syscalls(false)
	, verbose_setup(false)
	, param_verbose_all("verbose-all", this, verbose_all, "globally enable/disable verbosity")
	, param_verbose_syscalls("verbose-syscalls", this, verbose_syscalls, "enable/disable verbosity while system calls")
	, param_verbose_setup("verbose-setup", this, verbose_setup, "enable/disable verbosity while setup")
	, param_argc("argc", this, argc, "Number of program arguments")
	, param_stdin_pipe_filename("stdin-pipe-filename", this, stdin_pipe_filename, "stdin pipe filename")
	, param_stdout_pipe_filename("stdout-pipe-filename", this, stdout_pipe_filename, "stdout pipe filename")
	, param_stderr_pipe_filename("stderr-pipe-filename", this, stderr_pipe_filename, "stderr pipe filename")
{
	param_argc.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	
	reg_params[0] = 0;
	reg_params[1] = 0;
	reg_params[2] = 0;
	reg_params[3] = 0;
	reg_params[4] = 0;

	param_register_names[0] = "r12";
	param_register_names[1] = "r11";
	param_register_names[2] = "r10";
	param_register_names[3] = "r9";
	param_register_names[4] = "r8";

	t2h_syscall_table[T2H_SYSCALL_OPEN] = &AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_open;
	t2h_syscall_table[T2H_SYSCALL_CLOSE] = &AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_close;
	t2h_syscall_table[T2H_SYSCALL_READ] = &AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_read;
	t2h_syscall_table[T2H_SYSCALL_WRITE] = &AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_write;
	t2h_syscall_table[T2H_SYSCALL_LSEEK] = &AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_lseek;
	t2h_syscall_table[T2H_SYSCALL_RENAME] = &AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_rename;
	t2h_syscall_table[T2H_SYSCALL_UNLINK] = &AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_unlink;
	t2h_syscall_table[T2H_SYSCALL_STAT] = &AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_stat;
	t2h_syscall_table[T2H_SYSCALL_FSTAT] = &AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_fstat;
	t2h_syscall_table[T2H_SYSCALL_GETTIMEOFDAY] = &AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_gettimeofday;
	t2h_syscall_table[T2H_SYSCALL_ISATTY] = &AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_isatty;
	t2h_syscall_table[T2H_SYSCALL_SYSTEM] = &AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_system;
	t2h_syscall_table[T2H_SYSCALL_EXIT] = &AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_exit;
	t2h_syscall_table[T2H_SYSCALL_INIT_ARGV] = &AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_init_argv;
}

template <class MEMORY_ADDR>
AVR32_T2H_Syscalls<MEMORY_ADDR>::~AVR32_T2H_Syscalls()
{
	std::vector<std::string *>::iterator it_argv;

	for(it_argv = argv.begin(); it_argv != argv.end(); it_argv++)
	{
		if(*it_argv) delete (*it_argv);
	}

	std::vector<Parameter<std::string> *>::iterator it_param_argv;

	for(it_param_argv = param_argv.begin(); it_param_argv != param_argv.end(); it_param_argv++)
	{
		if(*it_param_argv) delete (*it_param_argv);
	}
	
	if(stdin_pipe_fd != -1) close(stdin_pipe_fd);
	if(stdout_pipe_fd != -1) close(stdout_pipe_fd);
	if(stderr_pipe_fd != -1) close(stderr_pipe_fd);
}

template <class MEMORY_ADDR>
void AVR32_T2H_Syscalls<MEMORY_ADDR>::OnDisconnect()
{
}

template <class MEMORY_ADDR>
bool AVR32_T2H_Syscalls<MEMORY_ADDR>::EndSetup()
{
	if(!memory_injection_import)
	{
		logger << DebugError << memory_injection_import.GetName() << " is not connected" << EndDebugError;
		return false;
	}

	if(!registers_import)
	{
		logger << DebugError << registers_import.GetName() << " is not connected" << EndDebugError;
		return false;
	}
	
	if(!blob_import)
	{
		logger << DebugError << blob_import.GetName() << " is not connected" << EndDebugError;
		return false;
	}

	reg_pc = registers_import->GetRegister(pc_register_name.c_str());

	if(!reg_pc)
	{
		logger << DebugError << "Undefined register " << pc_register_name << "." << EndDebugError;
		return false;
	}

	if(reg_pc->GetSize() != 4)
	{
		logger << DebugError << "Register " << pc_register_name << " is not a 32-bit register." << EndDebugError;
		return false;
	}

	reg_sp = registers_import->GetRegister(sp_register_name.c_str());

	if(!reg_sp)
	{
		logger << DebugError << "Undefined register " << sp_register_name << "." << EndDebugError;
		return false;
	}

	if(reg_sp->GetSize() != 4)
	{
		logger << DebugError << "Register " << sp_register_name << " is not a 32-bit register." << EndDebugError;
		return false;
	}
	
	reg_syscall_num = registers_import->GetRegister(syscall_num_register_name.c_str());

	if(!reg_syscall_num)
	{
		logger << DebugError << "Undefined register " << syscall_num_register_name << "." << EndDebugError;
		return false;
	}

	if(reg_syscall_num->GetSize() != 4)
	{
		logger << DebugError << "Register " << syscall_num_register_name << " is not a 32-bit register." << EndDebugError;
		return false;
	}

	for(unsigned int i = 0; i < 5; i++)
	{
		reg_params[i] = registers_import->GetRegister(param_register_names[i].c_str());
		if(!reg_params[i])
		{
			logger << DebugError << "Undefined register " << param_register_names[i] << "." << EndDebugError;
			return false;
		}

		if(reg_params[i]->GetSize() != 4)
		{
			logger << DebugError << "Register " << param_register_names[i] << " is not a 32-bit register." << EndDebugError;
			return false;
		}
	}

	reg_return_status = registers_import->GetRegister(return_status_register_name.c_str());

	if(!reg_return_status)
	{
		logger << DebugError << "Undefined register " << return_status_register_name << "." << EndDebugError;
		return false;
	}

	if(reg_return_status->GetSize() != 4)
	{
		logger << DebugError << "Register " << return_status_register_name << " is not a 32-bit register." << EndDebugError;
		return false;
	}

	reg_errno = registers_import->GetRegister(errno_register_name.c_str());

	if(!reg_errno)
	{
		logger << DebugError << "Undefined register " << errno_register_name << "." << EndDebugError;
		return false;
	}

	if(reg_errno->GetSize() != 4)
	{
		logger << DebugError << "Register " << errno_register_name << " is not a 32-bit register." << EndDebugError;
		return false;
	}

	for(int i = 0; i < argc; i++)
	{
		std::string *arg = new std::string();
		argv.push_back(arg);
		std::stringstream sstr_param_argv_name;
		sstr_param_argv_name << "argv[" << i << "]";
		param_argv.push_back(new Parameter<std::string>(sstr_param_argv_name.str().c_str(), this, *arg, "program argument"));
	}
	
	
	const unisim::util::blob::Blob<MEMORY_ADDR> *blob = blob_import->GetBlob();
	MEMORY_ADDR entry_point = blob->GetEntryPoint();
	reg_pc->SetValue(&entry_point);

	if(!stdin_pipe_filename.empty())
	{
		int stdin_pipe_flags = O_RDONLY;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		stdin_pipe_flags |= O_BINARY;
#endif
		stdin_pipe_fd = open(stdin_pipe_filename.c_str(), stdin_pipe_flags);
		if(stdin_pipe_fd == -1)
		{
			logger << DebugError << "Can't open \"" << stdin_pipe_filename << "\"" << EndDebugError;
			return false;
		}
	}
	
	
	if(!stdout_pipe_filename.empty())
	{
		int stdout_pipe_flags = O_WRONLY | O_CREAT | O_TRUNC;
		mode_t stdout_pipe_mode = S_IRUSR | S_IWUSR;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		stdout_pipe_flags |= O_BINARY;
#else
		stdout_pipe_mode |= S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
#endif
		stdout_pipe_fd = open(stdout_pipe_filename.c_str(), stdout_pipe_flags, stdout_pipe_mode);
		if(stdout_pipe_fd == -1)
		{
			logger << DebugError << "Can't open \"" << stdout_pipe_filename << "\"" << EndDebugError;
			return false;
		}
	}
	
	if(!stderr_pipe_filename.empty())
	{
		int stderr_pipe_flags = O_WRONLY | O_CREAT | O_TRUNC;
		mode_t stderr_pipe_mode = S_IRUSR | S_IWUSR;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		stderr_pipe_flags |= O_BINARY;
#else
		stderr_pipe_mode |= S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
#endif
		stderr_pipe_fd = open(stderr_pipe_filename.c_str(), stderr_pipe_flags, stderr_pipe_mode);
		if(stderr_pipe_fd == -1)
		{
			logger << DebugError << "Can't open \"" << stderr_pipe_filename << "\"" << EndDebugError;
			return false;
		}
	}

	MapTargetToHostFileDescriptor(0, (stdin_pipe_fd == -1) ? 0 : stdin_pipe_fd); // map target stdin file descriptor to either host stdin file descriptor or host input file descriptor
	MapTargetToHostFileDescriptor(1, (stdout_pipe_fd == -1) ? 1 : stdout_pipe_fd); // map target stdout file descriptor to either host stdout file descriptor or host output file descriptor
	MapTargetToHostFileDescriptor(2, (stderr_pipe_fd == -1) ? 2 : stderr_pipe_fd); // map target stdout file descriptor to either host stderr file descriptor or host output file descriptor

	return true;
}

template <class MEMORY_ADDR>
const char *AVR32_T2H_Syscalls<MEMORY_ADDR>::GetSyscallFriendlyName(uint32_t command)
{
	switch(command)
	{
		case T2H_SYSCALL_OPEN: return "open";
		case T2H_SYSCALL_CLOSE: return "close";
		case T2H_SYSCALL_READ: return "read";
		case T2H_SYSCALL_WRITE: return "write";
		case T2H_SYSCALL_LSEEK: return "lseek";
		case T2H_SYSCALL_RENAME: return "rename";
		case T2H_SYSCALL_UNLINK: return "unlink";
		case T2H_SYSCALL_STAT: return "stat";
		case T2H_SYSCALL_FSTAT: return "fstat";
		case T2H_SYSCALL_GETTIMEOFDAY: return "gettimeofday";
		case T2H_SYSCALL_ISATTY: return "isatty";
		case T2H_SYSCALL_SYSTEM: return "system";
		case T2H_SYSCALL_EXIT: return "exit";
		case T2H_SYSCALL_INIT_ARGV: return "init_argv";
	}
	return "?";
}

template <class MEMORY_ADDR>
uint32_t AVR32_T2H_Syscalls<MEMORY_ADDR>::GetSystemCallParam(unsigned int arg_num)
{
	uint32_t value = 0;
	reg_params[arg_num]->GetValue(&value);
	
	return value;
}

template <class MEMORY_ADDR>
void AVR32_T2H_Syscalls<MEMORY_ADDR>::SetSystemCallParam(unsigned int arg_num, uint32_t value)
{
	reg_params[arg_num]->SetValue(&value);
}

template <class MEMORY_ADDR>
void AVR32_T2H_Syscalls<MEMORY_ADDR>::SetSystemCallStatus(int32_t ret)
{
	reg_return_status->SetValue(&ret);
}

template <class MEMORY_ADDR>
void AVR32_T2H_Syscalls<MEMORY_ADDR>::SetErrno(int32_t target_errno)
{
	reg_errno->SetValue(&target_errno);
}

template <class MEMORY_ADDR>
int32_t AVR32_T2H_Syscalls<MEMORY_ADDR>::Host2TargetErrno(int host_errno)
{
	switch(host_errno)
	{
#ifdef EPERM
		case EPERM: return T2H_EPERM;
#endif
#ifdef ENOENT
		case ENOENT: return T2H_ENOENT;
#endif
#ifdef ESRCH
		case ESRCH: return T2H_ESRCH;
#endif
#ifdef EINTR
		case EINTR: return T2H_EINTR;
#endif
#ifdef EIO
		case EIO: return T2H_EIO;
#endif
#ifdef ENXIO
		case ENXIO: return T2H_ENXIO;
#endif
#ifdef E2BIG
		case E2BIG: return T2H_E2BIG;
#endif
#ifdef ENOEXEC
		case ENOEXEC: return T2H_ENOEXEC;
#endif
#ifdef EBADF
		case EBADF: return T2H_EBADF;
#endif
#ifdef ECHILD
		case ECHILD: return T2H_ECHILD;
#endif
#ifdef EAGAIN
		case EAGAIN: return T2H_EAGAIN;
#endif
#ifdef ENOMEM
		case ENOMEM: return T2H_ENOMEM;
#endif
#ifdef EACCES
		case EACCES: return T2H_EACCES;
#endif
#ifdef EFAULT
		case EFAULT: return T2H_EFAULT;
#endif
#ifdef ENOTBLK
		case ENOTBLK: return T2H_ENOTBLK;
#endif
#ifdef EBUSY
		case EBUSY: return T2H_EBUSY;
#endif
#ifdef EEXIST
		case EEXIST: return T2H_EEXIST;
#endif
#ifdef EXDEV
		case EXDEV: return T2H_EXDEV;
#endif
#ifdef ENODEV
		case ENODEV: return T2H_ENODEV;
#endif
#ifdef ENOTDIR
		case ENOTDIR: return T2H_ENOTDIR;
#endif
#ifdef EISDIR
		case EISDIR: return T2H_EISDIR;
#endif
#ifdef EINVAL
		case EINVAL: return T2H_EINVAL;
#endif
#ifdef ENFILE
		case ENFILE: return T2H_ENFILE;
#endif
#ifdef EMFILE
		case EMFILE: return T2H_EMFILE;
#endif
#ifdef ENOTTY
		case ENOTTY: return T2H_ENOTTY;
#endif
#ifdef ETXTBSY
		case ETXTBSY: return T2H_ETXTBSY;
#endif
#ifdef EFBIG
		case EFBIG: return T2H_EFBIG;
#endif
#ifdef ENOSPC
		case ENOSPC: return T2H_ENOSPC;
#endif
#ifdef ESPIPE
		case ESPIPE: return T2H_ESPIPE;
#endif
#ifdef EROFS
		case EROFS: return T2H_EROFS;
#endif
#ifdef EMLINK
		case EMLINK: return T2H_EMLINK;
#endif
#ifdef EPIPE
		case EPIPE: return T2H_EPIPE;
#endif
#ifdef EDOM
		case EDOM: return T2H_EDOM;
#endif
#ifdef ERANGE
		case ERANGE: return T2H_ERANGE;
#endif
#ifdef EDEADLK
		case EDEADLK: return T2H_EDEADLK;
#endif
#ifdef ENAMETOOLONG
		case ENAMETOOLONG: return T2H_ENAMETOOLONG;
#endif
#ifdef ENOLCK
		case ENOLCK: return T2H_ENOLCK;
#endif
#ifdef ENOSYS
		case ENOSYS: return T2H_ENOSYS;
#endif
#ifdef ENOTEMPTY
		case ENOTEMPTY: return T2H_ENOTEMPTY;
#endif
#ifdef ELOOP
		case ELOOP: return T2H_ELOOP;
#endif
#ifdef ENOMSG
		case ENOMSG: return T2H_ENOMSG;
#endif
#ifdef EIDRM
		case EIDRM: return T2H_EIDRM;
#endif
#ifdef ECHRNG
		case ECHRNG: return T2H_ECHRNG;
#endif
#ifdef EL2NSYNC
		case EL2NSYNC: return T2H_EL2NSYNC;
#endif
#ifdef EL3HLT
		case EL3HLT: return T2H_EL3HLT;
#endif
#ifdef EL3RST
		case EL3RST: return T2H_EL3RST;
#endif
#ifdef ELNRNG
		case ELNRNG: return T2H_ELNRNG;
#endif
#ifdef EUNATCH
		case EUNATCH: return T2H_EUNATCH;
#endif
#ifdef ENOCSI
		case ENOCSI: return T2H_ENOCSI;
#endif
#ifdef EL2HLT
		case EL2HLT: return T2H_EL2HLT;
#endif
#ifdef EBADE
		case EBADE: return T2H_EBADE;
#endif
#ifdef EBADR
		case EBADR: return T2H_EBADR;
#endif
#ifdef EXFULL
		case EXFULL: return T2H_EXFULL;
#endif
#ifdef ENOANO
		case ENOANO: return T2H_ENOANO;
#endif
#ifdef EBADRQC
		case EBADRQC: return T2H_EBADRQC;
#endif
#ifdef EBADSLT
		case EBADSLT: return T2H_EBADSLT;
#endif
#ifdef EBFONT
		case EBFONT: return T2H_EBFONT;
#endif
#ifdef ENOSTR
		case ENOSTR: return T2H_ENOSTR;
#endif
#ifdef ENODATA
		case ENODATA: return T2H_ENODATA;
#endif
#ifdef ETIME
		case ETIME: return T2H_ETIME;
#endif
#ifdef ENOSR
		case ENOSR: return T2H_ENOSR;
#endif
#ifdef ENONET
		case ENONET: return T2H_ENONET;
#endif
#ifdef ENOPKG
		case ENOPKG: return T2H_ENOPKG;
#endif
#ifdef EREMOTE
		case EREMOTE: return T2H_EREMOTE;
#endif
#ifdef ENOLINK
		case ENOLINK: return T2H_ENOLINK;
#endif
#ifdef EADV
		case EADV: return T2H_EADV;
#endif
#ifdef ESRMNT
		case ESRMNT: return T2H_ESRMNT;
#endif
#ifdef ECOMM
		case ECOMM: return T2H_ECOMM;
#endif
#ifdef EPROTO
		case EPROTO: return T2H_EPROTO;
#endif
#ifdef EMULTIHOP
		case EMULTIHOP: return T2H_EMULTIHOP;
#endif
#ifdef EDOTDOT
		case EDOTDOT: return T2H_EDOTDOT;
#endif
#ifdef EBADMSG
		case EBADMSG: return T2H_EBADMSG;
#endif
#ifdef EOVERFLOW
		case EOVERFLOW: return T2H_EOVERFLOW;
#endif
#ifdef ENOTUNIQ
		case ENOTUNIQ: return T2H_ENOTUNIQ;
#endif
#ifdef EBADFD
		case EBADFD: return T2H_EBADFD;
#endif
#ifdef EREMCHG
		case EREMCHG: return T2H_EREMCHG;
#endif
#ifdef ELIBACC
		case ELIBACC: return T2H_ELIBACC;
#endif
#ifdef ELIBBAD
		case ELIBBAD: return T2H_ELIBBAD;
#endif
#ifdef ELIBSCN
		case ELIBSCN: return T2H_ELIBSCN;
#endif
#ifdef ELIBMAX
		case ELIBMAX: return T2H_ELIBMAX;
#endif
#ifdef ELIBEXEC
		case ELIBEXEC: return T2H_ELIBEXEC;
#endif
#ifdef EILSEQ
		case EILSEQ: return T2H_EILSEQ;
#endif
#ifdef EUSERS
		case EUSERS: return T2H_EUSERS;
#endif
#ifdef ENOTSOCK
		case ENOTSOCK: return T2H_ENOTSOCK;
#endif
#ifdef EDESTADDRREQ
		case EDESTADDRREQ: return T2H_EDESTADDRREQ;
#endif
#ifdef EMSGSIZE
		case EMSGSIZE: return T2H_EMSGSIZE;
#endif
#ifdef EPROTOTYPE
		case EPROTOTYPE: return T2H_EPROTOTYPE;
#endif
#ifdef ENOPROTOOPT
		case ENOPROTOOPT: return T2H_ENOPROTOOPT;
#endif
#ifdef EPROTONOSUPPORT
		case EPROTONOSUPPORT: return T2H_EPROTONOSUPPORT;
#endif
#ifdef ESOCKTNOSUPPORT
		case ESOCKTNOSUPPORT: return T2H_ESOCKTNOSUPPORT;
#endif
#ifdef EOPNOTSUPP
		case EOPNOTSUPP: return T2H_EOPNOTSUPP;
#endif
#ifdef EPFNOSUPPORT
		case EPFNOSUPPORT: return T2H_EPFNOSUPPORT;
#endif
#ifdef EAFNOSUPPORT
		case EAFNOSUPPORT: return T2H_EAFNOSUPPORT;
#endif
#ifdef EADDRINUSE
		case EADDRINUSE: return T2H_EADDRINUSE;
#endif
#ifdef EADDRNOTAVAIL
		case EADDRNOTAVAIL: return T2H_EADDRNOTAVAIL;
#endif
#ifdef ENETDOWN
		case ENETDOWN: return T2H_ENETDOWN;
#endif
#ifdef ENETUNREACH
		case ENETUNREACH: return T2H_ENETUNREACH;
#endif
#ifdef ENETRESET
		case ENETRESET: return T2H_ENETRESET;
#endif
#ifdef ECONNABORTED
		case ECONNABORTED: return T2H_ECONNABORTED;
#endif
#ifdef ECONNRESET
		case ECONNRESET: return T2H_ECONNRESET;
#endif
#ifdef ENOBUFS
		case ENOBUFS: return T2H_ENOBUFS;
#endif
#ifdef EISCONN
		case EISCONN: return T2H_EISCONN;
#endif
#ifdef ENOTCONN
		case ENOTCONN: return T2H_ENOTCONN;
#endif
#ifdef ESHUTDOWN
		case ESHUTDOWN: return T2H_ESHUTDOWN;
#endif
#ifdef ETOOMANYREFS
		case ETOOMANYREFS: return T2H_ETOOMANYREFS;
#endif
#ifdef ETIMEDOUT
		case ETIMEDOUT: return T2H_ETIMEDOUT;
#endif
#ifdef ECONNREFUSED
		case ECONNREFUSED: return T2H_ECONNREFUSED;
#endif
#ifdef EHOSTDOWN
		case EHOSTDOWN: return T2H_EHOSTDOWN;
#endif
#ifdef EHOSTUNREACH
		case EHOSTUNREACH: return T2H_EHOSTUNREACH;
#endif
#ifdef EALREADY
		case EALREADY: return T2H_EALREADY;
#endif
#ifdef EINPROGRESS
		case EINPROGRESS: return T2H_EINPROGRESS;
#endif
#ifdef ESTALE
		case ESTALE: return T2H_ESTALE;
#endif
#ifdef EDQUOT
		case EDQUOT: return T2H_EDQUOT;
#endif
#ifdef ENOMEDIUM
		case ENOMEDIUM: return T2H_ENOMEDIUM;
#endif
#ifdef ECANCELED
		case ECANCELED: return T2H_ECANCELED;
#endif
	}
	
	logger << DebugWarning << "Don't how to convert host errno #" << errno << " to target...Silently setting errno to EINVAL." << EndDebugWarning;
	
	return T2H_EINVAL;
}

template <class MEMORY_ADDR>
int AVR32_T2H_Syscalls<MEMORY_ADDR>::Target2HostFileDescriptor(int32_t fd)
{
	// Return an error if file descriptor does not exist
	typename std::map<int32_t, int>::iterator iter = target_to_host_fildes.find(fd);
	if(iter == target_to_host_fildes.end()) return -1;

	// Translate the target file descriptor to an host file descriptor
	return (*iter).second;
}

template <class MEMORY_ADDR>
int32_t AVR32_T2H_Syscalls<MEMORY_ADDR>::AllocateFileDescriptor()
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

template <class MEMORY_ADDR>
void AVR32_T2H_Syscalls<MEMORY_ADDR>::FreeFileDescriptor(int32_t fd)
{
	target_fildes_free_list.push(fd);
}

template <class MEMORY_ADDR>
void AVR32_T2H_Syscalls<MEMORY_ADDR>::MapTargetToHostFileDescriptor(int32_t target_fd, int host_fd)
{
	target_to_host_fildes.insert(std::pair<int32_t, int>(target_fd, host_fd));
}

template <class MEMORY_ADDR>
void AVR32_T2H_Syscalls<MEMORY_ADDR>::UnmapTargetToHostFileDescriptor(int32_t target_fd)
{
	std::map<int32_t, int>::iterator iter = target_to_host_fildes.find(target_fd);
	
	if(iter != target_to_host_fildes.end())
	{
		target_to_host_fildes.erase(iter);
	}
}

template <class MEMORY_ADDR>
bool AVR32_T2H_Syscalls<MEMORY_ADDR>::StringLength(MEMORY_ADDR addr, int &string_length)
{
	string_length = 0;
	char c;

	while(1)
	{
		c = 0;
		if(!memory_injection_import->InjectReadMemory(addr, &c, 1)) return false;
		if(c == 0) break;
		string_length++;
		addr++;
	}
	
	return true;
}

template <class MEMORY_ADDR>
int AVR32_T2H_Syscalls<MEMORY_ADDR>::Fstat(int fd, struct avr32_stat *target_stat)
{
	int ret;
	struct stat host_stat;
	ret = fstat(fd, &host_stat);
	if(ret < 0) return ret;

#if defined(__x86_64) || defined(__amd64) || defined(__LP64__) || defined(_LP64) || defined(__amd64__)
	// 64-bit host
	target_stat->st_dev = Host2BigEndian((uint64_t) host_stat.st_dev);
	target_stat->st_ino = Host2BigEndian((uint64_t) host_stat.st_ino);
	target_stat->st_mode = Host2BigEndian((uint32_t) host_stat.st_mode);
	target_stat->st_nlink = Host2BigEndian((uint64_t) host_stat.st_nlink);
	target_stat->st_uid = Host2BigEndian((uint32_t) host_stat.st_uid);
	target_stat->st_gid = Host2BigEndian((uint32_t) host_stat.st_gid);
	target_stat->st_rdev = Host2BigEndian((int64_t) host_stat.st_rdev);
	target_stat->st_size = Host2BigEndian((int64_t) host_stat.st_size);
#if defined(WIN64) || defined(_WIN64) // Windows x64
	target_stat->st_blksize = Host2BigEndian((int32_t) 512);
	target_stat->st_blocks = Host2BigEndian((int64_t)((host_stat.st_size + 511) / 512));
	target_stat->st_atim = Host2BigEndian((int64_t) host_stat.st_atime);
	target_stat->st_mtim = Host2BigEndian((int64_t) host_stat.st_mtime);
	target_stat->st_ctim = Host2BigEndian((int64_t) host_stat.st_ctime);
#elif defined(linux) || defined(__linux) || defined(__linux__) // Linux x64
	target_stat->st_blksize = Host2BigEndian((int64_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2BigEndian((int64_t) host_stat.st_blocks);
	target_stat->st_atim = Host2BigEndian((int64_t) host_stat.st_atim.tv_sec);
	target_stat->st_mtim = Host2BigEndian((int64_t) host_stat.st_mtim.tv_sec);
	target_stat->st_ctim = Host2BigEndian((int64_t) host_stat.st_ctim.tv_sec);
#elif defined(__APPLE_CC__) // darwin PPC64/x86_64
	target_stat->st_blksize = Host2BigEndian((int64_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2BigEndian((int64_t) host_stat.st_blocks);
	target_stat->st_atim = Host2BigEndian((int64_t) host_stat.st_atimespec.tv_sec);
	target_stat->st_mtim = Host2BigEndian((int64_t) host_stat.st_mtimespec.tv_sec);
	target_stat->st_ctim = Host2BigEndian((int64_t) host_stat.st_ctimespec.tv_sec);
#endif

#else
	// 32-bit host
	target_stat->st_dev = Host2BigEndian((uint64_t) host_stat.st_dev);
	target_stat->st_ino = Host2BigEndian((uint32_t) host_stat.st_ino);
	target_stat->st_mode = Host2BigEndian((uint32_t) host_stat.st_mode);
	target_stat->st_nlink = Host2BigEndian((uint32_t) host_stat.st_nlink);
	target_stat->st_uid = Host2BigEndian((uint32_t) host_stat.st_uid);
	target_stat->st_gid = Host2BigEndian((uint32_t) host_stat.st_gid);
	target_stat->st_rdev = Host2BigEndian((int64_t) host_stat.st_rdev);
	target_stat->st_size = Host2BigEndian((int64_t) host_stat.st_size);
#if defined(WIN32) || defined(_WIN32) // Windows 32
	target_stat->st_blksize = Host2BigEndian((int32_t) 512);
	target_stat->st_blocks = Host2BigEndian((int64_t)((host_stat.st_size + 511) / 512));
	target_stat->st_atim = Host2BigEndian((int32_t) host_stat.st_atime);
	target_stat->st_mtim = Host2BigEndian((int32_t) host_stat.st_mtime);
	target_stat->st_ctim = Host2BigEndian((int32_t) host_stat.st_ctime);
#elif defined(linux) || defined(__linux) || defined(__linux__) // Linux 32
	target_stat->st_blksize = Host2BigEndian((int32_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2BigEndian((int64_t) host_stat.st_blocks);
	target_stat->st_atim = Host2BigEndian((int32_t) host_stat.st_atim.tv_sec);
	target_stat->st_mtim = Host2BigEndian((int32_t) host_stat.st_mtim.tv_sec);
	target_stat->st_ctim = Host2BigEndian((int32_t) host_stat.st_ctim.tv_sec);
#elif defined(__APPLE_CC__) // Darwin PPC32/x86
	target_stat->st_blksize = Host2BigEndian((int32_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2BigEndian((int64_t) host_stat.st_blocks);
	target_stat->st_atim = Host2BigEndian((int32_t) host_stat.st_atimespec.tv_sec);
	target_stat->st_mtim = Host2BigEndian((int32_t) host_stat.st_mtimespec.tv_sec);
	target_stat->st_ctim = Host2BigEndian((int32_t) host_stat.st_ctimespec.tv_sec);
#endif

#endif
	return ret;
}

template <class MEMORY_ADDR>
int AVR32_T2H_Syscalls<MEMORY_ADDR>::Stat(const char *path, struct avr32_stat *target_stat)
{
	int ret;
	struct stat host_stat;
	ret = stat(path, &host_stat);
	if(ret < 0) return ret;

#if defined(__x86_64) || defined(__amd64) || defined(__LP64__) || defined(_LP64) || defined(__amd64__)
	// 64-bit host
	target_stat->st_dev = Host2BigEndian((uint64_t) host_stat.st_dev);
	target_stat->st_ino = Host2BigEndian((uint64_t) host_stat.st_ino);
	target_stat->st_mode = Host2BigEndian((uint32_t) host_stat.st_mode);
	target_stat->st_nlink = Host2BigEndian((uint64_t) host_stat.st_nlink);
	target_stat->st_uid = Host2BigEndian((uint32_t) host_stat.st_uid);
	target_stat->st_gid = Host2BigEndian((uint32_t) host_stat.st_gid);
	target_stat->st_rdev = Host2BigEndian((int64_t) host_stat.st_rdev);
	target_stat->st_size = Host2BigEndian((int64_t) host_stat.st_size);
#if defined(WIN64) || defined(_WIN64) // Windows x64
	target_stat->st_blksize = Host2BigEndian((int32_t) 512);
	target_stat->st_blocks = Host2BigEndian((int64_t)((host_stat.st_size + 511) / 512));
	target_stat->st_atim = Host2BigEndian((int64_t) host_stat.st_atime);
	target_stat->st_mtim = Host2BigEndian((int64_t) host_stat.st_mtime);
	target_stat->st_ctim = Host2BigEndian((int64_t) host_stat.st_ctime);
#elif defined(linux) || defined(__linux) || defined(__linux__) // Linux x64
	target_stat->st_blksize = Host2BigEndian((int64_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2BigEndian((int64_t) host_stat.st_blocks);
	target_stat->st_atim = Host2BigEndian((int64_t) host_stat.st_atim.tv_sec);
	target_stat->st_mtim = Host2BigEndian((int64_t) host_stat.st_mtim.tv_sec);
	target_stat->st_ctim = Host2BigEndian((int64_t) host_stat.st_ctim.tv_sec);
#elif defined(__APPLE_CC__) // darwin PPC64/x86_64
	target_stat->st_blksize = Host2BigEndian((int64_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2BigEndian((int64_t) host_stat.st_blocks);
	target_stat->st_atim = Host2BigEndian((int64_t) host_stat.st_atimespec.tv_sec);
	target_stat->st_mtim = Host2BigEndian((int64_t) host_stat.st_mtimespec.tv_sec);
	target_stat->st_ctim = Host2BigEndian((int64_t) host_stat.st_ctimespec.tv_sec);
#endif

#else
	// 32-bit host
	target_stat->st_dev = Host2BigEndian((uint64_t) host_stat.st_dev);
	target_stat->st_ino = Host2BigEndian((uint32_t) host_stat.st_ino);
	target_stat->st_mode = Host2BigEndian((uint32_t) host_stat.st_mode);
	target_stat->st_nlink = Host2BigEndian((uint32_t) host_stat.st_nlink);
	target_stat->st_uid = Host2BigEndian((uint32_t) host_stat.st_uid);
	target_stat->st_gid = Host2BigEndian((uint32_t) host_stat.st_gid);
	target_stat->st_rdev = Host2BigEndian((int64_t) host_stat.st_rdev);
	target_stat->st_size = Host2BigEndian((int64_t) host_stat.st_size);
#if defined(WIN32) || defined(_WIN32) // Windows 32
	target_stat->st_blksize = Host2BigEndian((int32_t) 512);
	target_stat->st_blocks = Host2BigEndian((int64_t)((host_stat.st_size + 511) / 512));
	target_stat->st_atim = Host2BigEndian((int32_t) host_stat.st_atime);
	target_stat->st_mtim = Host2BigEndian((int32_t) host_stat.st_mtime);
	target_stat->st_ctim = Host2BigEndian((int32_t) host_stat.st_ctime);
#elif defined(linux) || defined(__linux) || defined(__linux__) // Linux 32
	target_stat->st_blksize = Host2BigEndian((int32_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2BigEndian((int64_t) host_stat.st_blocks);
	target_stat->st_atim = Host2BigEndian((int32_t) host_stat.st_atim.tv_sec);
	target_stat->st_mtim = Host2BigEndian((int32_t) host_stat.st_mtim.tv_sec);
	target_stat->st_ctim = Host2BigEndian((int32_t) host_stat.st_ctim.tv_sec);
#elif defined(__APPLE_CC__) // Darwin PPC32/x86
	target_stat->st_blksize = Host2BigEndian((int32_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2BigEndian((int64_t) host_stat.st_blocks);
	target_stat->st_atim = Host2BigEndian((int32_t) host_stat.st_atimespec.tv_sec);
	target_stat->st_mtim = Host2BigEndian((int32_t) host_stat.st_mtimespec.tv_sec);
	target_stat->st_ctim = Host2BigEndian((int32_t) host_stat.st_ctimespec.tv_sec);
#endif

#endif
	return ret;
}

template <class MEMORY_ADDR>
int AVR32_T2H_Syscalls<MEMORY_ADDR>::GetTimeOfDay(struct avr32_timeval *target_timeval, struct avr32_timezone *target_timezone)
{
	int ret;
	struct timeval host_tv;
	struct timezone host_tz;

	ret = gettimeofday(&host_tv, &host_tz);

	if(ret == 0)
	{
		if(target_timeval)
		{
			target_timeval->tv_sec = Host2BigEndian((int32_t) host_tv.tv_sec);
			target_timeval->tv_usec = Host2BigEndian((int32_t) host_tv.tv_usec);
		}
		if(target_timezone)
		{
			target_timezone->tz_minuteswest = Host2BigEndian((int32_t) host_tz.tz_minuteswest);
			target_timezone->tz_dsttime = Host2BigEndian((int32_t) host_tz.tz_dsttime);
		}
	}
	return ret;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::AVR32_T2H_Syscalls::Status AVR32_T2H_Syscalls<MEMORY_ADDR>::HandleEmulatorBreakpoint()
{
	// read PC
	uint32_t pc_value = 0;
	reg_pc->GetValue(&pc_value);

	// read a 6-byte pattern at PC
	uint8_t pattern[6];
	if(!memory_injection_import->InjectReadMemory(pc_value, pattern, 6))
	{
		return unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_ERROR;
	}
	
	// try to recognize pattern breakpoint/mov r12,-1/mov r11,...
	if((pattern[0] != 0xd6) || (pattern[1] != 0x73) || // breakpoint
	   (pattern[2] != 0x3f) || (pattern[3] != 0xfc) || // mov r12,-1
	   ((pattern[4] & 0xf0) != 0x30) || ((pattern[5] & 0x0f) != 0x0b)) // mov r11,...
	{
		return unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_UNHANDLED;
	}

	uint32_t syscall_num;
	reg_syscall_num->GetValue(&syscall_num);
	
	if((syscall_num >= T2H_SYSCALL_OPEN) && (syscall_num <= T2H_SYSCALL_INIT_ARGV))
	{
		if((this->*t2h_syscall_table[syscall_num])() == unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_ERROR)
		{
			logger << DebugWarning << "System call #" << syscall_num << " (" << GetSyscallFriendlyName(syscall_num) << ")" << EndDebugWarning;
			return unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_ERROR;
		}
	}
	else
	{
		logger << DebugWarning << "System call #" << syscall_num << " does not exist" << EndDebugWarning;
		SetSystemCallStatus(-1);
		SetErrno(ENOSYS);
	}

	// skip 2 bytes of breakpoint instruction plus next 4 bytes
	pc_value = pc_value + 6;
	reg_pc->SetValue(&pc_value);
	
	return unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_OK;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::AVR32_T2H_Syscalls::Status AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_open()
{
	MEMORY_ADDR addr;
	int pathnamelen;
	char *pathname;
	int flags;
	int ret;
	mode_t mode;
	int32_t target_errno = 0;

	addr = GetSystemCallParam(0);
	flags = GetSystemCallParam(1);
	mode = GetSystemCallParam(2);
	
	if(!StringLength(addr, pathnamelen)) return unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_ERROR;
	
	pathname = (char *) malloc(pathnamelen + 1);
	
	if(!memory_injection_import->InjectReadMemory(addr, pathname, pathnamelen + 1))
	{
		free(pathname);
		return unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_ERROR;
	}
	
	if(((strncmp(pathname, "/dev", 4) == 0) && ((pathname[4] == 0) || (pathname[4] == '/'))) ||
		((strncmp(pathname, "/proc", 5) == 0) && ((pathname[5] == 0) || (pathname[5] == '/'))) ||
		((strncmp(pathname, "/sys", 4) == 0) && ((pathname[4] == 0) || (pathname[4] == '/'))) ||
		((strncmp(pathname, "/var", 4) == 0) && ((pathname[4] == 0) || (pathname[4] == '/'))))
	{
		// deny access to /dev, /proc, /sys, /var
		target_errno = T2H_EACCES;
		ret = -1;
	}
	else
	{
		int host_flags = 0;
		int host_mode = 0;
		// open flags encoding may differ between target and host
		if((flags & T2H_O_ACCMODE) == T2H_O_RDONLY) host_flags = (host_flags & ~O_ACCMODE) | O_RDONLY;
		if((flags & T2H_O_ACCMODE) == T2H_O_WRONLY) host_flags = (host_flags & ~O_ACCMODE) | O_WRONLY;
		if((flags & T2H_O_ACCMODE) == T2H_O_RDWR) host_flags = (host_flags & ~O_ACCMODE) | O_RDWR;
		if(flags & T2H_O_CREAT) host_flags |= O_CREAT;
		if(flags & T2H_O_EXCL) host_flags |= O_EXCL;
		if(flags & T2H_O_TRUNC) host_flags |= O_TRUNC;
		if(flags & T2H_O_APPEND) host_flags |= O_APPEND;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		host_flags |= O_BINARY; // target opens file as binary files
		host_mode = mode & S_IRWXU; // Windows doesn't have bits for group and others
#else
		if(flags & T2H_O_SYNC) host_flags |= O_SYNC;
		if(flags & T2H_O_NONBLOCK) host_flags |= O_NONBLOCK;
		if(flags & T2H_O_NOCTTY) host_flags |= O_NOCTTY;
		host_mode = mode; // other UNIX systems should have the same bit encoding for protection
#endif
		ret = open(pathname, host_flags, host_mode);
	}
	
	int host_fd = ret;
	int32_t target_fd = -1;
	
	if(ret == -1)
	{
		target_errno = Host2TargetErrno(errno);
	}
	else
	{
		target_fd = AllocateFileDescriptor();
		// keep relation between the target file descriptor and the host file descriptor
		MapTargetToHostFileDescriptor(target_fd, host_fd);
	}
	
	if(unlikely(IsVerboseSyscalls()))
	{
		logger << DebugInfo
			<< "open(pathname=\"" << pathname << "\", flags=0x" << std::hex << flags
			<< ", mode=0x" << mode << std::dec << ") return " << ret
			<< EndDebugInfo;
	}

	free(pathname);
	SetSystemCallStatus(target_fd);
	if(target_fd == -1) SetErrno(target_errno);
	
	return unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_OK;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::AVR32_T2H_Syscalls::Status AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_close()
{
	int host_fd;
	int32_t target_fd;
	int ret;
	int32_t target_errno = 0;
	
	target_fd = GetSystemCallParam(0);
	
	host_fd = Target2HostFileDescriptor(target_fd);
	
	if(host_fd == -1)
	{
		target_errno = T2H_EBADF;
		ret = -1;
	}
	else
	{
		switch(host_fd)
		{
			case 0:
			case 1:
			case 2:
				UnmapTargetToHostFileDescriptor(target_fd);
				FreeFileDescriptor(target_fd);
				ret = 0;
				break;
			default:
				ret = close(host_fd);
				if(ret == 0)
				{
					UnmapTargetToHostFileDescriptor(target_fd);
					FreeFileDescriptor(target_fd);
				}
		}
		
		if(ret == -1) target_errno = Host2TargetErrno(errno);
	}
	
	if(unlikely(IsVerboseSyscalls()))
	{
		logger << DebugInfo << "close(fd=" << target_fd << ") return " << ret << EndDebugInfo;
	}
	
	SetSystemCallStatus(ret);
	if(ret == -1) SetErrno(target_errno);
	return unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_OK;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::AVR32_T2H_Syscalls::Status AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_read()
{
	int32_t target_fd;
	int host_fd;
	size_t count;
	MEMORY_ADDR buf_addr;
	void *buf;
	size_t ret;
	int32_t target_errno = 0;

	target_fd = GetSystemCallParam(0);
	buf_addr = (MEMORY_ADDR) GetSystemCallParam(1);
	count = (size_t) GetSystemCallParam(2);

	host_fd = Target2HostFileDescriptor(target_fd);
	
	if(host_fd == -1)
	{
		target_errno = T2H_EBADF;
		ret = -1;
	}
	else
	{
		buf = malloc(count);

		if(buf)
		{
			ret = read(host_fd, buf, count);
			if(ret > 0)
			{
				if(!memory_injection_import->InjectWriteMemory(buf_addr, buf, ret))
				{
					free(buf);
					return unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_ERROR;
				}
			}
			
			if(ret == (size_t) -1)
			{
				target_errno = Host2TargetErrno(errno);
			}
			free(buf);
		}
		else
		{
			target_errno = T2H_ENOMEM;
			ret = (size_t) -1;
		}
	}

	if(unlikely(IsVerboseSyscalls()))
	logger << DebugInfo << "read(fd=" << target_fd << ", buf=0x" << std::hex << buf_addr << std::dec
		<< ", count=" << count << ") return " << ret << EndDebugInfo;

	SetSystemCallStatus(ret);
	if(ret == (size_t) -1) SetErrno(target_errno);
	return unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_OK;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::AVR32_T2H_Syscalls::Status AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_write()
{
	int32_t target_fd;
	int host_fd;
	size_t count;
	void *buf;
	MEMORY_ADDR buf_addr;
	size_t ret;
	int32_t target_errno = 0;

	target_fd = GetSystemCallParam(0);
	buf_addr = GetSystemCallParam(1);
	count = (size_t)GetSystemCallParam(2);
	
	host_fd = Target2HostFileDescriptor(target_fd);
	
	if(host_fd == -1)
	{
		target_errno = T2H_EBADF;
		ret = -1;
	}
	else
	{
		buf = malloc(count);

		ret = (size_t) -1;

		if(buf)
		{
			if(!memory_injection_import->InjectReadMemory(buf_addr, buf, count))
			{
				free(buf);
				return unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_ERROR;
			}
			ret = write(host_fd, buf, count);
			
			if(ret == (size_t) -1)
			{
				target_errno = Host2TargetErrno(errno);
			}
			
			if(unlikely(IsVerboseSyscalls()))
			{
				logger << DebugInfo
				<< "write(fd=" << target_fd << ", buf=0x" << std::hex << buf_addr << std::dec
				<< ", count=" << count << ") return " << ret << std::endl
				<< "buffer =";
				for (size_t i = 0; i < count; i++)
				{
					logger << " " << std::hex << (unsigned int)((uint8_t *) buf)[i] << std::dec;
				}
				logger << EndDebugInfo;
			}
			
			free(buf);
		}
		else
		{
			target_errno = T2H_ENOMEM;
			ret = (size_t) -1;
		}
	}

	SetSystemCallStatus(ret);
	if(ret == (size_t) -1) SetErrno(target_errno);
	return unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_OK;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::AVR32_T2H_Syscalls::Status AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_lseek()
{
	int32_t target_fd;
	int host_fd;
	off_t offset;
	int whence;
	off_t ret;
	int32_t target_errno = 0;

	target_fd = GetSystemCallParam(0);
	offset = GetSystemCallParam(1);
	whence = GetSystemCallParam(2);
	
	host_fd = Target2HostFileDescriptor(target_fd);
	
	if(host_fd == -1)
	{
		target_errno = T2H_EBADF;
		ret = -1;
	}
	else
	{
		ret = lseek(host_fd, offset, whence);
		
		if(ret == -1) target_errno = Host2TargetErrno(errno);
	}
	if(unlikely(IsVerboseSyscalls()))
	{
		logger << DebugInfo << "lseek(fildes=" << target_fd << ", offset=" << offset
			<< ", whence=" << whence << ") return " << ret << EndDebugInfo;
	}

	SetSystemCallStatus(ret);
	if(ret == (off_t) -1) SetErrno(target_errno);
	
	return unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_OK;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::AVR32_T2H_Syscalls::Status AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_rename()
{
	unisim::service::interfaces::AVR32_T2H_Syscalls::Status status = unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_OK;
	MEMORY_ADDR oldpathaddr;
	MEMORY_ADDR newpathaddr;
	int oldpathlen;
	int newpathlen;
	char *oldpath;
	char *newpath;
	int ret = -1;
	int32_t target_errno = 0;

	oldpathaddr = GetSystemCallParam(0);
	newpathaddr = GetSystemCallParam(1);
	
	if(!StringLength(oldpathaddr, oldpathlen)) return unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_ERROR;
	if(!StringLength(newpathaddr, newpathlen)) return unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_ERROR;
	
	oldpath = (char *) malloc(oldpathlen + 1);
	if(memory_injection_import->InjectReadMemory(oldpathaddr, oldpath, oldpathlen + 1))
	{
		newpath = (char *) malloc(newpathlen + 1);
		if(memory_injection_import->InjectReadMemory(newpathaddr, newpath, newpathlen + 1))
		{
			ret = rename(oldpath, newpath);
			
			if(ret == -1) target_errno = Host2TargetErrno(errno);
			
			if(unlikely(IsVerboseSyscalls()))
			{
				logger << DebugInfo
					<< "rename(oldpath=\"" << oldpath << "\", newpath=\"" << newpath << "\") return " << ret << std::endl
					<< EndDebugInfo;
			}
		}
		else
		{
			status = unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_ERROR;
		}
		free(newpath);
	}
	else
	{
		status = unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_ERROR;
	}
	free(oldpath);
	
	SetSystemCallStatus(ret);
	if(ret == -1) SetErrno(target_errno);
	return status;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::AVR32_T2H_Syscalls::Status AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_unlink()
{
	MEMORY_ADDR pathnameaddr;
	int pathnamelen;
	char *pathname;
	int ret;
	int32_t target_errno = 0;

	pathnameaddr = GetSystemCallParam(0);
	if(!StringLength(pathnameaddr, pathnamelen)) return unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_ERROR;
	pathname = (char *) malloc(pathnamelen + 1);
	if(!memory_injection_import->InjectReadMemory(pathnameaddr, pathname, pathnamelen + 1))
	{
		free(pathname);
		return unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_ERROR;
	}
	ret = unlink(pathname);
	
	if(ret == -1) target_errno = Host2TargetErrno(errno);
	
	if(unlikely(IsVerboseSyscalls()))
	logger << DebugInfo
		<< "unlink(pathname=\"" << pathname << "\") return " << ret << std::endl
		<< EndDebugInfo;

	free(pathname);
	SetSystemCallStatus(ret);
	if(ret == -1) SetErrno(target_errno);
	return unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_OK;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::AVR32_T2H_Syscalls::Status AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_stat()
{
	MEMORY_ADDR addr;
	int pathnamelen;
	char *pathname;
	MEMORY_ADDR buf_address;
	int ret;
	int32_t target_errno = 0;

	addr = GetSystemCallParam(0);
	if(!StringLength(addr, pathnamelen)) return unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_ERROR;
	pathname = (char *) malloc(pathnamelen + 1);
	if(!memory_injection_import->InjectReadMemory(addr, pathname, pathnamelen + 1))
	{
		free(pathname);
		return unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_ERROR;
	}
	buf_address = GetSystemCallParam(1);
	struct avr32_stat target_stat;
	ret = Stat(pathname, &target_stat);
	if(ret == 0) memory_injection_import->InjectWriteMemory(buf_address, &target_stat, sizeof(target_stat));
	if(ret == -1) target_errno = Host2TargetErrno(errno);

	if(unlikely(IsVerboseSyscalls()))
	logger << DebugInfo
		<< "stat(pathname=\"" << pathname << "\""
		<< ", buf_addr=0x" << std::hex << buf_address << std::dec
		<< ") return " << ret << EndDebugInfo;

	free(pathname);
	SetSystemCallStatus(ret);
	if(ret == -1) SetErrno(target_errno);
	return unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_OK;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::AVR32_T2H_Syscalls::Status AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_fstat()
{
	int ret;
	int32_t target_fd;
	int host_fd;
	MEMORY_ADDR buf_address;
	int32_t target_errno = 0;

	target_fd = GetSystemCallParam(0);
	buf_address = GetSystemCallParam(1);
	
	host_fd = Target2HostFileDescriptor(target_fd);
	
	if(host_fd == -1)
	{
		target_errno = T2H_EBADF;
		ret = -1;
	}
	else
	{
		struct avr32_stat target_stat;
		ret = Fstat(target_fd, &target_stat);
		if(ret == 0)
		{
			if(!memory_injection_import->InjectWriteMemory(buf_address, &target_stat, sizeof(target_stat))) return unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_ERROR;
		}
		if(ret == -1) target_errno = Host2TargetErrno(errno);
	}

	if(unlikely(IsVerboseSyscalls()))
	logger << DebugInfo
		<< "fstat(fd=" << target_fd
		<< ", buf_addr=0x" << std::hex << buf_address << std::dec
		<< ") return " << ret << EndDebugInfo;

	SetSystemCallStatus(ret);
	if(ret == -1) SetErrno(target_errno);
	return unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_OK;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::AVR32_T2H_Syscalls::Status AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_gettimeofday()
{
	int ret = -1;
	MEMORY_ADDR tv_addr;
	MEMORY_ADDR tz_addr;
	int32_t target_errno = 0;
	
	tv_addr = GetSystemCallParam(0);
	tz_addr = GetSystemCallParam(1);

	struct avr32_timeval target_tv;
	struct avr32_timezone target_tz;

	ret = GetTimeOfDay(tv_addr ? &target_tv : 0, tz_addr ? &target_tz : 0);

	if(ret == 0)
	{
		if(tv_addr)
		{
			if(!memory_injection_import->InjectWriteMemory(tv_addr, &target_tv, sizeof(target_tv))) return unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_ERROR;
		}
		if(tz_addr)
		{
			if(!memory_injection_import->InjectWriteMemory(tz_addr, &target_tz, sizeof(target_tz))) return unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_ERROR;
		}
	}
	
	if(ret == -1) target_errno = Host2TargetErrno(errno);

	if(unlikely(IsVerboseSyscalls()))
	logger << DebugInfo
		<< "gettimeofday(tv = 0x" << std::hex << tv_addr << std::dec
		<< ", tz = 0x" << std::hex << tz_addr << std::dec << ") return " << ret
		<< EndDebugInfo;

	SetSystemCallStatus(ret);
	if(ret == -1) SetErrno(target_errno);
	return unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_OK;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::AVR32_T2H_Syscalls::Status AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_isatty()
{
	int32_t target_fd = GetSystemCallParam(0);
	int host_fd;
	int ret;
	int32_t target_errno = 0;
	
	host_fd = Target2HostFileDescriptor(target_fd);
	
	if(host_fd == -1)
	{
		target_errno = T2H_EBADF;
		ret = -1;
	}
	else
	{
		ret = isatty(target_fd);
		
		if(ret == -1) target_errno = Host2TargetErrno(errno);
	}
	
	if(unlikely(IsVerboseSyscalls()))
	logger << DebugInfo
		<< "isatty() return " << ret
		<< EndDebugInfo;

	SetSystemCallStatus(ret);
	if(ret == -1) SetErrno(target_errno);
	return unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_OK;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::AVR32_T2H_Syscalls::Status AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_system()
{
	MEMORY_ADDR addr;
	int command_len;
	char *command;
	int ret;
	int32_t target_errno = 0;

	addr = GetSystemCallParam(0);
	
	if(!StringLength(addr, command_len)) return unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_ERROR;
	command = (char *) malloc(command_len + 1);
	if(!memory_injection_import->InjectReadMemory(addr, command, command_len + 1))
	{
		free(command);
		return unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_ERROR;
	}
	
	ret = system(command);
	
	if(ret == -1)
	{
		target_errno = T2H_EINVAL;
	}
	
	if(unlikely(IsVerboseSyscalls()))
	{
		logger << DebugInfo
			<< "system(command=\"" << command << "\") return " << ret
			<< EndDebugInfo;
	}

	free(command);
	SetSystemCallStatus(ret);
	if(ret == -1) SetErrno(target_errno);
	return unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_OK;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::AVR32_T2H_Syscalls::Status AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_exit()
{
	int status;

	status = GetSystemCallParam(0);
	
	if(unlikely(IsVerboseSyscalls()))
	{
		logger << DebugInfo
			<< "exit(status=" << status << EndDebugInfo;
	}
	
	terminated = true;
	exit_status = status;

	SetSystemCallStatus(0);
	
	Object::Stop(exit_status);
	return unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_OK;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::AVR32_T2H_Syscalls::Status AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_init_argv()
{
	// Initialize argv, use stack for keeping arguments. 
	// _init_argv() returns argc in r12 and argv in r11
	// and the total size used for the arguments in r10.
	// Signal that we are storing the arguments in a stackwise top down approach (i.e. r11=0). */	
	// If initialization of argv is not handled then _init_argv
        // returns -1 so set argc to 0 and make sure no space is 
	// allocated on the stack. */

	MEMORY_ADDR sp = GetSystemCallParam(0); // high address (next byte address) of argv
	MEMORY_ADDR high_addr = sp;
	
	std::vector<MEMORY_ADDR> argv_ptr;
	int i; // this is signed for a good reason
	int n = argv.size();
	for(i = 0; i < n; i++)
	{
		const std::string& arg = *argv[i];
		sp -= arg.length() + 1;
		sp = sp & ~3; // align to a 32-bit boundary
		
		// Write argument string into memory
		if(!memory_injection_import->InjectWriteMemory(sp, arg.c_str(), arg.length() + 1)) return unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_ERROR;
		
		argv_ptr.push_back(sp); // keep string pointer
	}

	argv_ptr.push_back(0);

	for(i = n; i >= 0; i--) // n elements + null terminal pointer
	{
		MEMORY_ADDR arg_addr = Host2BigEndian(argv_ptr[i]);
		sp -= 4;
		
		// Write pointer to argument string
		if(!memory_injection_import->InjectWriteMemory(sp, &arg_addr, 4)) return unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_ERROR;
	}

	MEMORY_ADDR low_addr = sp;
	
	SetSystemCallParam(0, argc);                 // r12 = argc
	SetSystemCallParam(1, sp);                   // r11 = argv
	SetSystemCallParam(2, high_addr - low_addr); // r10 = byte size of argv + argc on the stack

	if(unlikely(IsVerboseSyscalls()))
	{
		logger << DebugInfo << "init_argv(): argc=" << argc << ", argv=0x" << std::hex << sp << std::dec << ", byte size of argv=" << high_addr - low_addr << EndDebugInfo;
	}

	return unisim::service::interfaces::AVR32_T2H_Syscalls::AVR32_T2H_SYSCALL_OK;
}

} // end of namespace avr32_t2h_syscalls
} // end of namespace os
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_OS_AVR32_T2H_SYSCALLS_AVR32_T2H_SYSCALLS_TCC__
