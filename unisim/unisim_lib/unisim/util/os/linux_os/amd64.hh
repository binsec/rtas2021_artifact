/*
 *  Copyright (c) 2018, Commissariat a l'Energie Atomique (CEA)
 *
 *    All rights reserved.
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
 */

#ifndef __UNISIM_UTIL_OS_LINUX_AMD64_HH__
#define __UNISIM_UTIL_OS_LINUX_AMD64_HH__

#include <unisim/util/likely/likely.hh>
#include <unisim/util/os/linux_os/errno.hh>

#include <stdexcept>
#include <cerrno>
#include <cstdlib>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#include <process.h>
#include <windows.h>
#else
#include <sys/times.h>
#include <sys/time.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <inttypes.h>

namespace unisim {
namespace util {
namespace os {
namespace linux_os {

  template <class LINUX>
  struct AMD64TS : public LINUX::TargetSystem
  {
    // HWCAP flags
    // static uint32_t const AMD64_HWCAP_AMD64_SWP       = 1 << 0;
    // static uint32_t const AMD64_HWCAP_AMD64_HALF      = 1 << 1;
    // static uint32_t const AMD64_HWCAP_AMD64_THUMB     = 1 << 2;
    // static uint32_t const AMD64_HWCAP_AMD64_26BIT     = 1 << 3;
    // static uint32_t const AMD64_HWCAP_AMD64_FAST_MULT = 1 << 4;
    // static uint32_t const AMD64_HWCAP_AMD64_FPA       = 1 << 5;
    // static uint32_t const AMD64_HWCAP_AMD64_VFP       = 1 << 6;
    // static uint32_t const AMD64_HWCAP_AMD64_EDSP      = 1 << 7;
    // static uint32_t const AMD64_HWCAP_AMD64_JAVA      = 1 << 8;
    // static uint32_t const AMD64_HWCAP_AMD64_IWMMXT    = 1 << 9;
    // static uint32_t const AMD64_HWCAP_AMD64_CRUNCH    = 1 << 10;
    // static uint32_t const AMD64_HWCAP_AMD64_THUMBEE   = 1 << 11;
    // static uint32_t const AMD64_HWCAP_AMD64_NEON      = 1 << 12;
    // static uint32_t const AMD64_HWCAP_AMD64_VFPv3     = 1 << 13;
    // static uint32_t const AMD64_HWCAP_AMD64_VFPv3D16  = 1 << 14;
    // static uint32_t const AMD64_HWCAP_AMD64_TLS       = 1 << 15;
    // static uint32_t const AMD64_HWCAP_AMD64_VFPv4     = 1 << 16;
    // static uint32_t const AMD64_HWCAP_AMD64_IDIVA     = 1 << 17;
    // static uint32_t const AMD64_HWCAP_AMD64_IDIVT     = 1 << 18;
    
    // Name imports
    typedef typename LINUX::SysCall SysCall;
    typedef typename LINUX::address_type address_type;
    typedef typename LINUX::parameter_type parameter_type;
    typedef typename LINUX::UTSName UTSName;
    using LINUX::TargetSystem::lin;
    using LINUX::TargetSystem::name;
    
    // // AMD64 linux structs
    // struct amd64_timespec {
    //   uint32_t tv_sec;  /* Seconds.      (__time_t) */
    //   uint32_t tv_nsec; /* Nanoseconds.  (long int) */
    // };
    
    struct amd64_stat
    {
      uint64_t ust_dev;        // => 0
      uint64_t ust_ino;        // => 8
      uint64_t ust_nlink;      // => 16
      uint32_t ust_mode;       // => 24
      uint32_t ust_uid;        // => 28
      uint32_t ust_gid;        // => 32
      uint32_t __pad0;         // => 36
      uint64_t ust_rdev;       // => 40
      uint64_t ust_size;       // => 48
      uint64_t ust_blksize;    // => 56
      int64_t  ust_blocks;     // => 64
      int64_t  ust_atime;      // => 72
      uint64_t ust_atime_nsec; // => 80
      int64_t  ust_mtime;      // => 88
      uint64_t ust_mtime_nsec; // => 96
      int64_t  ust_ctime;      // => 104
      uint64_t ust_ctime_nsec; // => 112
      uint64_t __unused[3];    // => 120
    };

    struct amd64_utsname
    {
      char sysname[65];
      char nodename[65];
      char release[65];
      char version[65];
      char machine[65];
    };

    AMD64TS( LINUX& _lin ) : LINUX::TargetSystem( "amd64", _lin ) {}
    
