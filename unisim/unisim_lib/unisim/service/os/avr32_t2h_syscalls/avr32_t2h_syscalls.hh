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
 
#ifndef __UNISIM_SERVICE_OS_AVR32_T2H_SYSCALLS_AVR32_T2H_SYSCALLS_HH__
#define __UNISIM_SERVICE_OS_AVR32_T2H_SYSCALLS_AVR32_T2H_SYSCALLS_HH__

#include <inttypes.h>
#include <map>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <fstream>
#include <sstream>
#include <queue>

#include "unisim/kernel/kernel.hh"
#include <unisim/kernel/variable/variable.hh>
#include "unisim/kernel/logger/logger.hh"
#include "unisim/service/interfaces/avr32_t2h_syscalls.hh"
#include "unisim/service/interfaces/memory.hh"
#include "unisim/service/interfaces/memory_injection.hh"
#include "unisim/service/interfaces/symbol_table_lookup.hh"
#include "unisim/service/interfaces/registers.hh"
#include "unisim/service/interfaces/blob.hh"

#if defined(__GNUC__) && (__GNUC__ >= 3)
#define PACKED __attribute__ ((packed))
#else
#define PACKED
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
using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::kernel::ServiceImport;
using unisim::kernel::ServiceExport;
using unisim::kernel::variable::Parameter;
using unisim::kernel::logger::Logger;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::MemoryInjection;
using unisim::service::interfaces::SymbolTableLookup;
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::Blob;

struct avr32_stat
{
	uint32_t st_dev;      /* device */
	uint32_t st_ino;      /* inode */
	int32_t  st_mode;     /* protection */
	uint32_t st_nlink;    /* number of hard links */
	uint32_t st_uid;      /* user ID of owner */
	uint32_t st_gid;      /* group ID of owner */
	uint32_t st_rdev;     /* device type (if inode device) */
	uint64_t st_size;     /* total size, in bytes */
	uint64_t st_blksize;  /* blocksize for filesystem I/O */
	uint64_t st_blocks;   /* number of blocks allocated */
	int32_t  st_atim;     /* time of last access */
	int32_t  st_mtim;     /* time of last modification */
	int32_t  st_ctim;     /* time of last change */
} PACKED;

struct avr32_timeval
{
	int32_t tv_sec;
	int64_t tv_usec;
} PACKED;

struct avr32_timezone
{
	int32_t tz_minuteswest;
	int32_t tz_dsttime;
} PACKED;

