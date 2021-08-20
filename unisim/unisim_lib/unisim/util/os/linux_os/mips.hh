/*
 *  Copyright (c) 2019-2020,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
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

#ifndef __UNISIM_UTIL_OS_LINUX_MIPS_HH__
#define __UNISIM_UTIL_OS_LINUX_MIPS_HH__

#include <unisim/util/likely/likely.hh>
#include <unisim/util/os/linux_os/errno.hh>
#include <stdexcept>
#include <cerrno>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) | defined(_WIN64)
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
  struct MIPSTS : public LINUX::TargetSystem
  {
    // Name imports
    typedef typename LINUX::SysCall SysCall;
    typedef typename LINUX::address_type address_type;
    typedef typename LINUX::parameter_type parameter_type;
    typedef typename LINUX::UTSName UTSName;
    using LINUX::TargetSystem::lin;
    using LINUX::TargetSystem::name;

    // MIPS linux structs
    struct mips_timespec
    {
      int32_t tv_sec;     /* Seconds.     (__time_t) */
      int32_t tv_nsec;    /* Nanoseconds. (long int) */
    };

    struct mips_stat64
    {
      uint64_t st_dev;            // 00-08 0b0000xxx
      uint32_t __pad1;            // 08-0c 0b00010xx
      uint32_t __st_ino;          // 0c-10 0b00011xx
      uint32_t st_mode;           // 10-14 0b00100xx
      uint32_t st_nlink;          // 14-18 0b00101xx
      uint32_t st_uid;            // 18-1c 0b00110xx
      uint32_t st_gid;            // 1c-20 0b00111xx
      uint64_t st_rdev;           // 20-28 0b0100xxx
      uint32_t __pad2;            // 28-2c 0b01010xx
      int32_t  st_size_low;       // 2c-30 0b01011xx ... need to split st_size to solve
      int32_t  st_size_high;      // 30-34 0b01100xx ... alignment issues on 64bits platforms
      int32_t  st_blksize;        // 34-38 0b01101xx
      int64_t  st_blocks;         // 38-40 0b0111xxx
      int32_t  st_atim_tv_sec;    // 40-44 0b10000xx
      int32_t  st_atim_tv_nsec;   // 44-48 0b10001xx
      int32_t  st_mtim_tv_sec;    // 48-4c 0b10010xx
      int32_t  st_mtim_tv_nsec;   // 4c-50 0b10011xx
      int32_t  st_ctim_tv_sec;    // 50-54 0b10100xx
      int32_t  st_ctim_tv_nsec;   // 54-58 0b10101xx
      uint64_t st_ino;            // 58-60 0b1011xxx
    };

    struct mips_tms
    {
      int32_t tms_utime;          /* User CPU time.                    (clock_t) */
      int32_t tms_stime;          /* System CPU time.                  (clock_t) */
      int32_t tms_cutime;         /* User CPU time of dead children.   (clock_t) */
      int32_t tms_cstime;         /* System CPU time of dead children. (clock_t) */
    };

    struct mips_utsname
    {
      char sysname[65];
      char nodename[65];
      char release[65];
      char version[65];
      char machine[65];
      char domainname[65];
    };

    struct mips_timeval
    {
      int32_t tv_sec;         /* seconds      (__kernel_time_t) */
      int32_t tv_usec;        /* microseconds (__kernel_suseconds_t) */
    };

    struct mips_timezone
    {
      int32_t tz_minuteswest; /* minutes west of Greenwich (int) */
      int32_t tz_dsttime;     /* type of dst correction    (int) */
    };

    MIPSTS( LINUX& _lin ) : LINUX::TargetSystem( "mips", _lin ) {}

    bool GetAT_HWCAP( address_type& hwcap ) const { return false; }

    bool SetupTarget() const
    {
      // Reset all target registers

      // General purpose registers
      for (unsigned reg = 1; reg < 32; ++reg)
        {
          char regname[4];
          snprintf( regname, sizeof regname, "$%d", reg );
          if (not lin.ClearTargetRegister(regname))
            return false;
        }

      // Set EIP to the program entry point
      if (not lin.SetTargetRegister("pc", lin.GetEntryPoint()))
        return false;
      // Set SP to the base of the created stack
      unisim::util::blob::Section<address_type> const * sp_section =
        lin.GetBlob()->FindSection(".unisim.linux_os.stack.stack_pointer");
      if (sp_section == NULL)
        {
          lin.DebugErrorStream() << "Could not find the stack pointer section." << std::endl;
          return false;
        }
      if (not lin.SetTargetRegister("sp", sp_section->GetAddr()))
        return false;

      // parameter_type par1 = 0;
      // parameter_type par2 = 0;
      // if (not this->MemIF().ReadMemory(stack_pointer +  8, (uint8_t *)&par1, sizeof(par1)) or
      //     not this->MemIF().ReadMemory(stack_pointer + 16, (uint8_t *)&par2, sizeof(par2)) or
      //     not lin.SetTargetRegister("%rdi", Target2Host(lin.GetEndianness(), par1)) or
      //     not lin.SetTargetRegister("%rsi", Target2Host(lin.GetEndianness(), par2)))
      //   return false;

      // Early TLS setup
      if (auto etls_section = lin.GetBlob()->FindSection(".unisim.linux_os.etls.middle_pointer"))
        {
          if (not lin.SetTargetRegister("hwr_ulr", etls_section->GetAddr() ) ) // for early TLS kludge
            return false;
        }
      else
        {
          lin.DebugErrorStream() << "Could not find the early TLS section." << std::endl;
          return false;
        }
      
      return true;
    }

    static int Fstat64(int fd, struct mips_stat64* target_stat, unisim::util::endian::endian_type endianness)
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

      memset(target_stat, 0, sizeof(struct mips_stat64));

