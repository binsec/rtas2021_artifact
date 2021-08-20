/*
 *  Copyright (c) 2007,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#ifndef __UNISIM_SERVICE_OS_LINUX_OS_LINUX_OS_HH__
#define __UNISIM_SERVICE_OS_LINUX_OS_LINUX_OS_HH__

#include <map>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <fstream>
#include <sstream>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#include <windows.h>
#else
#include <sys/times.h>
#endif
#include "unisim/kernel/kernel.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/service/interfaces/linux_os.hh"
#include "unisim/service/interfaces/loader.hh"
#include "unisim/service/interfaces/blob.hh"
#include "unisim/service/interfaces/memory.hh"
#include "unisim/service/interfaces/memory_injection.hh"
#include "unisim/service/interfaces/registers.hh"
#include "unisim/service/os/linux_os/linux_os_exception.hh"
#include "unisim/util/endian/endian.hh"
#include "unisim/service/interfaces/register.hh"

namespace unisim {
namespace service {
namespace os {
namespace linux_os {

using std::runtime_error;
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
using unisim::kernel::ServiceExportBase;
using unisim::kernel::variable::Parameter;
using unisim::kernel::logger::Logger;
using unisim::service::interfaces::Loader;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::MemoryInjection;
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::Blob;
using unisim::util::endian::endian_type;
using unisim::service::interfaces::Register;

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
class LinuxOS :
	public Service<unisim::service::interfaces::LinuxOS>,
	public Service<unisim::service::interfaces::Loader>,
	public Service<unisim::service::interfaces::Blob<ADDRESS_TYPE> >,
	public Client<Memory<ADDRESS_TYPE> >,
	public Client<MemoryInjection<ADDRESS_TYPE> >,
	public Client<Registers>,
	public Client<Loader>,
	public Client<unisim::service::interfaces::Blob<ADDRESS_TYPE> >
{
public:
    /* Exported services */
	ServiceExport<unisim::service::interfaces::LinuxOS> linux_os_export;
	ServiceExport<unisim::service::interfaces::Loader> loader_export;
	ServiceExport<unisim::service::interfaces::Blob<ADDRESS_TYPE> > blob_export;

	/* Imported services */
	ServiceImport<Memory<ADDRESS_TYPE> > memory_import;
	ServiceImport<MemoryInjection<ADDRESS_TYPE> > memory_injection_import;
	ServiceImport<Registers> registers_import;
	ServiceImport<Loader> loader_import;
	ServiceImport<Blob<ADDRESS_TYPE> > blob_import;

    /* Constructor/Destructor */
    LinuxOS(const char *name, Object *parent = 0);
    virtual ~LinuxOS();

    /* Service methods */
    virtual void OnDisconnect();
	virtual bool BeginSetup();
	virtual bool Setup(ServiceExportBase *srv_export);
	virtual bool EndSetup();

    /* Service interface methods */
    virtual void ExecuteSystemCall(int id);
	virtual bool Load();
	virtual const unisim::util::blob::Blob<ADDRESS_TYPE> *GetBlob();