template <class MEMORY_ADDR>
class AVR32_T2H_Syscalls :
	public Service<unisim::service::interfaces::AVR32_T2H_Syscalls>,
	public Client<MemoryInjection<MEMORY_ADDR> >,
	public Client<Registers>,
	public Client<Blob<MEMORY_ADDR> >
{
public:
	ServiceExport<unisim::service::interfaces::AVR32_T2H_Syscalls> avr32_t2h_syscalls_export;

	ServiceImport<MemoryInjection<MEMORY_ADDR> > memory_injection_import;
	ServiceImport<Registers> registers_import;
	ServiceImport<Blob<MEMORY_ADDR> > blob_import;

	AVR32_T2H_Syscalls(const char *name, Object *parent = 0);
	virtual ~AVR32_T2H_Syscalls();

	virtual void OnDisconnect();
	virtual bool EndSetup();

    virtual unisim::service::interfaces::AVR32_T2H_Syscalls::Status HandleEmulatorBreakpoint();
private:
	// AVR32's target to host syscall numbers
	static const uint32_t T2H_SYSCALL_OPEN = 1;
	static const uint32_t T2H_SYSCALL_CLOSE = 2;
	static const uint32_t T2H_SYSCALL_READ = 3;
	static const uint32_t T2H_SYSCALL_WRITE = 4;
	static const uint32_t T2H_SYSCALL_LSEEK = 5;
	static const uint32_t T2H_SYSCALL_RENAME = 6;
	static const uint32_t T2H_SYSCALL_UNLINK = 7;
	static const uint32_t T2H_SYSCALL_STAT = 8;
	static const uint32_t T2H_SYSCALL_FSTAT = 9;
	static const uint32_t T2H_SYSCALL_GETTIMEOFDAY = 10;
	static const uint32_t T2H_SYSCALL_ISATTY = 11;
	static const uint32_t T2H_SYSCALL_SYSTEM = 12;
	static const uint32_t T2H_SYSCALL_EXIT = 13;
	static const uint32_t T2H_SYSCALL_INIT_ARGV = 14;

	static const uint32_t NUM_SYSCALLS = T2H_SYSCALL_INIT_ARGV + 1;
	
	// Newlib syscall open flags
	static const uint32_t T2H_O_RDONLY   = 0x0000;
	static const uint32_t T2H_O_WRONLY   = 0x0001;
	static const uint32_t T2H_O_RDWR     = 0x0002;
	static const uint32_t T2H_O_ACCMODE  = T2H_O_RDONLY | T2H_O_WRONLY | T2H_O_RDWR;
	static const uint32_t T2H_O_APPEND   = 0x0008;
	static const uint32_t T2H_O_CREAT    = 0x0200;
	static const uint32_t T2H_O_TRUNC    = 0x0400;
	static const uint32_t T2H_O_EXCL     = 0x0800;
	static const uint32_t T2H_O_SYNC     = 0x2000;
	static const uint32_t T2H_O_NONBLOCK = 0x4000;
	static const uint32_t T2H_O_NOCTTY   = 0x8000;

	// AVR32's target Newlib errno encoding
	static const int32_t T2H_EPERM           = 1;     // Not super-user
	static const int32_t T2H_ENOENT          = 2;     // No such file or directory
	static const int32_t T2H_ESRCH           = 3;     // No such process
	static const int32_t T2H_EINTR           = 4;     // Interrupted system call
	static const int32_t T2H_EIO             = 5;     // I/O error
	static const int32_t T2H_ENXIO           = 6;     // No such device or address
	static const int32_t T2H_E2BIG           = 7;     // Arg list too long
	static const int32_t T2H_ENOEXEC         = 8;     // Exec format error
	static const int32_t T2H_EBADF           = 9;     // Bad file number
	static const int32_t T2H_ECHILD          = 10;    // No children
	static const int32_t T2H_EAGAIN          = 11;    // No more processes
	static const int32_t T2H_ENOMEM          = 12;    // Not enough core
	static const int32_t T2H_EACCES          = 13;    // Permission denied
	static const int32_t T2H_EFAULT          = 14;    // Bad address
	static const int32_t T2H_ENOTBLK         = 15;    // Block device required
	static const int32_t T2H_EBUSY           = 16;    // Mount device busy
	static const int32_t T2H_EEXIST          = 17;    // File exists
	static const int32_t T2H_EXDEV           = 18;    // Cross-device link
	static const int32_t T2H_ENODEV          = 19;    // No such device
	static const int32_t T2H_ENOTDIR         = 20;    // Not a directory
	static const int32_t T2H_EISDIR          = 21;    // Is a directory
	static const int32_t T2H_EINVAL          = 22;    // Invalid argument
	static const int32_t T2H_ENFILE          = 23;    // Too many open files in system
	static const int32_t T2H_EMFILE          = 24;    // Too many open files
	static const int32_t T2H_ENOTTY          = 25;    // Not a typewriter
	static const int32_t T2H_ETXTBSY         = 26;    // Text file busy
	static const int32_t T2H_EFBIG           = 27;    // File too large
	static const int32_t T2H_ENOSPC          = 28;    // No space left on device
	static const int32_t T2H_ESPIPE          = 29;    // Illegal seek
	static const int32_t T2H_EROFS           = 30;    // Read only file system
	static const int32_t T2H_EMLINK          = 31;    // Too many links
	static const int32_t T2H_EPIPE           = 32;    // Broken pipe
	static const int32_t T2H_EDOM            = 33;    // Math arg out of domain of func
	static const int32_t T2H_ERANGE          = 34;    // Math result not representable
	static const int32_t T2H_ENOMSG          = 35;    // No message of desired type
	static const int32_t T2H_EIDRM           = 36;    // Identifier removed
	static const int32_t T2H_ECHRNG          = 37;    // Channel number out of range
	static const int32_t T2H_EL2NSYNC        = 38;    // Level 2 not synchronized
	static const int32_t T2H_EL3HLT          = 39;    // Level 3 halted
	static const int32_t T2H_EL3RST          = 40;    // Level 3 reset
	static const int32_t T2H_ELNRNG          = 41;    // Link number out of range
	static const int32_t T2H_EUNATCH         = 42;    // Protocol driver not attached
	static const int32_t T2H_ENOCSI          = 43;    // No CSI structure available
	static const int32_t T2H_EL2HLT          = 44;    // Level 2 halted
	static const int32_t T2H_EDEADLK         = 45;    // Deadlock condition
	static const int32_t T2H_ENOLCK          = 46;    // No record locks available
	static const int32_t T2H_EBADE           = 50;    // Invalid exchange
	static const int32_t T2H_EBADR           = 51;    // Invalid request descriptor
	static const int32_t T2H_EXFULL          = 52;    // Exchange full
	static const int32_t T2H_ENOANO          = 53;    // No anode
	static const int32_t T2H_EBADRQC         = 54;    // Invalid request code
	static const int32_t T2H_EBADSLT         = 55;    // Invalid slot
	static const int32_t T2H_EDEADLOCK       = 56;    // File locking deadlock error
	static const int32_t T2H_EBFONT          = 57;    // Bad font file fmt
	static const int32_t T2H_ENOSTR          = 60;    // Device not a stream
	static const int32_t T2H_ENODATA         = 61;    // No data (for no delay io)
	static const int32_t T2H_ETIME           = 62;    // Timer expired
	static const int32_t T2H_ENOSR           = 63;    // Out of streams resources
	static const int32_t T2H_ENONET          = 64;    // Machine is not on the network
	static const int32_t T2H_ENOPKG          = 65;    // Package not installed
	static const int32_t T2H_EREMOTE         = 66;    // The object is remote
	static const int32_t T2H_ENOLINK         = 67;    // The link has been severed
	static const int32_t T2H_EADV            = 68;    // Advertise error
	static const int32_t T2H_ESRMNT          = 69;    // Srmount error
	static const int32_t T2H_ECOMM           = 70;    // Communication error on send
	static const int32_t T2H_EPROTO          = 71;    // Protocol error
	static const int32_t T2H_EMULTIHOP       = 74;    // Multihop attempted
	static const int32_t T2H_ELBIN           = 75;    // Inode is remote (not really error)
	static const int32_t T2H_EDOTDOT         = 76;    // Cross mount point (not really error)
	static const int32_t T2H_EBADMSG         = 77;    // Trying to read unreadable message
	static const int32_t T2H_EFTYPE          = 79;    // Inappropriate file type or format
	static const int32_t T2H_ENOTUNIQ        = 80;    // Given log. name not unique
	static const int32_t T2H_EBADFD          = 81;    // f.d. invalid for this operation
	static const int32_t T2H_EREMCHG         = 82;    // Remote address changed
	static const int32_t T2H_ELIBACC         = 83;    // Can't access a needed shared lib
	static const int32_t T2H_ELIBBAD         = 84;    // Accessing a corrupted shared lib
	static const int32_t T2H_ELIBSCN         = 85;    // .lib section in a.out corrupted
	static const int32_t T2H_ELIBMAX         = 86;    // Attempting to link in too many libs
	static const int32_t T2H_ELIBEXEC        = 87;    // Attempting to exec a shared library
	static const int32_t T2H_ENOSYS          = 88;    // Function not implemented
	static const int32_t T2H_ENMFILE         = 89;    // No more files
	static const int32_t T2H_ENOTEMPTY       = 90;    // Directory not empty
	static const int32_t T2H_ENAMETOOLONG    = 91;    // File or path name too long
	static const int32_t T2H_ELOOP           = 92;    // Too many symbolic links
	static const int32_t T2H_EOPNOTSUPP      = 95;    // Operation not supported on transport endpoint
	static const int32_t T2H_EPFNOSUPPORT    = 96;    // Protocol family not supported
	static const int32_t T2H_ECONNRESET      = 104;   // Connection reset by peer
	static const int32_t T2H_ENOBUFS         = 105;   // No buffer space available
	static const int32_t T2H_EAFNOSUPPORT    = 106;   // Address family not supported by protocol family
	static const int32_t T2H_EPROTOTYPE      = 107;   // Protocol wrong type for socket
	static const int32_t T2H_ENOTSOCK        = 108;   // Socket operation on non-socket
	static const int32_t T2H_ENOPROTOOPT     = 109;   // Protocol not available
	static const int32_t T2H_ESHUTDOWN       = 110;   // Can't send after socket shutdown
	static const int32_t T2H_ECONNREFUSED    = 111;   // Connection refused
	static const int32_t T2H_EADDRINUSE      = 112;   // Address already in use
	static const int32_t T2H_ECONNABORTED    = 113;   // Connection aborted
	static const int32_t T2H_ENETUNREACH     = 114;   // Network is unreachable
	static const int32_t T2H_ENETDOWN        = 115;   // Network interface is not configured
	static const int32_t T2H_ETIMEDOUT       = 116;   // Connection timed out
	static const int32_t T2H_EHOSTDOWN       = 117;   // Host is down
	static const int32_t T2H_EHOSTUNREACH    = 118;   // Host is unreachable
	static const int32_t T2H_EINPROGRESS     = 119;   // Connection already in progress
	static const int32_t T2H_EALREADY        = 120;   // Socket already connected
	static const int32_t T2H_EDESTADDRREQ    = 121;   // Destination address required
	static const int32_t T2H_EMSGSIZE        = 122;   // Message too long
	static const int32_t T2H_EPROTONOSUPPORT = 123;   // Unknown protocol
	static const int32_t T2H_ESOCKTNOSUPPORT = 124;   // Socket type not supported
	static const int32_t T2H_EADDRNOTAVAIL   = 125;   // Address not available
	static const int32_t T2H_ENETRESET       = 126;
	static const int32_t T2H_EISCONN         = 127;   // Socket is already connected
	static const int32_t T2H_ENOTCONN        = 128;   // Socket is not connected
	static const int32_t T2H_ETOOMANYREFS    = 129;
	static const int32_t T2H_EPROCLIM        = 130;
	static const int32_t T2H_EUSERS          = 131;
	static const int32_t T2H_EDQUOT          = 132;
	static const int32_t T2H_ESTALE          = 133;
	static const int32_t T2H_ENOTSUP         = 134;   // Not supported
	static const int32_t T2H_ENOMEDIUM       = 135;   // No medium (in tape drive)
	static const int32_t T2H_ENOSHARE        = 136;   // No such host or network path
	static const int32_t T2H_ECASECLASH      = 137;   // Filename exists with different case
	static const int32_t T2H_EILSEQ          = 138;
	static const int32_t T2H_EOVERFLOW       = 139;   // Value too large for defined data type
	static const int32_t T2H_ECANCELED       = 140;   // Operation canceled.
	
	// the kernel logger
	unisim::kernel::logger::Logger logger;

	// register access interfaces
	unisim::service::interfaces::Register *reg_pc;
	unisim::service::interfaces::Register *reg_sp;
	unisim::service::interfaces::Register *reg_syscall_num;
	unisim::service::interfaces::Register *reg_params[5];
	unisim::service::interfaces::Register *reg_return_status;
	unisim::service::interfaces::Register *reg_errno;

	// target to host file descriptors
	std::map<int32_t, int> target_to_host_fildes;
	std::queue<int32_t> target_fildes_free_list;
	
	// system call table
	unisim::service::interfaces::AVR32_T2H_Syscalls::Status (unisim::service::os::avr32_t2h_syscalls::AVR32_T2H_Syscalls<MEMORY_ADDR>::*t2h_syscall_table[NUM_SYSCALLS])();

	// program termination and return status
	bool terminated;
	int exit_status;

	// register names
	std::string pc_register_name;
	std::string sp_register_name;
	std::string syscall_num_register_name;
	std::string return_status_register_name;
	std::string errno_register_name;
	std::string param_register_names[5];
	
	// program arguments
	int argc;
	std::vector<std::string *> argv;
	
	// input/output pipes
	std::string stdin_pipe_filename;
	std::string stdout_pipe_filename;
	std::string stderr_pipe_filename;
	int stdin_pipe_fd;
	int stdout_pipe_fd;
	int stderr_pipe_fd;

	bool verbose_all;
	bool verbose_syscalls;
	bool verbose_setup;
	Parameter<bool> param_verbose_all;
	Parameter<bool> param_verbose_syscalls;
	Parameter<bool> param_verbose_setup;
	Parameter<int> param_argc;
	std::vector<Parameter<std::string> *> param_argv;
	Parameter<std::string> param_stdin_pipe_filename;
	Parameter<std::string> param_stdout_pipe_filename;
	Parameter<std::string> param_stderr_pipe_filename;

	
	bool IsVerboseSyscalls() const { return verbose_all || verbose_syscalls; }
	bool IsVerboseSetup() const { return verbose_all || verbose_setup; }
	
	const char *GetSyscallFriendlyName(uint32_t syscall_num);

	uint32_t GetSystemCallParam(unsigned int arg_num);
	void SetSystemCallParam(unsigned int arg_num, uint32_t value);
	bool StringLength(MEMORY_ADDR addr, int &string_length);
	int Fstat(int fd, struct avr32_stat *target_stat);
	int Stat(const char *path, struct avr32_stat *target_stat);
	int GetTimeOfDay(struct avr32_timeval *target_timeval, struct avr32_timezone *target_timezone);
	void SetSystemCallStatus(int32_t ret);
	void SetErrno(int32_t target_errno);
	int32_t Host2TargetErrno(int host_errno);
	int Target2HostFileDescriptor(int32_t fd);
	int32_t AllocateFileDescriptor();
	void FreeFileDescriptor(int32_t fd);
	void MapTargetToHostFileDescriptor(int32_t target_fd, int host_fd);
	void UnmapTargetToHostFileDescriptor(int32_t target_fd);
	
	unisim::service::interfaces::AVR32_T2H_Syscalls::Status t2h_syscall_open();
	unisim::service::interfaces::AVR32_T2H_Syscalls::Status t2h_syscall_close();
	unisim::service::interfaces::AVR32_T2H_Syscalls::Status t2h_syscall_read();
	unisim::service::interfaces::AVR32_T2H_Syscalls::Status t2h_syscall_write();
	unisim::service::interfaces::AVR32_T2H_Syscalls::Status t2h_syscall_lseek();
	unisim::service::interfaces::AVR32_T2H_Syscalls::Status t2h_syscall_rename();
	unisim::service::interfaces::AVR32_T2H_Syscalls::Status t2h_syscall_unlink();
	unisim::service::interfaces::AVR32_T2H_Syscalls::Status t2h_syscall_stat();
	unisim::service::interfaces::AVR32_T2H_Syscalls::Status t2h_syscall_fstat();
	unisim::service::interfaces::AVR32_T2H_Syscalls::Status t2h_syscall_gettimeofday();
	unisim::service::interfaces::AVR32_T2H_Syscalls::Status t2h_syscall_isatty();
	unisim::service::interfaces::AVR32_T2H_Syscalls::Status t2h_syscall_system();
	unisim::service::interfaces::AVR32_T2H_Syscalls::Status t2h_syscall_exit();
	unisim::service::interfaces::AVR32_T2H_Syscalls::Status t2h_syscall_init_argv();
};

} // end of namespace ti_c_io
} // end of namespace os
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_OS_AVR32_T2H_SYSCALLS_HH__