#if defined(__x86_64) || defined(__amd64) || defined(__x86_64__) || defined(__amd64__) || defined(__LP64__) || defined(_LP64)
      // 64-bit host
      target_stat->st_dev = Host2Target(endianness, (uint64_t) host_stat.st_dev);
      target_stat->__st_ino = Host2Target(endianness, (uint32_t) host_stat.st_ino);
      target_stat->st_ino = Host2Target(endianness, (uint64_t) host_stat.st_ino);
      target_stat->st_mode = Host2Target(endianness, (uint32_t) host_stat.st_mode);
      target_stat->st_nlink = Host2Target(endianness, (uint64_t) host_stat.st_nlink);
      target_stat->st_uid = Host2Target(endianness, (uint32_t) host_stat.st_uid);
      target_stat->st_gid = Host2Target(endianness, (uint32_t) host_stat.st_gid);
      target_stat->st_rdev = Host2Target(endianness, (uint64_t) host_stat.st_rdev);
      int64_t st_size = host_stat.st_size;
      target_stat->st_size_low = Host2Target(endianness, (int32_t) (st_size >> 0));
      target_stat->st_size_high = Host2Target(endianness, (int32_t) (st_size >> 32));
#if defined(WIN64) || defined(_WIN64) // Windows x64
      target_stat->st_blksize = Host2Target(endianness, (int32_t) 512);
      target_stat->st_blocks = Host2Target(endianness, (uint64_t)((host_stat.st_size + 511) / 512));
      target_stat->st_atim_tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_atime);
      target_stat->st_atim_tv_nsec = 0;
      target_stat->st_mtim_tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_mtime);
      target_stat->st_mtim_tv_nsec = 0;
      target_stat->st_ctim_tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_ctime);
      target_stat->st_ctim_tv_nsec = 0;
#elif defined(linux) || defined(__linux) || defined(__linux__) // Linux x64
      target_stat->st_blksize = Host2Target(endianness, (uint32_t) host_stat.st_blksize);
      target_stat->st_blocks = Host2Target(endianness, (uint64_t) host_stat.st_blocks);
      target_stat->st_atim_tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_atim.tv_sec);
      target_stat->st_atim_tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_atim.tv_nsec);
      target_stat->st_mtim_tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_mtim.tv_sec);
      target_stat->st_mtim_tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_mtim.tv_nsec);
      target_stat->st_ctim_tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_ctim.tv_sec);
      target_stat->st_ctim_tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // darwin PPC64/x86_64
      target_stat->st_blksize = Host2Target(endianness, (uint32_t) host_stat.st_blksize);
      target_stat->st_blocks = Host2Target(endianness, (int64_t) host_stat.st_blocks);
      target_stat->st_atim_tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_atimespec.tv_sec);
      target_stat->st_atim_tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_atimespec.tv_nsec);
      target_stat->st_mtim_tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_mtimespec.tv_sec);
      target_stat->st_mtim_tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_mtimespec.tv_nsec);
      target_stat->st_ctim_tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_ctimespec.tv_sec);
      target_stat->st_ctim_tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_ctimespec.tv_nsec);
#endif

#else
      // 32-bit host
      target_stat->st_dev = Host2Target(endianness, (uint64_t) host_stat.st_dev);
      target_stat->__st_ino = Host2Target(endianness, (uint32_t) host_stat.st_ino);
      target_stat->st_ino = Host2Target(endianness, (uint64_t) host_stat.st_ino);
      target_stat->st_mode = Host2Target(endianness, (uint32_t) host_stat.st_mode);
      target_stat->st_nlink = Host2Target(endianness, (uint32_t) host_stat.st_nlink);
      target_stat->st_uid = Host2Target(endianness, (uint32_t) host_stat.st_uid);
      target_stat->st_gid = Host2Target(endianness, (uint32_t) host_stat.st_gid);
      target_stat->st_rdev = Host2Target(endianness, (uint64_t) host_stat.st_rdev);
      int64_t st_size = host_stat.st_size;
      target_stat->st_size_low = Host2Target(endianness, (int32_t) (st_size >> 0));
      target_stat->st_size_high = Host2Target(endianness, (int32_t) (st_size >> 32));
#if defined(WIN32) || defined(_WIN32) // Windows 32
      target_stat->st_blksize = Host2Target(endianness, (int32_t) 512);
      target_stat->st_blocks = Host2Target(endianness, (int64_t)((host_stat.st_size + 511) / 512));
      target_stat->st_atim_tv_sec = Host2Target(endianness, (int32_t) host_stat.st_atime);
      target_stat->st_atim_tv_nsec = 0;
      target_stat->st_mtim_tv_sec = Host2Target(endianness, (int32_t) host_stat.st_mtime);
      target_stat->st_mtim_tv_nsec = 0;
      target_stat->st_ctim_tv_sec = Host2Target(endianness, (int32_t) host_stat.st_ctime);
      target_stat->st_ctim_tv_nsec = 0;