private:
	bool LoadARM();
	bool LoadPPC();
	bool SetupLoadARM();
	bool SetupLoadPPC();
	bool SetupLoad();
	bool SetupBlobARM();
	bool SetupBlobPPC();
	bool SetupBlob();
	bool SetupLinuxOSARM();
	bool SetupLinuxOSPPC();
	bool SetupLinuxOS();
	bool ARMSetup();
	bool PPCSetup();

	/*
	 * The following methods are just here for printf debugging purposes,
	 * otherwise they are unused
	 */
	void DumpBlob();
	void DumpBlob(const unisim::util::blob::Blob<ADDRESS_TYPE> *b, int level);
	
	bool ReadMem(ADDRESS_TYPE, void *buffer, uint32_t size);
	bool WriteMem(ADDRESS_TYPE, const void *buffer, uint32_t size);
	
	int GetSyscallNumber(int id);
	int ARMGetSyscallNumber(int id);
	int ARMEABIGetSyscallNumber(int id);
	int PPCGetSyscallNumber(int id);
	ADDRESS_TYPE GetMmapBase() const;
	void SetMmapBase(ADDRESS_TYPE base);
	ADDRESS_TYPE GetMmapBrkPoint() const;
	void SetMmapBrkPoint(ADDRESS_TYPE brk_point);
	ADDRESS_TYPE GetBrkPoint() const;
	void SetBrkPoint(ADDRESS_TYPE brk_point);
	PARAMETER_TYPE GetSystemCallParam(int id);
	PARAMETER_TYPE ARMGetSystemCallParam(int id);
	PARAMETER_TYPE ARMEABIGetSystemCallParam(int id);
	PARAMETER_TYPE PPCGetSystemCallParam(int id);
	void SetSystemCallStatus(int ret, bool error);
	void ARMSetSystemCallStatus(int ret, bool error);
	void ARMEABISetSystemCallStatus(int ret, bool error);
	void PPCSetSystemCallStatus(int ret, bool error);
	
	endian_type GetEndianess();
	void SetSyscallId(const string &syscall_name, int syscall_id);
    bool Swap();
    void PerformSwap(void *buf,
                     int count);
    int StringLength(ADDRESS_TYPE addr);

    bool HasSyscall(const string &syscall_name);
    bool HasSyscall(int syscall_id);

    typedef LinuxOS<ADDRESS_TYPE,PARAMETER_TYPE> thistype;
    typedef void (thistype::*syscall_t)();

	unisim::util::blob::Blob<ADDRESS_TYPE> *blob;

	map<string, syscall_t> syscall_name_map;
    map<int, string> syscall_name_assoc_map;
    map<int, syscall_t> syscall_impl_assoc_map;

    int current_syscall_id;
    string current_syscall_name;

    string system;
    Parameter<string> param_system;
    endian_type endianess;
    string endianess_string;
    Parameter<string> param_endian;
    PARAMETER_TYPE memory_page_size;
    Parameter<PARAMETER_TYPE> param_memory_page_size;

    /* uname parameters */
    string utsname_sysname;
    Parameter<string> param_utsname_sysname;
    string utsname_nodename;
    Parameter<string> param_utsname_nodename;
    string utsname_release;
    Parameter<string> param_utsname_release;
    string utsname_version;
    Parameter<string> param_utsname_version;
    string utsname_machine;
    Parameter<string> param_utsname_machine;
    string utsname_domainname;
    Parameter<string> param_utsname_domainname;

    /* the logger and its verbose option */
	Logger logger;
    bool verbose;
    Parameter<bool> param_verbose;
		
    //#include "linux_system_calls.hh"
	void LSC_unknown();
	void LSC_exit();
	void LSC_read();
	void LSC_write();
	void LSC_open();
	void LSC_close();
	void LSC_lseek();
	void LSC_getpid();
	void LSC_getuid();
	void LSC_access();
	void LSC_times();
	void LSC_brk();
	void LSC_getgid();
	void LSC_geteuid();
	void LSC_getegid();
	void LSC_munmap();
	void LSC_stat();
	void LSC_fstat();
	void LSC_uname();
	void LSC_llseek();
	void LSC_writev();
	void LSC_mmap();
	void LSC_mmap2();
	void LSC_stat64(); 
	void LSC_fstat64(); 
	void LSC_getuid32();
	void LSC_getgid32();
	void LSC_geteuid32();
	void LSC_getegid32();
	void LSC_flistxattr();
	void LSC_exit_group(); 
	void LSC_fcntl(); 
	void LSC_fcntl64();
	void LSC_dup();
	void LSC_ioctl();
	void LSC_ugetrlimit();
	void LSC_getrlimit();
	void LSC_setrlimit();
	void LSC_rt_sigaction();
	void LSC_getrusage();
	void LSC_unlink();
	void LSC_rename();
	void LSC_time();
	void LSC_gettimeofday();
	void LSC_socketcall();
	void LSC_rt_sigprocmask();
	void LSC_kill();
	void LSC_ftruncate();
	void LSC_arm_breakpoint();
	void LSC_arm_cacheflush();
	void LSC_arm_usr26();
	void LSC_arm_usr32();
	void LSC_arm_set_tls();
	void SetSyscallNameMap();

    // state shared by all the systems
	PARAMETER_TYPE mmap_base;
    PARAMETER_TYPE mmap_brk_point;
    PARAMETER_TYPE brk_point;

    // registers for the arm system
    Register *arm_regs[16];

    // registers for the ppc system
    Register *ppc_cr;
	Register *ppc_cia;
    Register *ppc_regs[31];
    
    // uname structure
    struct utsname_t {
    	char sysname[65];
    	char nodename[65];
    	char release[65];
    	char version[65];
    	char machine[65];
    };
    /*****************************************************************************
     * ARM structures START                                                      *
     *****************************************************************************/

    // this structure supposes that the timespec was needed (__timespec_defined)
    // original structure
    // struct timespec
    //   {
    //     __time_t tv_sec;            /* Seconds.  */
    //     long int tv_nsec;           /* Nanoseconds.  */
    //   };
    struct arm_timespec_t {
    	uint32_t tv_sec;		/* Seconds.  */
    	uint32_t tv_nsec;		/* Nanoseconds.  */
    };

    struct arm_timeval_t {
	int32_t tv_sec;
	int32_t tv_usec;
    };
    
    struct  arm_rusage_t {
	struct arm_timeval_t ru_utime;        /* user time used */
        struct arm_timeval_t ru_stime;        /* system time used */
        int32_t ru_maxrss;                 /* maximum resident set size */
        int32_t ru_ixrss;                  /* integral shared memory size */
        int32_t ru_idrss;                  /* integral unshared data size */
        int32_t ru_isrss;                  /* integral unshared stack size */
        int32_t ru_minflt;                 /* page reclaims */
        int32_t ru_majflt;                 /* page faults */
        int32_t ru_nswap;                  /* swaps */
        int32_t ru_inblock;                /* block input operations */
        int32_t ru_oublock;                /* block output operations */
        int32_t ru_msgsnd;                 /* messages sent */
        int32_t ru_msgrcv;                 /* messages received */
        int32_t ru_nsignals;               /* signals received */
        int32_t ru_nvcsw;                  /* voluntary context switches */
        int32_t ru_nivcsw;                 /* involuntary " */
    };

    // this structure supposes that the arm file was compiled with
    //   the __USE_LARGEFILE64 and __USE_MISC
    // original structure
    // #ifdef __USE_LARGEFILE64
    // struct stat64
    //   {
    //     __dev_t st_dev;			/* Device.  */
    //     unsigned int __pad1;
    
    //     __ino_t __st_ino;			/* 32bit file serial number.	*/
    //     __mode_t st_mode;			/* File mode.  */
    //     __nlink_t st_nlink;			/* Link count.  */
    //     __uid_t st_uid;			/* User ID of the file's owner.	*/
    //     __gid_t st_gid;			/* Group ID of the file's group.*/
    //     __dev_t st_rdev;			/* Device number, if device.  */
    //     unsigned int __pad2;
    //     __off64_t st_size;			/* Size of file, in bytes.  */
    //     __blksize_t st_blksize;		/* Optimal block size for I/O.  */
    
    //     __blkcnt64_t st_blocks;		/* Number 512-byte blocks allocated. */
    // #ifdef __USE_MISC
    //     /* Nanosecond resolution timestamps are stored in a format
    //        equivalent to 'struct timespec'.  This is the type used
    //        whenever possible but the Unix namespace rules do not allow the
    //        identifier 'timespec' to appear in the <sys/stat.h> header.
    //        Therefore we have to handle the use of this header in strictly
    //        standard-compliant sources special.  */
    //     struct timespec st_atim;		/* Time of last access.  */
    //     struct timespec st_mtim;		/* Time of last modification.  */
    //     struct timespec st_ctim;		/* Time of last status change.  */
    // #else
    //     __time_t st_atime;			/* Time of last access.  */
    //     unsigned long int st_atimensec;	/* Nscecs of last access.  */
    //     __time_t st_mtime;			/* Time of last modification.  */
    //     unsigned long int st_mtimensec;	/* Nsecs of last modification.  */
    //     __time_t st_ctime;			/* Time of last status change.  */
    //     unsigned long int st_ctimensec;	/* Nsecs of last status change.  */
    // #endif
    //     __ino64_t st_ino;			/* File serial number.		*/
    //   };
    // #endif


    struct arm_stat64_t {
    	uint64_t st_dev;			/* Device.  */
    	uint32_t __pad1;
      
    	uint32_t __st_ino;			/* 32bit file serial number.	*/
    	uint32_t st_mode;			/* File mode.  */
    	uint32_t st_nlink;			/* Link count.  */
    	uint32_t st_uid;			/* User ID of the file's owner.	*/
    	uint32_t st_gid;			/* Group ID of the file's group.*/
    	uint64_t st_rdev;			/* Device number, if device.  */
    	uint32_t __pad2;
    	uint64_t st_size;			/* Size of file, in bytes.  */
    	uint32_t st_blksize;		/* Optimal block size for I/O.  */
      
    	uint64_t st_blocks;		/* Number 512-byte blocks allocated. */
    	struct arm_timespec_t st_atim;		/* Time of last access.  */
    	struct arm_timespec_t st_mtim;		/* Time of last modification.  */
    	struct arm_timespec_t st_ctim;		/* Time of last status change.  */
    	uint64_t st_ino;			/* File serial number.		*/
    };
    
    // 	original structure
    // 	struct tms
    // 	{
    // 	  clock_t tms_utime;          /* User CPU time.  */
    // 	  clock_t tms_stime;          /* System CPU time.  */
    // 	  clock_t tms_cutime;         /* User CPU time of dead children.  */
    // 	  clock_t tms_cstime;         /* System CPU time of dead children.  */
    // 	};
    struct arm_tms_t {
    	int32_t tms_utime;          /* User CPU time.  */
    	int32_t tms_stime;          /* System CPU time.  */
      
    	int32_t tms_cutime;         /* User CPU time of dead children.  */
    	int32_t tms_cstime;         /* System CPU time of dead children.  */
    };

    /*****************************************************************************
     * ARM structures END                                                        *
     *****************************************************************************/

    /*****************************************************************************
     * PPC structures START                                                      *
     *****************************************************************************/

    // this structure supposes that the timespec was needed (__timespec_defined)
    // original structure
    // struct timespec
    //   {
    //     __time_t tv_sec;            /* Seconds.  */
    //     long int tv_nsec;           /* Nanoseconds.  */
    //   };
    struct powerpc_timespec_t {
        int32_t tv_sec;        /* Seconds.  */
        int32_t tv_nsec;       /* Nanoseconds.  */
    };
    
    struct powerpc_timeval_t {
	int32_t tv_sec;
	int32_t tv_usec;
    };
    
    struct powerpc_rusage_t {
	struct powerpc_timeval_t ru_utime;        /* user time used */
        struct powerpc_timeval_t ru_stime;        /* system time used */
        int32_t ru_maxrss;                 /* maximum resident set size */
        int32_t ru_ixrss;                  /* integral shared memory size */
        int32_t ru_idrss;                  /* integral unshared data size */
        int32_t ru_isrss;                  /* integral unshared stack size */
        int32_t ru_minflt;                 /* page reclaims */
        int32_t ru_majflt;                 /* page faults */
        int32_t ru_nswap;                  /* swaps */
        int32_t ru_inblock;                /* block input operations */
        int32_t ru_oublock;                /* block output operations */
        int32_t ru_msgsnd;                 /* messages sent */
        int32_t ru_msgrcv;                 /* messages received */
        int32_t ru_nsignals;               /* signals received */
        int32_t ru_nvcsw;                  /* voluntary context switches */
        int32_t ru_nivcsw;                 /* involuntary " */
    };

    // this structure supposes that the powerpc file was compiled with
    //   the __USE_LARGEFILE64 and __USE_MISC
    // original structure