    bool GetAT_HWCAP( address_type& hwcap ) const
    {
      uint64_t amd64_hwcap = 0;
      std::string hwcap_token;
      std::stringstream sstr(this->GetHWCAP());
      while(sstr >> hwcap_token)
        {
          // if(hwcap_token.compare("swp") == 0)             { amd64_hwcap |= AMD64_HWCAP_AMD64_SWP; }
          // else if(hwcap_token.compare("half") == 0)       { amd64_hwcap |= AMD64_HWCAP_AMD64_HALF; }
          // else if(hwcap_token.compare("thumb") == 0)      { amd64_hwcap |= AMD64_HWCAP_AMD64_THUMB; }
          // else if(hwcap_token.compare("26bit") == 0)      { amd64_hwcap |= AMD64_HWCAP_AMD64_26BIT; }
          // else if(hwcap_token.compare("fastmult") == 0)   { amd64_hwcap |= AMD64_HWCAP_AMD64_FAST_MULT; }
          // else if(hwcap_token.compare("fpa") == 0)        { amd64_hwcap |= AMD64_HWCAP_AMD64_FPA; }
          // else if(hwcap_token.compare("vfp") == 0)        { amd64_hwcap |= AMD64_HWCAP_AMD64_VFP; }
          // else if(hwcap_token.compare("edsp") == 0)       { amd64_hwcap |= AMD64_HWCAP_AMD64_EDSP; }
          // else if(hwcap_token.compare("edsp") == 0)       { amd64_hwcap |= AMD64_HWCAP_AMD64_JAVA; }
          // else if(hwcap_token.compare("java") == 0)       { amd64_hwcap |= AMD64_HWCAP_AMD64_JAVA; }
          // else if(hwcap_token.compare("iwmmxt") == 0)     { amd64_hwcap |= AMD64_HWCAP_AMD64_IWMMXT; }
          // else if(hwcap_token.compare("crunch") == 0)     { amd64_hwcap |= AMD64_HWCAP_AMD64_CRUNCH; }
          // else if(hwcap_token.compare("thumbee") == 0)    { amd64_hwcap |= AMD64_HWCAP_AMD64_THUMBEE; }
          // else if(hwcap_token.compare("neon") == 0)       { amd64_hwcap |= AMD64_HWCAP_AMD64_NEON; }
          // else if(hwcap_token.compare("vfpv3") == 0)      { amd64_hwcap |= AMD64_HWCAP_AMD64_VFPv3; }
          // else if(hwcap_token.compare("vfpv3d16") == 0)   { amd64_hwcap |= AMD64_HWCAP_AMD64_VFPv3D16; }
          // else if(hwcap_token.compare("tls") == 0)        { amd64_hwcap |= AMD64_HWCAP_AMD64_TLS; }
          // else if(hwcap_token.compare("vfpv4") == 0)      { amd64_hwcap |= AMD64_HWCAP_AMD64_VFPv4; }
          // else if(hwcap_token.compare("idiva") == 0)      { amd64_hwcap |= AMD64_HWCAP_AMD64_IDIVA; }
          // else if(hwcap_token.compare("idivt") == 0)      { amd64_hwcap |= AMD64_HWCAP_AMD64_IDIVT; }
          // else
            { lin.DebugWarningStream() << "unknown hardware capability \"" << hwcap_token << "\"" << std::endl; }
        }
      hwcap = amd64_hwcap;
      return true;
    }
    
    bool SetupTarget() const
    {
      // Reset all target registers
      // for (int idx = 0; idx < 32; ++idx) {
      //   std::ostringstream buf;
      //   buf << 'x' << std::dec << idx;
      //   if (not ClearRegister(lin, buf.str().c_str()))
      //     return false;
      // }
      
      // Set PC to the program entry point
      if (not lin.SetTargetRegister("%rip", lin.GetEntryPoint()))
        return false;
      
      // Set SP to the base of the created stack
      unisim::util::blob::Section<address_type> const * sp_section =
        lin.GetBlob()->FindSection(".unisim.linux_os.stack.stack_pointer");
      if (sp_section == NULL) {
        lin.DebugErrorStream() << "Could not find the stack pointer section." << std::endl;
        return false;
      }
      address_type stack_pointer = sp_section->GetAddr();
      if (not lin.SetTargetRegister("%rsp", stack_pointer))
        return false;
      parameter_type par1 = 0;
      parameter_type par2 = 0;
      if (not this->MemIF().ReadMemory(stack_pointer +  8, (uint8_t *)&par1, sizeof(par1)) or
          not this->MemIF().ReadMemory(stack_pointer + 16, (uint8_t *)&par2, sizeof(par2)) or
          not lin.SetTargetRegister("%rdi", Target2Host(lin.GetEndianness(), par1)) or
          not lin.SetTargetRegister("%rsi", Target2Host(lin.GetEndianness(), par2)))
        return false;
          
      return true;
    }
    
    static void SetAMD64SystemCallStatus(LINUX& _lin, int64_t ret, bool error)
    { _lin.SetTargetRegister("%rax", (parameter_type) ret); }
    
    void SetSystemCallStatus(int64_t ret, bool error) const { SetAMD64SystemCallStatus( lin, ret, error ); }
    
    static parameter_type GetSystemCallParam( LINUX& lin, int id )
    {
      parameter_type val = 0;
      switch (id) {
      case 0: lin.GetTargetRegister("%rdi", val); break;
      case 1: lin.GetTargetRegister("%rsi", val); break;
      case 2: lin.GetTargetRegister("%rdx", val); break;
      case 3: lin.GetTargetRegister("%r10", val); break;
      case 4: lin.GetTargetRegister("%r8", val); break;
      case 5: lin.GetTargetRegister("%r9", val); break;
      default: throw std::logic_error("internal_error");
      }
          
      return val;
    }
    
    parameter_type GetSystemCallParam( int id ) const
    {
      try { return GetSystemCallParam( lin, id ); }
      
      catch (...) {
        lin.DebugErrorStream() << "No syscall argument #" << id << " in " << this->name << " linux" << std::endl;
      }
      
      return 0;
    }