#elif defined(linux) || defined(__linux) || defined(__linux__) // Linux 32
      target_stat->st_blksize = Host2Target(endianness, (uint32_t) host_stat.st_blksize);
      target_stat->st_blocks = Host2Target(endianness, (uint64_t) host_stat.st_blocks);
      target_stat->st_atim_tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_atim.tv_sec);
      target_stat->st_atim_tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_atim.tv_nsec);
      target_stat->st_mtim_tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_mtim.tv_sec);
      target_stat->st_mtim_tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_mtim.tv_nsec);
      target_stat->st_ctim_tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_ctim.tv_sec);
      target_stat->st_ctim_tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // Darwin PPC32/x86
      target_stat->st_blksize = Host2Target(endianness, (uint32_t) host_stat.st_blksize);
      target_stat->st_blocks = Host2Target(endianness, (uint64_t) host_stat.st_blocks);
      target_stat->st_atim_tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_atimespec.tv_sec);
      target_stat->st_atim_tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_atimespec.tv_nsec);
      target_stat->st_mtim_tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_mtimespec.tv_sec);
      target_stat->st_mtim_tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_mtimespec.tv_nsec);
      target_stat->st_ctim_tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_ctimespec.tv_sec);
      target_stat->st_ctim_tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_ctimespec.tv_nsec);
#endif