//     # ifdef __USE_LARGEFILE64
//     struct stat64
//     {
//         __dev_t st_dev;			/* Device.  */
//         __ino64_t st_ino;			/* File serial number.	*/
//         __mode_t st_mode;			/* File mode.  */
//         __nlink_t st_nlink;			/* Link count.  */
//         __uid_t st_uid;			/* User ID of the file's owner.	*/
//         __gid_t st_gid;			/* Group ID of the file's group.*/
//         __dev_t st_rdev;			/* Device number, if device.  */
//         unsigned short int __pad2;
//         __off64_t st_size;			/* Size of file, in bytes.  */
//         __blksize_t st_blksize;		/* Optimal block size for I/O.  */
//         __blkcnt64_t st_blocks;		/* Number 512-byte blocks allocated. */
//     #ifdef __USE_MISC
//         /* Nanosecond resolution timestamps are stored in a format
//         equivalent to 'struct timespec'.  This is the type used
//         whenever possible but the Unix namespace rules do not allow the
//         identifier 'timespec' to appear in the <sys/stat.h> header.
//         Therefore we have to handle the use of this header in strictly
//         standard-compliant sources special.  */
//         struct timespec st_atim;		/* Time of last access.  */
//         struct timespec st_mtim;		/* Time of last modification.  */
//         struct timespec st_ctim;		/* Time of last status change.  */
//     # define st_atime st_atim.tv_sec	/* Backward compatibility.  */
//     # define st_mtime st_mtim.tv_sec
//     # define st_ctime st_ctim.tv_sec
//     #else
//         __time_t st_atime;			/* Time of last access.  */
//         unsigned long int st_atimensec;	/* Nscecs of last access.  */
//         __time_t st_mtime;			/* Time of last modification.  */
//         unsigned long int st_mtimensec;	/* Nsecs of last modification.  */
//         __time_t st_ctime;			/* Time of last status change.  */
//         unsigned long int st_ctimensec;	/* Nsecs of last status change.  */
//     #endif
//         unsigned long int __unused4;
//         unsigned long int __unused5;
//     };
//     # endif /* __USE_LARGEFILE64 */

    struct powerpc_stat64_t {
        uint64_t st_dev;			/* Device.  */
        uint64_t st_ino;			/* File serial number.	*/
        uint32_t st_mode;			/* File mode.  */
        uint32_t st_nlink;			/* Link count.  */
        uint32_t st_uid;			/* User ID of the file's owner.	*/
        uint32_t st_gid;			/* Group ID of the file's group.*/
        uint64_t st_rdev;			/* Device number, if device.  */
        uint32_t __pad2;
        int64_t st_size;			/* Size of file, in bytes.  */
        int32_t st_blksize;		/* Optimal block size for I/O.  */
        int64_t st_blocks;		/* Number 512-byte blocks allocated. */
    /* Nanosecond resolution timestamps are stored in a format
       equivalent to 'struct timespec'.  This is the type used
       whenever possible but the Unix namespace rules do not allow the
       identifier 'timespec' to appear in the <sys/stat.h> header.
       Therefore we have to handle the use of this header in strictly
       standard-compliant sources special.  */
        struct powerpc_timespec_t st_atim;		/* Time of last access.  */
        struct powerpc_timespec_t st_mtim;		/* Time of last modification.  */
        struct powerpc_timespec_t st_ctim;		/* Time of last status change.  */
        uint32_t __unused4;
        uint32_t __unused5;
    };

    struct powerpc_stat_t
    {
      int64_t st_dev;
      uint32_t __pad1;
      uint32_t st_ino;
      uint32_t st_mode;
      uint32_t st_nlink;
      uint32_t st_uid;
      uint32_t st_gid;
      int64_t st_rdev;
      uint32_t __pad2;
      int32_t st_size;
      int32_t st_blksize;
      int32_t st_blocks;
      struct powerpc_timespec_t st_atim;
      struct powerpc_timespec_t st_mtim;
      struct powerpc_timespec_t st_ctim;
      uint32_t __unused4;
      uint32_t __unused5;
    };
    
    // 	original structure
    // 	struct tms
    // 	{
    // 	  clock_t tms_utime;          /* User CPU time.  */
    // 	  clock_t tms_stime;          /* System CPU time.  */
    // 	  clock_t tms_cutime;         /* User CPU time of dead children.  */
    // 	  clock_t tms_cstime;         /* System CPU time of dead children.  */
    // 	};
    struct powerpc_tms_t {
        int32_t tms_utime;      /* User CPU time.  */
        int32_t tms_stime;      /* System CPU time.  */
    
        int32_t tms_cutime;     /* User CPU time of dead children.  */
        int32_t tms_cstime;     /* System CPU time of dead children.  */
    };

    /*****************************************************************************
     * PPC structures END                                                        *
     *****************************************************************************/

	int Stat(int fd, struct powerpc_stat_t *target_stat);
	int Stat64(int fd, struct powerpc_stat64_t *target_stat);
	int Stat64(int fd, struct arm_stat64_t *target_stat);
	int Times(struct powerpc_tms_t *target_tms);
	int Times(struct arm_tms_t *target_tms);
	int GetTimeOfDay(struct powerpc_timeval_t *target_tv);
	int GetTimeOfDay(struct arm_timeval_t *target_tv);
	int GetRUsage(PARAMETER_TYPE who, struct powerpc_rusage_t *target_ru);
	int GetRUsage(PARAMETER_TYPE who, struct arm_rusage_t *target_ru);

	static const int LINUX_O_ACCMODE = 00000003;
	static const int LINUX_O_RDONLY = 00000000;
	static const int LINUX_O_WRONLY = 00000001;
	static const int LINUX_O_RDWR = 00000002;
	static const int LINUX_O_CREAT = 00000100;
	static const int LINUX_O_EXCL = 00000200;
	static const int LINUX_O_TRUNC = 00001000;
	static const int LINUX_O_APPEND = 00002000;

	const char *osrelease_filename;
	const char *fake_osrelease_filename;
};

} // end of linux_os namespace
} // end of os namespace
} // end of service namespace
} // end of unisim namespace

#endif // __UNISIM_SERVICE_OS_LINUX_OS_LINUX_OS_HH__