    static int Fstat64(int fd, struct amd64_stat *target_stat, unisim::util::endian::endian_type endianness)
    {
      int ret;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) | defined(_WIN64)
      struct _stati64 host_stat;
      ret = _fstati64(fd, &host_stat);
#elif defined(linux) || defined(__linux) || defined(__linux__)
      struct stat64 host_stat;
      ret = fstat64(fd, &host_stat);
#elif defined(__APPLE_CC__)
      struct stat host_stat;
      ret = fstat(fd, &host_stat);
#endif
      if(ret < 0) return ret;

      memset(target_stat, 0, sizeof(struct amd64_stat));
	
#if defined(__x86_64) || defined(__amd64) || defined(__x86_64__) || defined(__amd64__) || defined(__LP64__) || defined(_LP64)
      // 64-bit host
      target_stat->ust_dev = Host2Target(endianness, (uint64_t) host_stat.st_dev);
      target_stat->ust_ino = Host2Target(endianness, (uint64_t) host_stat.st_ino);
      target_stat->ust_mode = Host2Target(endianness, (uint32_t) host_stat.st_mode);
      target_stat->ust_nlink = Host2Target(endianness, (uint64_t) host_stat.st_nlink);
      target_stat->ust_uid = Host2Target(endianness, (uint32_t) host_stat.st_uid);
      target_stat->ust_gid = Host2Target(endianness, (uint32_t) host_stat.st_gid);
      target_stat->ust_rdev = Host2Target(endianness, (uint64_t) host_stat.st_rdev);
      target_stat->ust_size = Host2Target(endianness, (int64_t) host_stat.st_size);
#if defined(WIN64) // Windows x64
      target_stat->ust_blksize = Host2Target(endianness, (int32_t) 512);
      target_stat->ust_blocks = Host2Target(endianness, (uint64_t)((host_stat.st_size + 511) / 512));
      target_stat->ust_atime = Host2Target(endianness, (uint32_t) host_stat.st_atime);
      target_stat->ust_atime_nsec = 0;
      target_stat->ust_mtime = Host2Target(endianness, (uint32_t) host_stat.st_mtime);
      target_stat->ust_mtime_nsec = 0;
      target_stat->ust_ctime = Host2Target(endianness, (uint32_t) host_stat.st_ctime);
      target_stat->ust_ctime_nsec = 0;
#elif defined(linux) || defined(__linux) || defined(__linux__) // Linux x64
      target_stat->ust_blksize = Host2Target(endianness, (uint32_t) host_stat.st_blksize);
      target_stat->ust_blocks = Host2Target(endianness, (uint64_t) host_stat.st_blocks);
      target_stat->ust_atime = Host2Target(endianness, (uint32_t) host_stat.st_atim.tv_sec);
      target_stat->ust_atime_nsec = Host2Target(endianness, (uint32_t) host_stat.st_atim.tv_nsec);
      target_stat->ust_mtime = Host2Target(endianness, (uint32_t) host_stat.st_mtim.tv_sec);
      target_stat->ust_mtime_nsec = Host2Target(endianness, (uint32_t) host_stat.st_mtim.tv_nsec);
      target_stat->ust_ctime = Host2Target(endianness, (uint32_t) host_stat.st_ctim.tv_sec);
      target_stat->ust_ctime_nsec = Host2Target(endianness, (uint32_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // darwin PPC64/x86_64
      target_stat->ust_blksize = Host2Target(endianness, (uint32_t) host_stat.st_blksize);
      target_stat->ust_blocks = Host2Target(endianness, (int64_t) host_stat.st_blocks);
      target_stat->ust_atime = Host2Target(endianness, (uint32_t) host_stat.st_atimespec.tv_sec);
      target_stat->ust_atime_nsec = Host2Target(endianness, (uint32_t) host_stat.st_atimespec.tv_nsec);
      target_stat->ust_mtime = Host2Target(endianness, (uint32_t) host_stat.st_mtimespec.tv_sec);
      target_stat->ust_mtime_nsec = Host2Target(endianness, (uint32_t) host_stat.st_mtimespec.tv_nsec);
      target_stat->ust_ctime = Host2Target(endianness, (uint32_t) host_stat.st_ctimespec.tv_sec);
      target_stat->ust_ctime_nsec = Host2Target(endianness, (uint32_t) host_stat.st_ctimespec.tv_nsec);
#endif

#else
      // 32-bit host
      target_stat->ust_dev = Host2Target(endianness, (uint64_t) host_stat.st_dev);
      target_stat->ust_ino = Host2Target(endianness, (uint64_t) host_stat.st_ino);
      target_stat->ust_mode = Host2Target(endianness, (uint32_t) host_stat.st_mode);
      target_stat->ust_nlink = Host2Target(endianness, (uint32_t) host_stat.st_nlink);
      target_stat->ust_uid = Host2Target(endianness, (uint32_t) host_stat.st_uid);
      target_stat->ust_gid = Host2Target(endianness, (uint32_t) host_stat.st_gid);
      target_stat->ust_rdev = Host2Target(endianness, (uint64_t) host_stat.st_rdev);
      target_stat->ust_size = Host2Target(endianness, (int64_t) host_stat.st_size);
#if defined(WIN32) || defined(_WIN32) // Windows 32
      target_stat->ust_blksize = Host2Target(endianness, (int32_t) 512);
      target_stat->ust_blocks = Host2Target(endianness, (int64_t)((host_stat.st_size + 511) / 512));
      target_stat->ust_atime = Host2Target(endianness, (int32_t) host_stat.st_atime);
      target_stat->ust_atime_nsec = 0;
      target_stat->ust_mtime = Host2Target(endianness, (int32_t) host_stat.st_mtime);
      target_stat->ust_mtime_nsec = 0;
      target_stat->ust_ctime = Host2Target(endianness, (int32_t) host_stat.st_ctime);
      target_stat->ust_ctime_nsec = 0;
#elif defined(linux) || defined(__linux) || defined(__linux__) // Linux 32
      target_stat->ust_blksize = Host2Target(endianness, (uint32_t) host_stat.st_blksize);
      target_stat->ust_blocks = Host2Target(endianness, (uint64_t) host_stat.st_blocks);
      target_stat->ust_atime = Host2Target(endianness, (uint32_t) host_stat.st_atim.tv_sec);
      target_stat->ust_atime_nsec = Host2Target(endianness, (uint32_t) host_stat.st_atim.tv_nsec);
      target_stat->ust_mtime = Host2Target(endianness, (uint32_t) host_stat.st_mtim.tv_sec);
      target_stat->ust_mtime_nsec = Host2Target(endianness, (uint32_t) host_stat.st_mtim.tv_nsec);
      target_stat->ust_ctime = Host2Target(endianness, (uint32_t) host_stat.st_ctim.tv_sec);
      target_stat->ust_ctime_nsec = Host2Target(endianness, (uint32_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // Darwin PPC32/x86
      target_stat->ust_blksize = Host2Target(endianness, (uint32_t) host_stat.st_blksize);
      target_stat->ust_blocks = Host2Target(endianness, (uint64_t) host_stat.st_blocks);
      target_stat->ust_atime = Host2Target(endianness, (uint32_t) host_stat.st_atimespec.tv_sec);
      target_stat->ust_atime_nsec = Host2Target(endianness, (uint32_t) host_stat.st_atimespec.tv_nsec);
      target_stat->ust_mtime = Host2Target(endianness, (uint32_t) host_stat.st_mtimespec.tv_sec);
      target_stat->ust_mtime_nsec = Host2Target(endianness, (uint32_t) host_stat.st_mtimespec.tv_nsec);
      target_stat->ust_ctime = Host2Target(endianness, (uint32_t) host_stat.st_ctimespec.tv_sec);
      target_stat->ust_ctime_nsec = Host2Target(endianness, (uint32_t) host_stat.st_ctimespec.tv_nsec);
#endif

#endif
      return ret;
    }

    SysCall* GetSystemCall( int& id ) const
    {
      // see either arch/amd64/include/asm/unistd[_64].h
      switch (id) {
      case 0:     return this->GetSysCall("read");
      case 1:     return this->GetSysCall("write");
      case 2:     return this->GetSysCall("open");
      case 3:     return this->GetSysCall("close");
      case 4:     return this->GetSysCall("stat");
        /* 5: fstat (see amd64 specific) */
      case 6:     return this->GetSysCall("lstat");
      case 7:     return this->GetSysCall("poll");
      case 8:     return this->GetSysCall("lseek");
      case 9:     return this->GetSysCall("mmap");
      case 10:    return this->GetSysCall("mprotect");
      case 11:    return this->GetSysCall("munmap");
      case 12:    return this->GetSysCall("brk");
      case 13:    return this->GetSysCall("rt_sigaction");
      case 14:    return this->GetSysCall("rt_sigprocmask");
      case 15:    return this->GetSysCall("rt_sigreturn");
      case 16:    return this->GetSysCall("ioctl");
      case 17:    return this->GetSysCall("pread64");
      case 18:    return this->GetSysCall("pwrite64");
      case 19:    return this->GetSysCall("readv");
      case 20:    return this->GetSysCall("writev");
      case 21:    return this->GetSysCall("access");
      case 22:    return this->GetSysCall("pipe");
      case 23:    return this->GetSysCall("select");
      case 24:    return this->GetSysCall("sched_yield");
      case 25:    return this->GetSysCall("mremap");
      case 26:    return this->GetSysCall("msync");
      case 27:    return this->GetSysCall("mincore");
      case 28:    return this->GetSysCall("madvise");
      case 29:    return this->GetSysCall("shmget");
      case 30:    return this->GetSysCall("shmat");
      case 31:    return this->GetSysCall("shmctl");
      case 32:    return this->GetSysCall("dup");
      case 33:    return this->GetSysCall("dup2");
      case 34:    return this->GetSysCall("pause");
      case 35:    return this->GetSysCall("nanosleep");
      case 36:    return this->GetSysCall("getitimer");
      case 37:    return this->GetSysCall("alarm");
      case 38:    return this->GetSysCall("setitimer");
      case 39:    return this->GetSysCall("getpid");
      case 40:    return this->GetSysCall("sendfile");
      case 41:    return this->GetSysCall("socket");
      case 42:    return this->GetSysCall("connect");
      case 43:    return this->GetSysCall("accept");
      case 44:    return this->GetSysCall("sendto");
      case 45:    return this->GetSysCall("recvfrom");
      case 46:    return this->GetSysCall("sendmsg");
      case 47:    return this->GetSysCall("recvmsg");
      case 48:    return this->GetSysCall("shutdown");
      case 49:    return this->GetSysCall("bind");
      case 50:    return this->GetSysCall("listen");
      case 51:    return this->GetSysCall("getsockname");
      case 52:    return this->GetSysCall("getpeername");
      case 53:    return this->GetSysCall("socketpair");
      case 54:    return this->GetSysCall("setsockopt");
      case 55:    return this->GetSysCall("getsockopt");
      case 56:    return this->GetSysCall("clone");
      case 57:    return this->GetSysCall("fork");
      case 58:    return this->GetSysCall("vfork");
      case 59:    return this->GetSysCall("execve");
      case 60:    return this->GetSysCall("exit");
      case 61:    return this->GetSysCall("wait4");
      case 62:    return this->GetSysCall("kill");
        /* 63: uname (see AMD64 specific) */;
      case 64:    return this->GetSysCall("semget");
      case 65:    return this->GetSysCall("semop");
      case 66:    return this->GetSysCall("semctl");
      case 67:    return this->GetSysCall("shmdt");
      case 68:    return this->GetSysCall("msgget");
      case 69:    return this->GetSysCall("msgsnd");
      case 70:    return this->GetSysCall("msgrcv");
      case 71:    return this->GetSysCall("msgctl");
      case 72:    return this->GetSysCall("fcntl");
      case 73:    return this->GetSysCall("flock");
      case 74:    return this->GetSysCall("fsync");
      case 75:    return this->GetSysCall("fdatasync");
      case 76:    return this->GetSysCall("truncate");
      case 77:    return this->GetSysCall("ftruncate");
      case 78:    return this->GetSysCall("getdents");
      case 79:    return this->GetSysCall("getcwd");
      case 80:    return this->GetSysCall("chdir");
      case 81:    return this->GetSysCall("fchdir");
      case 82:    return this->GetSysCall("rename");
      case 83:    return this->GetSysCall("mkdir");
      case 84:    return this->GetSysCall("rmdir");
      case 85:    return this->GetSysCall("creat");
      case 86:    return this->GetSysCall("link");
      case 87:    return this->GetSysCall("unlink");
      case 88:    return this->GetSysCall("symlink");
      case 89:    return this->GetSysCall("readlink");
      case 90:    return this->GetSysCall("chmod");
      case 91:    return this->GetSysCall("fchmod");
      case 92:    return this->GetSysCall("chown");
      case 93:    return this->GetSysCall("fchown");
      case 94:    return this->GetSysCall("lchown");
      case 95:    return this->GetSysCall("umask");
      case 96:    return this->GetSysCall("gettimeofday");
      case 97:    return this->GetSysCall("getrlimit");
      case 98:    return this->GetSysCall("getrusage");
      case 99:    return this->GetSysCall("sysinfo");
      case 100:   return this->GetSysCall("times");
      case 101:   return this->GetSysCall("ptrace");
      case 102:   return this->GetSysCall("getuid");
      case 103:   return this->GetSysCall("syslog");
      case 104:   return this->GetSysCall("getgid");
      case 105:   return this->GetSysCall("setuid");
      case 106:   return this->GetSysCall("setgid");
      case 107:   return this->GetSysCall("geteuid");
      case 108:   return this->GetSysCall("getegid");
      case 109:   return this->GetSysCall("setpgid");
      case 110:   return this->GetSysCall("getppid");
      case 111:   return this->GetSysCall("getpgrp");
      case 112:   return this->GetSysCall("setsid");
      case 113:   return this->GetSysCall("setreuid");
      case 114:   return this->GetSysCall("setregid");
      case 115:   return this->GetSysCall("getgroups");
      case 116:   return this->GetSysCall("setgroups");
      case 117:   return this->GetSysCall("setresuid");
      case 118:   return this->GetSysCall("getresuid");
      case 119:   return this->GetSysCall("setresgid");
      case 120:   return this->GetSysCall("getresgid");
      case 121:   return this->GetSysCall("getpgid");
      case 122:   return this->GetSysCall("setfsuid");
      case 123:   return this->GetSysCall("setfsgid");
      case 124:   return this->GetSysCall("getsid");
      case 125:   return this->GetSysCall("capget");
      case 126:   return this->GetSysCall("capset");
      case 127:   return this->GetSysCall("rt_sigpending");
      case 128:   return this->GetSysCall("rt_sigtimedwait");
      case 129:   return this->GetSysCall("rt_sigqueueinfo");
      case 130:   return this->GetSysCall("rt_sigsuspend");
      case 131:   return this->GetSysCall("sigaltstack");
      case 132:   return this->GetSysCall("utime");
      case 133:   return this->GetSysCall("mknod");
      case 134:   return this->GetSysCall("uselib");
      case 135:   return this->GetSysCall("personality");
      case 136:   return this->GetSysCall("ustat");
      case 137:   return this->GetSysCall("statfs");
      case 138:   return this->GetSysCall("fstatfs");
      case 139:   return this->GetSysCall("sysfs");
      case 140:   return this->GetSysCall("getpriority");
      case 141:   return this->GetSysCall("setpriority");
      case 142:   return this->GetSysCall("sched_setparam");
      case 143:   return this->GetSysCall("sched_getparam");
      case 144:   return this->GetSysCall("sched_setscheduler");
      case 145:   return this->GetSysCall("sched_getscheduler");
      case 146:   return this->GetSysCall("sched_get_priority_max");
      case 147:   return this->GetSysCall("sched_get_priority_min");
      case 148:   return this->GetSysCall("sched_rr_get_interval");
      case 149:   return this->GetSysCall("mlock");
      case 150:   return this->GetSysCall("munlock");
      case 151:   return this->GetSysCall("mlockall");
      case 152:   return this->GetSysCall("munlockall");
      case 153:   return this->GetSysCall("vhangup");
      case 154:   return this->GetSysCall("modify_ldt");
      case 155:   return this->GetSysCall("pivot_root");
      case 156:   return this->GetSysCall("_sysctl");
      case 157:   return this->GetSysCall("prctl");
        /* 158: arch_prctl (see AMD64 specific) */
      case 159:   return this->GetSysCall("adjtimex");
      case 160:   return this->GetSysCall("setrlimit");
      case 161:   return this->GetSysCall("chroot");
      case 162:   return this->GetSysCall("sync");
      case 163:   return this->GetSysCall("acct");
      case 164:   return this->GetSysCall("settimeofday");
      case 165:   return this->GetSysCall("mount");
      case 166:   return this->GetSysCall("umount2");
      case 167:   return this->GetSysCall("swapon");
      case 168:   return this->GetSysCall("swapoff");
      case 169:   return this->GetSysCall("reboot");
      case 170:   return this->GetSysCall("sethostname");
      case 171:   return this->GetSysCall("setdomainname");
      case 172:   return this->GetSysCall("iopl");
      case 173:   return this->GetSysCall("ioperm");
      case 174:   return this->GetSysCall("create_module");
      case 175:   return this->GetSysCall("init_module");
      case 176:   return this->GetSysCall("delete_module");
      case 177:   return this->GetSysCall("get_kernel_syms");
      case 178:   return this->GetSysCall("query_module");
      case 179:   return this->GetSysCall("quotactl");
      case 180:   return this->GetSysCall("nfsservctl");
      case 181:   return this->GetSysCall("getpmsg");
      case 182:   return this->GetSysCall("putpmsg");
      case 183:   return this->GetSysCall("afs_syscall");
      case 184:   return this->GetSysCall("tuxcall");
      case 185:   return this->GetSysCall("security");
      case 186:   return this->GetSysCall("gettid");
      case 187:   return this->GetSysCall("readahead");
      case 188:   return this->GetSysCall("setxattr");
      case 189:   return this->GetSysCall("lsetxattr");
      case 190:   return this->GetSysCall("fsetxattr");
      case 191:   return this->GetSysCall("getxattr");
      case 192:   return this->GetSysCall("lgetxattr");
      case 193:   return this->GetSysCall("fgetxattr");
      case 194:   return this->GetSysCall("listxattr");
      case 195:   return this->GetSysCall("llistxattr");
      case 196:   return this->GetSysCall("flistxattr");
      case 197:   return this->GetSysCall("removexattr");
      case 198:   return this->GetSysCall("lremovexattr");
      case 199:   return this->GetSysCall("fremovexattr");
      case 200:   return this->GetSysCall("tkill");
      case 201:   return this->GetSysCall("time");
      case 202:   return this->GetSysCall("futex");
      case 203:   return this->GetSysCall("sched_setaffinity");
      case 204:   return this->GetSysCall("sched_getaffinity");
      case 205:   return this->GetSysCall("set_thread_area");
      case 206:   return this->GetSysCall("io_setup");
      case 207:   return this->GetSysCall("io_destroy");
      case 208:   return this->GetSysCall("io_getevents");
      case 209:   return this->GetSysCall("io_submit");
      case 210:   return this->GetSysCall("io_cancel");
      case 211:   return this->GetSysCall("get_thread_area");
      case 212:   return this->GetSysCall("lookup_dcookie");
      case 213:   return this->GetSysCall("epoll_create");
      case 214:   return this->GetSysCall("epoll_ctl_old");
      case 215:   return this->GetSysCall("epoll_wait_old");
      case 216:   return this->GetSysCall("remap_file_pages");
      case 217:   return this->GetSysCall("getdents64");
      case 218:   return this->GetSysCall("set_tid_address");
      case 219:   return this->GetSysCall("restart_syscall");
      case 220:   return this->GetSysCall("semtimedop");
      case 221:   return this->GetSysCall("fadvise64");
      case 222:   return this->GetSysCall("timer_create");
      case 223:   return this->GetSysCall("timer_settime");
      case 224:   return this->GetSysCall("timer_gettime");
      case 225:   return this->GetSysCall("timer_getoverrun");
      case 226:   return this->GetSysCall("timer_delete");
      case 227:   return this->GetSysCall("clock_settime");
      case 228:   return this->GetSysCall("clock_gettime");
      case 229:   return this->GetSysCall("clock_getres");
      case 230:   return this->GetSysCall("clock_nanosleep");
      case 231:   return this->GetSysCall("exit_group");
      case 232:   return this->GetSysCall("epoll_wait");
      case 233:   return this->GetSysCall("epoll_ctl");
      case 234:   return this->GetSysCall("tgkill");
      case 235:   return this->GetSysCall("utimes");
      case 236:   return this->GetSysCall("vserver");
      case 237:   return this->GetSysCall("mbind");
      case 238:   return this->GetSysCall("set_mempolicy");
      case 239:   return this->GetSysCall("get_mempolicy");
      case 240:   return this->GetSysCall("mq_open");
      case 241:   return this->GetSysCall("mq_unlink");
      case 242:   return this->GetSysCall("mq_timedsend");
      case 243:   return this->GetSysCall("mq_timedreceive");
      case 244:   return this->GetSysCall("mq_notify");
      case 245:   return this->GetSysCall("mq_getsetattr");
      case 246:   return this->GetSysCall("kexec_load");
      case 247:   return this->GetSysCall("waitid");
      case 248:   return this->GetSysCall("add_key");
      case 249:   return this->GetSysCall("request_key");
      case 250:   return this->GetSysCall("keyctl");
      case 251:   return this->GetSysCall("ioprio_set");
      case 252:   return this->GetSysCall("ioprio_get");
      case 253:   return this->GetSysCall("inotify_init");
      case 254:   return this->GetSysCall("inotify_add_watch");
      case 255:   return this->GetSysCall("inotify_rm_watch");
      case 256:   return this->GetSysCall("migrate_pages");
      case 257:   return this->GetSysCall("openat");
      case 258:   return this->GetSysCall("mkdirat");
      case 259:   return this->GetSysCall("mknodat");
      case 260:   return this->GetSysCall("fchownat");
      case 261:   return this->GetSysCall("futimesat");
      case 262:   return this->GetSysCall("newfstatat");
      case 263:   return this->GetSysCall("unlinkat");
      case 264:   return this->GetSysCall("renameat");
      case 265:   return this->GetSysCall("linkat");
      case 266:   return this->GetSysCall("symlinkat");
      case 267:   return this->GetSysCall("readlinkat");
      case 268:   return this->GetSysCall("fchmodat");
      case 269:   return this->GetSysCall("faccessat");
      case 270:   return this->GetSysCall("pselect6");
      case 271:   return this->GetSysCall("ppoll");
      case 272:   return this->GetSysCall("unshare");
      case 273:   return this->GetSysCall("set_robust_list");
      case 274:   return this->GetSysCall("get_robust_list");
      case 275:   return this->GetSysCall("splice");
      case 276:   return this->GetSysCall("tee");
      case 277:   return this->GetSysCall("sync_file_range");
      case 278:   return this->GetSysCall("vmsplice");
      case 279:   return this->GetSysCall("move_pages");
      case 280:   return this->GetSysCall("utimensat");
      case 281:   return this->GetSysCall("epoll_pwait");
      case 282:   return this->GetSysCall("signalfd");
      case 283:   return this->GetSysCall("timerfd_create");
      case 284:   return this->GetSysCall("eventfd");
      case 285:   return this->GetSysCall("fallocate");
      case 286:   return this->GetSysCall("timerfd_settime");
      case 287:   return this->GetSysCall("timerfd_gettime");
      case 288:   return this->GetSysCall("accept4");
      case 289:   return this->GetSysCall("signalfd4");
      case 290:   return this->GetSysCall("eventfd2");
      case 291:   return this->GetSysCall("epoll_create1");
      case 292:   return this->GetSysCall("dup3");
      case 293:   return this->GetSysCall("pipe2");
      case 294:   return this->GetSysCall("inotify_init1");
      case 295:   return this->GetSysCall("preadv");
      case 296:   return this->GetSysCall("pwritev");
      case 297:   return this->GetSysCall("rt_tgsigqueueinfo");
      case 298:   return this->GetSysCall("perf_event_open");
      case 299:   return this->GetSysCall("recvmmsg");
      case 300:   return this->GetSysCall("fanotify_init");
      case 301:   return this->GetSysCall("fanotify_mark");
      case 302:   return this->GetSysCall("prlimit64");
      case 303:   return this->GetSysCall("name_to_handle_at");
      case 304:   return this->GetSysCall("open_by_handle_at");
      case 305:   return this->GetSysCall("clock_adjtime");
      case 306:   return this->GetSysCall("syncfs");
      case 307:   return this->GetSysCall("sendmmsg");
      case 308:   return this->GetSysCall("setns");
      case 309:   return this->GetSysCall("getcpu");
      case 310:   return this->GetSysCall("process_vm_readv");
      case 311:   return this->GetSysCall("process_vm_writev");
      case 312:   return this->GetSysCall("kcmp");
      case 313:   return this->GetSysCall("finit_module");
      case 314:   return this->GetSysCall("sched_setattr");
      case 315:   return this->GetSysCall("sched_getattr");
      case 316:   return this->GetSysCall("renameat2");
      case 317:   return this->GetSysCall("seccomp");
      case 318:   return this->GetSysCall("getrandom");
      case 319:   return this->GetSysCall("memfd_create");
      case 320:   return this->GetSysCall("kexec_file_load");
      case 321:   return this->GetSysCall("bpf");
      case 322:   return this->GetSysCall("execveat");
      case 323:   return this->GetSysCall("userfaultfd");
      case 324:   return this->GetSysCall("membarrier");
      case 325:   return this->GetSysCall("mlock2");
      case 326:   return this->GetSysCall("copy_file_range");
      case 327:   return this->GetSysCall("preadv2");
      case 328:   return this->GetSysCall("pwritev2");
      case 329:   return this->GetSysCall("pkey_mprotect");
      case 330:   return this->GetSysCall("pkey_alloc");
      case 331:   return this->GetSysCall("pkey_free");
      case 332:   return this->GetSysCall("statx");
	
      case 5: /* Amd64 specific stat call */
        {
          // asmlinkage long sys_fstat(unsigned int fd, struct __old_kernel_stat __user *statbuf);
          static struct : public SysCall
          {
            char const* GetName() const { return "fstat"; }
            struct Args {
              Args(LINUX& lin) : fd(SysCall::GetParam(lin, 0)), statbuf(SysCall::GetParam(lin, 1)) {};
              unsigned fd; parameter_type statbuf;
            };
            void Describe( LINUX& lin, std::ostream& sink ) const
            {
              Args sc(lin);
              sink << "(unsigned fd=" << std::dec << sc.fd << ", struct __old_kernel_stat __user *statbuf=" << std::hex << sc.statbuf << ")";
            }
            void Execute( LINUX& lin, int syscall_id ) const
            {
              Args sc(lin);
              int host_fd = SysCall::Target2HostFileDescriptor(lin, sc.fd);

              if (host_fd == -1)
                {
                  lin.SetSystemCallStatus(-LINUX_EBADF,false);
                  return;
                }
        
              struct amd64_stat target_stat;
              
              int ret = Fstat64(host_fd, &target_stat, lin.GetEndianness());

              if (ret == -1) {
                lin.SetSystemCallStatus(-SysCall::HostToLinuxErrno(errno),true);
                return;
              }
  
              lin.WriteMemory(sc.statbuf, (uint8_t *)&target_stat, sizeof(target_stat));
	
              lin.SetSystemCallStatus(ret, false);
            }
          } sc; return &sc;
        } break;
        
      case 63: /* AMD64 specific uname syscall */
        {
          static struct : public SysCall {
            char const* GetName() const { return "uname"; }
            void Describe( LINUX& lin, std::ostream& sink ) const
            {
              address_type buf_addr = GetSystemCallParam(lin, 0);
              sink << "(struct utsname *buf=" << std::hex << buf_addr << std::dec << ")";
            }
            void Execute( LINUX& lin, int syscall_id ) const
            {
              int ret;
              int32_t target_errno = 0;
              address_type buf_addr = GetSystemCallParam(lin, 0);
              ret = 0;
	
              struct amd64_utsname value;
              memset(&value, 0, sizeof(value));
              UTSName const& utsname = lin.GetUTSName();
              
              strncpy(value.sysname,  utsname.sysname.c_str(), sizeof(value.sysname) - 1);
              strncpy(value.nodename, utsname.nodename.c_str(), sizeof(value.nodename) - 1);
              strncpy(value.release,  utsname.release.c_str(), sizeof(value.release) - 1);
              strncpy(value.version,  utsname.version.c_str(), sizeof(value.version) - 1);
              strncpy(value.machine,  utsname.machine.c_str(), sizeof(value.machine) - 1);
              lin.WriteMemory(buf_addr, (uint8_t *)&value, sizeof(value));
	
              SetAMD64SystemCallStatus(lin, (ret == -1) ? -target_errno : ret, (ret == -1));
            }
          } sc;
          return &sc;
        } break;

      case 158: /* amd64 specific arch_prctl */
        {
          static struct : public SysCall {
            // asmlinkage long long sys_arch_prctl(int code, unsigned long addr)            
            char const* GetName() const { return "arch_prctl"; }
            struct Args {
              Args(LINUX& lin) : code( SysCall::GetParam(lin, 0) ), addr( SysCall::GetParam(lin, 1) ) {}
              int code; address_type addr;
            };
            void Describe( LINUX& lin, std::ostream& sink ) const
            {
              Args sc(lin);
              sink << "(int code=" << std::dec << sc.code << ", unsigned long addr=" << std::hex << sc.addr << ")";
            }
            void Execute( LINUX& lin, int syscall_id ) const
            {
              Args sc(lin);
              
              switch (sc.code)
                {
                case 0x1001: // ARCH_SET_GS; Set the 64-bit base for the GS register to addr.
                  lin.SetTargetRegister( "%gs_base", sc.addr ); // pseudo allocation of a tls descriptor
                  break;
                case 0x1002: // ARCH_SET_FS; Set the 64-bit base for the FS register to addr.
                  lin.SetTargetRegister( "%fs_base", sc.addr ); // pseudo allocation of a tls descriptor
                  break;
                case 0x1003: // ARCH_GET_FS; Return the 64-bit base value for the FS register of the current thread in the unsigned long pointed to by addr.
                  lin.WriteMemory( sc.addr, lin.GetTargetRegister( "%fs_base" ) );
                  break;
                case 0x1004: // ARCH_GET_GS; Return the 64-bit base value for the GS register of the current thread in the unsigned long pointed to by addr.
                  lin.WriteMemory( sc.addr, lin.GetTargetRegister( "%gs_base" ) );
                  break;
                case 0x1011: // ARCH_GET_CPUID;
                case 0x1012: // ARCH_SET_CPUID;
                case 0x2001: // ARCH_MAP_VDSO_X32;
                case 0x2002: // ARCH_MAP_VDSO_32;
                case 0x2003: // ARCH_MAP_VDSO_64;
                default:  
                  lin.SetSystemCallStatus(-LINUX_EINVAL,true);
                  return;
                }
              
              lin.SetSystemCallStatus( 0, false );
            }
          } sc;
          return &sc;
        } break;
      }
      
      return 0;
    }
    
    bool SetSystemBlob( unisim::util::blob::Blob<address_type>* blob ) const { return true; }
  };

} // end of namespace linux_os
} // end of namespace os
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_OS_LINUX_AMD64_HH__