#endif
      return ret;
    }
    /*                  instruction  syscall #  retval  error
     *      mips        syscall      v0         v0      a3       [1]
     *
     *                  arg1  arg2  arg3  arg4  arg5  arg6  arg7
     *      mips/n32,64 a0    a1    a2    a3    a4    a5    -
     *      mips/o32    a0    a1    a2    a3    -     -     -
     */
    static void SetMIPSSystemCallStatus(LINUX& lin, int ret, bool error)
    {
      lin.SetTargetRegister("a3", (parameter_type) error);
      lin.SetTargetRegister("v0", (parameter_type) ret);
    }
    void SetSystemCallStatus(int64_t ret, bool error) const { SetMIPSSystemCallStatus( lin, ret, error ); }

    static parameter_type GetSystemCallParam(LINUX& lin, int id)
    {
      parameter_type val = 0;

      switch (id) {
      case 0: lin.GetTargetRegister("a0", val); break;
      case 1: lin.GetTargetRegister("a1", val); break;
      case 2: lin.GetTargetRegister("a2", val); break;
      case 3: lin.GetTargetRegister("a3", val); break;
      case 4: lin.GetTargetRegister("t0", val); break;
      case 5: lin.GetTargetRegister("t1", val); break;
      default: throw std::logic_error("internal error");
      }

      return val;
    }
    parameter_type GetSystemCallParam(int id) const
    {
      try { return GetSystemCallParam( lin, id ); }

      catch (int x) {
        lin.DebugErrorStream() << "No syscall argument #" << id << " in " << this->name << " linux" << std::endl;
      }

      return 0;
    }

    SysCall* GetSystemCall( int& id ) const
    {
      // TODO: fill syscall map
      // see either arch/mips/include/asm/unistd.h or arch/mips/include/uapi/asm/unistd.h in LINUX source

      parameter_type scnum = 0;
      lin.GetTargetRegister("v0", scnum);
      id = scnum;
      
      switch (scnum)
        {
      /* Linux o32 style syscalls are in the range from 4000 to 4999. */
        case 4000 +   0: return this->GetSysCall("syscall");
        case 4000 +   1: return this->GetSysCall("exit");
        case 4000 +   2: return this->GetSysCall("fork");
        case 4000 +   3: return this->GetSysCall("read");
        case 4000 +   4: return this->GetSysCall("write");
        case 4000 +   5: return this->GetSysCall("open");
        case 4000 +   6: return this->GetSysCall("close");
        case 4000 +   7: return this->GetSysCall("waitpid");
        case 4000 +   8: return this->GetSysCall("creat");
        case 4000 +   9: return this->GetSysCall("link");
        case 4000 +  10: return this->GetSysCall("unlink");
        case 4000 +  11: return this->GetSysCall("execve");
        case 4000 +  12: return this->GetSysCall("chdir");
        case 4000 +  13: return this->GetSysCall("time");
        case 4000 +  14: return this->GetSysCall("mknod");
        case 4000 +  15: return this->GetSysCall("chmod");
        case 4000 +  16: return this->GetSysCall("lchown");
        case 4000 +  17: return this->GetSysCall("break");
        case 4000 +  18: return this->GetSysCall("unused18");
        case 4000 +  19: return this->GetSysCall("lseek");
        case 4000 +  20: return this->GetSysCall("getpid");
        case 4000 +  21: return this->GetSysCall("mount");
        case 4000 +  22: return this->GetSysCall("umount");
        case 4000 +  23: return this->GetSysCall("setuid");
        case 4000 +  24: return this->GetSysCall("getuid");
        case 4000 +  25: return this->GetSysCall("stime");
        case 4000 +  26: return this->GetSysCall("ptrace");
        case 4000 +  27: return this->GetSysCall("alarm");
        case 4000 +  28: return this->GetSysCall("unused28");
        case 4000 +  29: return this->GetSysCall("pause");
        case 4000 +  30: return this->GetSysCall("utime");
        case 4000 +  31: return this->GetSysCall("stty");
        case 4000 +  32: return this->GetSysCall("gtty");
        case 4000 +  33: return this->GetSysCall("access");
        case 4000 +  34: return this->GetSysCall("nice");
        case 4000 +  35: return this->GetSysCall("ftime");
        case 4000 +  36: return this->GetSysCall("sync");
        case 4000 +  37: return this->GetSysCall("kill");
        case 4000 +  38: return this->GetSysCall("rename");
        case 4000 +  39: return this->GetSysCall("mkdir");
        case 4000 +  40: return this->GetSysCall("rmdir");
        case 4000 +  41: return this->GetSysCall("dup");
        case 4000 +  42: return this->GetSysCall("pipe");
        case 4000 +  43: return this->GetSysCall("times");
        case 4000 +  44: return this->GetSysCall("prof");
        case 4000 +  45: return this->GetSysCall("brk");
        case 4000 +  46: return this->GetSysCall("setgid");
        case 4000 +  47: return this->GetSysCall("getgid");
        case 4000 +  48: return this->GetSysCall("signal");
        case 4000 +  49: return this->GetSysCall("geteuid");
        case 4000 +  50: return this->GetSysCall("getegid");
        case 4000 +  51: return this->GetSysCall("acct");
        case 4000 +  52: return this->GetSysCall("umount2");
        case 4000 +  53: return this->GetSysCall("lock");
        case 4000 +  54: return this->GetSysCall("ioctl");
        case 4000 +  55: return this->GetSysCall("fcntl");
        case 4000 +  56: return this->GetSysCall("mpx");
        case 4000 +  57: return this->GetSysCall("setpgid");
        case 4000 +  58: return this->GetSysCall("ulimit");
        case 4000 +  59: return this->GetSysCall("unused59");
        case 4000 +  60: return this->GetSysCall("umask");
        case 4000 +  61: return this->GetSysCall("chroot");
        case 4000 +  62: return this->GetSysCall("ustat");
        case 4000 +  63: return this->GetSysCall("dup2");
        case 4000 +  64: return this->GetSysCall("getppid");
        case 4000 +  65: return this->GetSysCall("getpgrp");
        case 4000 +  66: return this->GetSysCall("setsid");
        case 4000 +  67: return this->GetSysCall("sigaction");
        case 4000 +  68: return this->GetSysCall("sgetmask");
        case 4000 +  69: return this->GetSysCall("ssetmask");
        case 4000 +  70: return this->GetSysCall("setreuid");
        case 4000 +  71: return this->GetSysCall("setregid");
        case 4000 +  72: return this->GetSysCall("sigsuspend");
        case 4000 +  73: return this->GetSysCall("sigpending");
        case 4000 +  74: return this->GetSysCall("sethostname");
        case 4000 +  75: return this->GetSysCall("setrlimit");
        case 4000 +  76: return this->GetSysCall("getrlimit");
        case 4000 +  77: return this->GetSysCall("getrusage");
        case 4000 +  78: return this->GetSysCall("gettimeofday");
        case 4000 +  79: return this->GetSysCall("settimeofday");
        case 4000 +  80: return this->GetSysCall("getgroups");
        case 4000 +  81: return this->GetSysCall("setgroups");
        case 4000 +  82: return this->GetSysCall("reserved82");
        case 4000 +  83: return this->GetSysCall("symlink");
        case 4000 +  84: return this->GetSysCall("unused84");
        case 4000 +  85: return this->GetSysCall("readlink");
        case 4000 +  86: return this->GetSysCall("uselib");
        case 4000 +  87: return this->GetSysCall("swapon");
        case 4000 +  88: return this->GetSysCall("reboot");
        case 4000 +  89: return this->GetSysCall("readdir");
        case 4000 +  90: return this->GetSysCall("mmap");
        case 4000 +  91: return this->GetSysCall("munmap");
        case 4000 +  92: return this->GetSysCall("truncate");
        case 4000 +  93: return this->GetSysCall("ftruncate");
        case 4000 +  94: return this->GetSysCall("fchmod");
        case 4000 +  95: return this->GetSysCall("fchown");
        case 4000 +  96: return this->GetSysCall("getpriority");
        case 4000 +  97: return this->GetSysCall("setpriority");
        case 4000 +  98: return this->GetSysCall("profil");
        case 4000 +  99: return this->GetSysCall("statfs");
        case 4000 + 100: return this->GetSysCall("fstatfs");
        case 4000 + 101: return this->GetSysCall("ioperm");
        case 4000 + 102: return this->GetSysCall("socketcall");
        case 4000 + 103: return this->GetSysCall("syslog");
        case 4000 + 104: return this->GetSysCall("setitimer");
        case 4000 + 105: return this->GetSysCall("getitimer");
        case 4000 + 106: return this->GetSysCall("stat");
        case 4000 + 107: return this->GetSysCall("lstat");
        case 4000 + 108: return this->GetSysCall("fstat");
        case 4000 + 109: return this->GetSysCall("unused109");
        case 4000 + 110: return this->GetSysCall("iopl");
        case 4000 + 111: return this->GetSysCall("vhangup");
        case 4000 + 112: return this->GetSysCall("idle");
        case 4000 + 113: return this->GetSysCall("vm86");
        case 4000 + 114: return this->GetSysCall("wait4");
        case 4000 + 115: return this->GetSysCall("swapoff");
        case 4000 + 116: return this->GetSysCall("sysinfo");
        case 4000 + 117: return this->GetSysCall("ipc");
        case 4000 + 118: return this->GetSysCall("fsync");
        case 4000 + 119: return this->GetSysCall("sigreturn");
        case 4000 + 120: return this->GetSysCall("clone");
        case 4000 + 121: return this->GetSysCall("setdomainname");
          // case 4000 + 122: return this->GetSysCall("uname"); see mips specific
        case 4000 + 123: return this->GetSysCall("modify_ldt");
        case 4000 + 124: return this->GetSysCall("adjtimex");
        case 4000 + 125: return this->GetSysCall("mprotect");
        case 4000 + 126: return this->GetSysCall("sigprocmask");
        case 4000 + 127: return this->GetSysCall("create_module");
        case 4000 + 128: return this->GetSysCall("init_module");
        case 4000 + 129: return this->GetSysCall("delete_module");
        case 4000 + 130: return this->GetSysCall("get_kernel_syms");
        case 4000 + 131: return this->GetSysCall("quotactl");
        case 4000 + 132: return this->GetSysCall("getpgid");
        case 4000 + 133: return this->GetSysCall("fchdir");
        case 4000 + 134: return this->GetSysCall("bdflush");
        case 4000 + 135: return this->GetSysCall("sysfs");
        case 4000 + 136: return this->GetSysCall("personality");
        case 4000 + 137: return this->GetSysCall("afs_syscall"); /* Syscall for Andrew File System */
        case 4000 + 138: return this->GetSysCall("setfsuid");
        case 4000 + 139: return this->GetSysCall("setfsgid");
        case 4000 + 140: return this->GetSysCall("_llseek");
        case 4000 + 141: return this->GetSysCall("getdents");
        case 4000 + 142: return this->GetSysCall("_newselect");
        case 4000 + 143: return this->GetSysCall("flock");
        case 4000 + 144: return this->GetSysCall("msync");
        case 4000 + 145: return this->GetSysCall("readv");
        case 4000 + 146: return this->GetSysCall("writev");
        case 4000 + 147: return this->GetSysCall("cacheflush");
        case 4000 + 148: return this->GetSysCall("cachectl");
        case 4000 + 149: return this->GetSysCall("sysmips");
        case 4000 + 150: return this->GetSysCall("unused150");
        case 4000 + 151: return this->GetSysCall("getsid");
        case 4000 + 152: return this->GetSysCall("fdatasync");
        case 4000 + 153: return this->GetSysCall("_sysctl");
        case 4000 + 154: return this->GetSysCall("mlock");
        case 4000 + 155: return this->GetSysCall("munlock");
        case 4000 + 156: return this->GetSysCall("mlockall");
        case 4000 + 157: return this->GetSysCall("munlockall");
        case 4000 + 158: return this->GetSysCall("sched_setparam");
        case 4000 + 159: return this->GetSysCall("sched_getparam");
        case 4000 + 160: return this->GetSysCall("sched_setscheduler");
        case 4000 + 161: return this->GetSysCall("sched_getscheduler");
        case 4000 + 162: return this->GetSysCall("sched_yield");
        case 4000 + 163: return this->GetSysCall("sched_get_priority_max");
        case 4000 + 164: return this->GetSysCall("sched_get_priority_min");
        case 4000 + 165: return this->GetSysCall("sched_rr_get_interval");
        case 4000 + 166: return this->GetSysCall("nanosleep");
        case 4000 + 167: return this->GetSysCall("mremap");
        case 4000 + 168: return this->GetSysCall("accept");
        case 4000 + 169: return this->GetSysCall("bind");
        case 4000 + 170: return this->GetSysCall("connect");
        case 4000 + 171: return this->GetSysCall("getpeername");
        case 4000 + 172: return this->GetSysCall("getsockname");
        case 4000 + 173: return this->GetSysCall("getsockopt");
        case 4000 + 174: return this->GetSysCall("listen");
        case 4000 + 175: return this->GetSysCall("recv");
        case 4000 + 176: return this->GetSysCall("recvfrom");
        case 4000 + 177: return this->GetSysCall("recvmsg");
        case 4000 + 178: return this->GetSysCall("send");
        case 4000 + 179: return this->GetSysCall("sendmsg");
        case 4000 + 180: return this->GetSysCall("sendto");
        case 4000 + 181: return this->GetSysCall("setsockopt");
        case 4000 + 182: return this->GetSysCall("shutdown");
        case 4000 + 183: return this->GetSysCall("socket");
        case 4000 + 184: return this->GetSysCall("socketpair");
        case 4000 + 185: return this->GetSysCall("setresuid");
        case 4000 + 186: return this->GetSysCall("getresuid");
        case 4000 + 187: return this->GetSysCall("query_module");
        case 4000 + 188: return this->GetSysCall("poll");
        case 4000 + 189: return this->GetSysCall("nfsservctl");
        case 4000 + 190: return this->GetSysCall("setresgid");
        case 4000 + 191: return this->GetSysCall("getresgid");
        case 4000 + 192: return this->GetSysCall("prctl");
        case 4000 + 193: return this->GetSysCall("rt_sigreturn");
        case 4000 + 194: return this->GetSysCall("rt_sigaction");
        case 4000 + 195: return this->GetSysCall("rt_sigprocmask");
        case 4000 + 196: return this->GetSysCall("rt_sigpending");
        case 4000 + 197: return this->GetSysCall("rt_sigtimedwait");
        case 4000 + 198: return this->GetSysCall("rt_sigqueueinfo");
        case 4000 + 199: return this->GetSysCall("rt_sigsuspend");
        case 4000 + 200: return this->GetSysCall("pread64");
        case 4000 + 201: return this->GetSysCall("pwrite64");
        case 4000 + 202: return this->GetSysCall("chown");
        case 4000 + 203: return this->GetSysCall("getcwd");
        case 4000 + 204: return this->GetSysCall("capget");
        case 4000 + 205: return this->GetSysCall("capset");
        case 4000 + 206: return this->GetSysCall("sigaltstack");
        case 4000 + 207: return this->GetSysCall("sendfile");
        case 4000 + 208: return this->GetSysCall("getpmsg");
        case 4000 + 209: return this->GetSysCall("putpmsg");
        case 4000 + 210: return this->GetSysCall("mmap2");
        case 4000 + 211: return this->GetSysCall("truncate64");
        case 4000 + 212: return this->GetSysCall("ftruncate64");
        case 4000 + 213: return this->GetSysCall("stat64");
        case 4000 + 214: return this->GetSysCall("lstat64");
        case 4000 + 215: return this->GetSysCall("fstat64");
        case 4000 + 216: return this->GetSysCall("pivot_root");
        case 4000 + 217: return this->GetSysCall("mincore");
        case 4000 + 218: return this->GetSysCall("madvise");
        case 4000 + 219: return this->GetSysCall("getdents64");
        case 4000 + 220: return this->GetSysCall("fcntl64");
        case 4000 + 221: return this->GetSysCall("reserved221");
        case 4000 + 222: return this->GetSysCall("gettid");
        case 4000 + 223: return this->GetSysCall("readahead");
        case 4000 + 224: return this->GetSysCall("setxattr");
        case 4000 + 225: return this->GetSysCall("lsetxattr");
        case 4000 + 226: return this->GetSysCall("fsetxattr");
        case 4000 + 227: return this->GetSysCall("getxattr");
        case 4000 + 228: return this->GetSysCall("lgetxattr");
        case 4000 + 229: return this->GetSysCall("fgetxattr");
        case 4000 + 230: return this->GetSysCall("listxattr");
        case 4000 + 231: return this->GetSysCall("llistxattr");
        case 4000 + 232: return this->GetSysCall("flistxattr");
        case 4000 + 233: return this->GetSysCall("removexattr");
        case 4000 + 234: return this->GetSysCall("lremovexattr");
        case 4000 + 235: return this->GetSysCall("fremovexattr");
        case 4000 + 236: return this->GetSysCall("tkill");
        case 4000 + 237: return this->GetSysCall("sendfile64");
        case 4000 + 238: return this->GetSysCall("futex");
        case 4000 + 239: return this->GetSysCall("sched_setaffinity");
        case 4000 + 240: return this->GetSysCall("sched_getaffinity");
        case 4000 + 241: return this->GetSysCall("io_setup");
        case 4000 + 242: return this->GetSysCall("io_destroy");
        case 4000 + 243: return this->GetSysCall("io_getevents");
        case 4000 + 244: return this->GetSysCall("io_submit");
        case 4000 + 245: return this->GetSysCall("io_cancel");
        case 4000 + 246: return this->GetSysCall("exit_group");
        case 4000 + 247: return this->GetSysCall("lookup_dcookie");
        case 4000 + 248: return this->GetSysCall("epoll_create");
        case 4000 + 249: return this->GetSysCall("epoll_ctl");
        case 4000 + 250: return this->GetSysCall("epoll_wait");
        case 4000 + 251: return this->GetSysCall("remap_file_pages");
        case 4000 + 252: return this->GetSysCall("set_tid_address");
        case 4000 + 253: return this->GetSysCall("restart_syscall");
        case 4000 + 254: return this->GetSysCall("fadvise64");
        case 4000 + 255: return this->GetSysCall("statfs64");
        case 4000 + 256: return this->GetSysCall("fstatfs64");
        case 4000 + 257: return this->GetSysCall("timer_create");
        case 4000 + 258: return this->GetSysCall("timer_settime");
        case 4000 + 259: return this->GetSysCall("timer_gettime");
        case 4000 + 260: return this->GetSysCall("timer_getoverrun");
        case 4000 + 261: return this->GetSysCall("timer_delete");
        case 4000 + 262: return this->GetSysCall("clock_settime");
        case 4000 + 263: return this->GetSysCall("clock_gettime");
        case 4000 + 264: return this->GetSysCall("clock_getres");
        case 4000 + 265: return this->GetSysCall("clock_nanosleep");
        case 4000 + 266: return this->GetSysCall("tgkill");
        case 4000 + 267: return this->GetSysCall("utimes");
        case 4000 + 268: return this->GetSysCall("mbind");
        case 4000 + 269: return this->GetSysCall("get_mempolicy");
        case 4000 + 270: return this->GetSysCall("set_mempolicy");
        case 4000 + 271: return this->GetSysCall("mq_open");
        case 4000 + 272: return this->GetSysCall("mq_unlink");
        case 4000 + 273: return this->GetSysCall("mq_timedsend");
        case 4000 + 274: return this->GetSysCall("mq_timedreceive");
        case 4000 + 275: return this->GetSysCall("mq_notify");
        case 4000 + 276: return this->GetSysCall("mq_getsetattr");
        case 4000 + 277: return this->GetSysCall("vserver");
        case 4000 + 278: return this->GetSysCall("waitid");
          /* case 4000 + 279: return this->GetSysCall("sys_setaltroot"); */
        case 4000 + 280: return this->GetSysCall("add_key");
        case 4000 + 281: return this->GetSysCall("request_key");
        case 4000 + 282: return this->GetSysCall("keyctl");
          // case 4000 + 283: return this->GetSysCall("set_thread_area"); see mips specific
        case 4000 + 284: return this->GetSysCall("inotify_init");
        case 4000 + 285: return this->GetSysCall("inotify_add_watch");
        case 4000 + 286: return this->GetSysCall("inotify_rm_watch");
        case 4000 + 287: return this->GetSysCall("migrate_pages");
        case 4000 + 288: return this->GetSysCall("openat");
        case 4000 + 289: return this->GetSysCall("mkdirat");
        case 4000 + 290: return this->GetSysCall("mknodat");
        case 4000 + 291: return this->GetSysCall("fchownat");
        case 4000 + 292: return this->GetSysCall("futimesat");
        case 4000 + 293: return this->GetSysCall("fstatat64");
        case 4000 + 294: return this->GetSysCall("unlinkat");
        case 4000 + 295: return this->GetSysCall("renameat");
        case 4000 + 296: return this->GetSysCall("linkat");
        case 4000 + 297: return this->GetSysCall("symlinkat");
        case 4000 + 298: return this->GetSysCall("readlinkat");
        case 4000 + 299: return this->GetSysCall("fchmodat");
        case 4000 + 300: return this->GetSysCall("faccessat");
        case 4000 + 301: return this->GetSysCall("pselect6");
        case 4000 + 302: return this->GetSysCall("ppoll");
        case 4000 + 303: return this->GetSysCall("unshare");
        case 4000 + 304: return this->GetSysCall("splice");
        case 4000 + 305: return this->GetSysCall("sync_file_range");
        case 4000 + 306: return this->GetSysCall("tee");
        case 4000 + 307: return this->GetSysCall("vmsplice");
        case 4000 + 308: return this->GetSysCall("move_pages");
        case 4000 + 309: return this->GetSysCall("set_robust_list");
        case 4000 + 310: return this->GetSysCall("get_robust_list");
        case 4000 + 311: return this->GetSysCall("kexec_load");
        case 4000 + 312: return this->GetSysCall("getcpu");
        case 4000 + 313: return this->GetSysCall("epoll_pwait");
        case 4000 + 314: return this->GetSysCall("ioprio_set");
        case 4000 + 315: return this->GetSysCall("ioprio_get");
        case 4000 + 316: return this->GetSysCall("utimensat");
        case 4000 + 317: return this->GetSysCall("signalfd");
        case 4000 + 318: return this->GetSysCall("timerfd");
        case 4000 + 319: return this->GetSysCall("eventfd");
        case 4000 + 320: return this->GetSysCall("fallocate");
        case 4000 + 321: return this->GetSysCall("timerfd_create");
        case 4000 + 322: return this->GetSysCall("timerfd_gettime");
        case 4000 + 323: return this->GetSysCall("timerfd_settime");
        case 4000 + 324: return this->GetSysCall("signalfd4");
        case 4000 + 325: return this->GetSysCall("eventfd2");
        case 4000 + 326: return this->GetSysCall("epoll_create1");
        case 4000 + 327: return this->GetSysCall("dup3");
        case 4000 + 328: return this->GetSysCall("pipe2");
        case 4000 + 329: return this->GetSysCall("inotify_init1");
        case 4000 + 330: return this->GetSysCall("preadv");
        case 4000 + 331: return this->GetSysCall("pwritev");
        case 4000 + 332: return this->GetSysCall("rt_tgsigqueueinfo");
        case 4000 + 333: return this->GetSysCall("perf_event_open");
        case 4000 + 334: return this->GetSysCall("accept4");
        case 4000 + 335: return this->GetSysCall("recvmmsg");
        case 4000 + 336: return this->GetSysCall("fanotify_init");
        case 4000 + 337: return this->GetSysCall("fanotify_mark");
        case 4000 + 338: return this->GetSysCall("prlimit64");
        case 4000 + 339: return this->GetSysCall("name_to_handle_at");
        case 4000 + 340: return this->GetSysCall("open_by_handle_at");
        case 4000 + 341: return this->GetSysCall("clock_adjtime");
        case 4000 + 342: return this->GetSysCall("syncfs");
        case 4000 + 343: return this->GetSysCall("sendmmsg");
        case 4000 + 344: return this->GetSysCall("setns");
        case 4000 + 345: return this->GetSysCall("process_vm_readv");
        case 4000 + 346: return this->GetSysCall("process_vm_writev");
        case 4000 + 347: return this->GetSysCall("kcmp");
        case 4000 + 348: return this->GetSysCall("finit_module");
        case 4000 + 349: return this->GetSysCall("sched_setattr");
        case 4000 + 350: return this->GetSysCall("sched_getattr");
        case 4000 + 351: return this->GetSysCall("renameat2");
        case 4000 + 352: return this->GetSysCall("seccomp");
        case 4000 + 353: return this->GetSysCall("getrandom");
        case 4000 + 354: return this->GetSysCall("memfd_create");
        case 4000 + 355: return this->GetSysCall("bpf");
        case 4000 + 356: return this->GetSysCall("execveat");
        case 4000 + 357: return this->GetSysCall("userfaultfd");
        case 4000 + 358: return this->GetSysCall("membarrier");
          // mips specific
        case 4000 + 122: // mips specific uname syscall
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
  
                struct mips_utsname value;
                memset(&value, 0, sizeof(value));
                UTSName const& utsname = lin.GetUTSName();
                strncpy(&value.sysname[0],    utsname.sysname.c_str(), sizeof(value.sysname) - 1);
                strncpy(&value.nodename[0],   utsname.nodename.c_str(), sizeof(value.nodename) - 1);
                strncpy(&value.release[0],    utsname.release.c_str(), sizeof(value.release) - 1);
                strncpy(&value.version[0],    utsname.version.c_str(), sizeof(value.version) - 1);
                strncpy(&value.machine[0],    utsname.machine.c_str(), sizeof(value.machine));
                strncpy(&value.domainname[0], utsname.domainname.c_str(), sizeof(value.domainname) - 1);
                lin.WriteMemory( buf_addr, (uint8_t *)&value, sizeof(value) );
  
                SetMIPSSystemCallStatus(lin, (ret == -1) ? -target_errno : ret, (ret == -1));
              }
            } sc;
            return &sc;
          }
        case 4000 + 283: // mips specific set_thread_area syscall
          {
            static struct : public SysCall {
              char const* GetName() const { return "set_thread_area"; }
               // SYSCALL_DEFINE1(set_thread_area, unsigned long, addr)
              void Describe( LINUX& lin, std::ostream& sink ) const
              {
                sink << "(unsigned long addr=" << std::hex << GetSystemCallParam(lin, 0) << std::dec << ")";
              }
              void Execute( LINUX& lin, int syscall_id ) const
              {
                if (not lin.SetTargetRegister("hwr_ulr", GetSystemCallParam(lin, 0)))
                  { struct No {}; throw No(); }
                SetMIPSSystemCallStatus( lin, 0, false );
              }
            } sc;
            return &sc;
          }
        }

      return 0;
    }

    bool SetSystemBlob( unisim::util::blob::Blob<address_type>* blob ) const
    {
      typedef unisim::util::blob::Section<address_type> Section;
      typedef unisim::util::blob::Segment<address_type> Segment;

      // The following code is clearly a hack since linux doesn't
      // allocate any TLS. Allocating the TLS is the libc's
      // responsability. Nevertheless, because syscalls emulation
      // is incomplete, errno (located in TLS) may be used before
      // any libc attempt to initialize it.  Thus, we need to
      // introduce a pseudo TLS are for these early accesses to
      // errno.
      if (not blob) return false;

      // TODO: Check that the existing segments/sections are not
      //   in conflict with the early TLS area. Once libc have
      //   initialized TLS this area becomes useless. We
      //   additionnaly need to ensure that early mallocs won't
      //   come in the way.
      //
      // In practice a [0xf0000000-0xf0000fff] zero-filled area seems safe...
      address_type const etls_addr = 0xf0000000UL;
      address_type const etls_size = 0x00001000UL;
      uint8_t* etls_seg_data = (uint8_t*)calloc(1,etls_size);
      // uint8_t* etls_sec_data = (uint8_t*)calloc(1,etls_size);

      Segment* etls_segment =
        new Segment(Segment::TY_LOADABLE, Segment::SA_RW, etls_size, etls_addr, etls_size, etls_size, etls_seg_data);
      // Section* etls_section =
      //   new Section(Section::TY_UNKNOWN, Section::SA_A, ".unisim.linux_os.interface.etls", etls_size, 0, etls_addr, etls_size, etls_sec_data);

      blob->AddSegment(etls_segment);
      blob->AddSection(new Section(Section::TY_NULL, Section::SA_NULL,".unisim.linux_os.etls.middle_pointer", 0, 0, (etls_addr + (etls_size/2)), 0, NULL));

      return true;
    }
  };

} // end of namespace linux_os
} // end of namespace os
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_OS_LINUX_MIPS_HH__
