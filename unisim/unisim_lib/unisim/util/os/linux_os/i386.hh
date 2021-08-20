/*
 *  Copyright (c) 2011-2020,
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

#ifndef __UNISIM_UTIL_OS_LINUX_I386_HH__
#define __UNISIM_UTIL_OS_LINUX_I386_HH__

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

  // Register names
  static char const* const kI386_eax = "%eax";
  static char const* const kI386_ecx = "%ecx";
  static char const* const kI386_edx = "%edx";
  static char const* const kI386_ebx = "%ebx";
  static char const* const kI386_esp = "%esp";
  static char const* const kI386_ebp = "%ebp";
  static char const* const kI386_esi = "%esi";
  static char const* const kI386_edi = "%edi";
  static char const* const kI386_eip = "%eip";

  static char const* const kI386_es = "%es";
  static char const* const kI386_cs = "%cs";
  static char const* const kI386_ss = "%ss";
  static char const* const kI386_ds = "%ds";
  static char const* const kI386_fs = "%fs";
  static char const* const kI386_gs = "%gs";

  template <class LINUX>
  struct I386TS : public LINUX::TargetSystem
  {
    // Name imports
    typedef typename LINUX::SysCall SysCall;
    typedef typename LINUX::address_type address_type;
    typedef typename LINUX::parameter_type parameter_type;
    typedef typename LINUX::UTSName UTSName;
    using LINUX::TargetSystem::lin;
    using LINUX::TargetSystem::name;
    
    // I386 linux structs
    struct i386_timespec
    {
      int32_t tv_sec;     /* Seconds.     (__time_t) */
      int32_t tv_nsec;    /* Nanoseconds. (long int) */
    };

    struct i386_stat64
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

    struct i386_tms
    {
      int32_t tms_utime;          /* User CPU time.                    (clock_t) */
      int32_t tms_stime;          /* System CPU time.                  (clock_t) */
      int32_t tms_cutime;         /* User CPU time of dead children.   (clock_t) */
      int32_t tms_cstime;         /* System CPU time of dead children. (clock_t) */
    };

    struct i386_utsname
    {
      char sysname[65];
      char nodename[65];
      char release[65];
      char version[65];
      char machine[65];
      char domainname[65];
    };

    struct i386_timeval
    {
      int32_t tv_sec;         /* seconds      (__kernel_time_t) */
      int32_t tv_usec;        /* microseconds (__kernel_suseconds_t) */
    };

    struct i386_timezone
    {
      int32_t tz_minuteswest; /* minutes west of Greenwich (int) */
      int32_t tz_dsttime;     /* type of dst correction    (int) */
    };
    
    I386TS( LINUX& _lin ) : LINUX::TargetSystem( "i386", _lin ) {}
    
    bool GetAT_HWCAP( address_type& hwcap ) const { return false; }
        
    void WriteSegmentSelector( char const* segname, uint16_t value ) const
    {
      if (unisim::service::interfaces::Register* reg = this->RegsIF().GetRegister(segname))
        reg->SetValue(&value);
      else
        throw std::logic_error("internal_error");
    }
        
    bool SetupTarget() const
    {
      // Reset all target registers
      {
        char const* clear_registers[] = {
          "%eax", "%ecx", "%edx", "%ebx", "%ebp", "%esi", "%edi",
          "%st", "%st(1)", "%st(2)", "%st(3)", "%st(4)", "%st(5)", "%st(6)", "%st(7)"
        };
        for (int idx = sizeof(clear_registers)/sizeof(clear_registers[0]); --idx >= 0;)
          if (not lin.ClearTargetRegister(clear_registers[idx]))
            return false;
      }
      // Set EIP to the program entry point
      if (not lin.SetTargetRegister(kI386_eip, lin.GetEntryPoint()))
        return false;
      // Set ESP to the base of the created stack
      unisim::util::blob::Section<address_type> const * esp_section =
        lin.GetBlob()->FindSection(".unisim.linux_os.stack.stack_pointer");
      if (esp_section == NULL)
        {
          lin.DebugErrorStream() << "Could not find the stack pointer section." << std::endl;
          return false;
        }
      if (not lin.SetTargetRegister(kI386_esp, esp_section->GetAddr()))
        return false;
          
      // Pseudo GDT initialization  (flat memory + early TLS)
      lin.SetTargetRegister("@gdt[1].base", 0 ); // For code segment
      lin.SetTargetRegister("@gdt[2].base", 0 ); // For data segments
      unisim::util::blob::Section<address_type> const* etls_section =
        lin.GetBlob()->FindSection(".unisim.linux_os.etls.middle_pointer");
      if (not etls_section)
        {
          lin.DebugErrorStream() << "Could not find the early TLS section." << std::endl;
          return false;
        }
      lin.SetTargetRegister("@gdt[3].base", etls_section->GetAddr() ); // for early TLS kludge
          
      this->WriteSegmentSelector(kI386_cs,(1<<3) | (0<<2) | 3); // code
      this->WriteSegmentSelector(kI386_ss,(2<<3) | (0<<2) | 3); // data
      this->WriteSegmentSelector(kI386_ds,(2<<3) | (0<<2) | 3); // data
      this->WriteSegmentSelector(kI386_es,(2<<3) | (0<<2) | 3); // data
      this->WriteSegmentSelector(kI386_fs,(2<<3) | (0<<2) | 3); // data
      this->WriteSegmentSelector(kI386_gs,(3<<3) | (0<<2) | 3); // tls
          
      return true;
    }
        
    static void set_tls_base( LINUX& lin, address_type base )
    {
      lin.SetTargetRegister("@gdt[3].base", base ); // pseudo allocation of a tls descriptor
    }

    static int Fstat64(int fd, struct i386_stat64* target_stat, unisim::util::endian::endian_type endianness)
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

      memset(target_stat, 0, sizeof(struct i386_stat64));
	
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

    static void SetI386SystemCallStatus(LINUX& lin, int ret, bool error) { lin.SetTargetRegister(kI386_eax, (parameter_type) ret); }
    void SetSystemCallStatus(int64_t ret, bool error) const { SetI386SystemCallStatus( lin, ret, error ); }
    
    static parameter_type GetSystemCallParam(LINUX& lin, int id)
    {
      parameter_type val = 0;
          
      switch (id) {
      case 0: lin.GetTargetRegister(kI386_ebx, val); break;
      case 1: lin.GetTargetRegister(kI386_ecx, val); break;
      case 2: lin.GetTargetRegister(kI386_edx, val); break;
      case 3: lin.GetTargetRegister(kI386_esi, val); break;
      case 4: lin.GetTargetRegister(kI386_edi, val); break;
      case 5: lin.GetTargetRegister(kI386_ebp, val); break;
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
      // see either arch/i386/include/asm/unistd.h or arch/i386/include/uapi/asm/unistd.h in LINUX source
      switch (id) {
      case 0: return this->GetSysCall("restart_syscall");
      case 1: return this->GetSysCall("exit");
      case 2: return this->GetSysCall("fork");
      case 3: return this->GetSysCall("read");
      case 4: return this->GetSysCall("write");
      case 5: return this->GetSysCall("open");
      case 6: return this->GetSysCall("close");
      case 7: return this->GetSysCall("waitpid");
      case 8: return this->GetSysCall("creat");
      case 9: return this->GetSysCall("link");
      case 10: return this->GetSysCall("unlink");
      case 11: return this->GetSysCall("execve");
      case 12: return this->GetSysCall("chdir");
      case 13: return this->GetSysCall("time");
      case 14: return this->GetSysCall("mknod");
      case 15: return this->GetSysCall("chmod");
      case 16: return this->GetSysCall("lchown");
      case 17: return this->GetSysCall("break");
      case 18: return this->GetSysCall("oldstat");
      case 19: return this->GetSysCall("lseek");
      case 20: return this->GetSysCall("getpid");
      case 21: return this->GetSysCall("mount");
      case 22: return this->GetSysCall("umount");
      case 23: return this->GetSysCall("setuid");
      case 24: return this->GetSysCall("getuid");
      case 25: return this->GetSysCall("stime");
      case 26: return this->GetSysCall("ptrace");
      case 27: return this->GetSysCall("alarm");
      case 28: return this->GetSysCall("oldfstat");
      case 29: return this->GetSysCall("pause");
      case 30: return this->GetSysCall("utime");
      case 31: return this->GetSysCall("stty");
      case 32: return this->GetSysCall("gtty");
      case 33: return this->GetSysCall("access");
      case 34: return this->GetSysCall("nice");
      case 35: return this->GetSysCall("ftime");
      case 36: return this->GetSysCall("sync");
      case 37: return this->GetSysCall("kill");
      case 38: return this->GetSysCall("rename");
      case 39: return this->GetSysCall("mkdir");
      case 40: return this->GetSysCall("rmdir");
      case 41: return this->GetSysCall("dup");
      case 42: return this->GetSysCall("pipe");
        // // 43: times (see i386 specific)
      case 44: return this->GetSysCall("prof");
      case 45: return this->GetSysCall("brk");
      case 46: return this->GetSysCall("setgid");
      case 47: return this->GetSysCall("getgid");
      case 48: return this->GetSysCall("signal");
      case 49: return this->GetSysCall("geteuid");
      case 50: return this->GetSysCall("getegid");
      case 51: return this->GetSysCall("acct");
      case 52: return this->GetSysCall("umount2");
      case 53: return this->GetSysCall("lock");
      case 54: return this->GetSysCall("ioctl");
      case 55: return this->GetSysCall("fcntl");
      case 56: return this->GetSysCall("mpx");
      case 57: return this->GetSysCall("setpgid");
      case 58: return this->GetSysCall("ulimit");
      case 59: return this->GetSysCall("oldolduname");
      case 60: return this->GetSysCall("umask");
      case 61: return this->GetSysCall("chroot");
      case 62: return this->GetSysCall("ustat");
      case 63: return this->GetSysCall("dup2");
      case 64: return this->GetSysCall("getppid");
      case 65: return this->GetSysCall("getpgrp");
      case 66: return this->GetSysCall("setsid");
      case 67: return this->GetSysCall("sigaction");
      case 68: return this->GetSysCall("sgetmask");
      case 69: return this->GetSysCall("ssetmask");
      case 70: return this->GetSysCall("setreuid");
      case 71: return this->GetSysCall("setregid");
      case 72: return this->GetSysCall("sigsuspend");
      case 73: return this->GetSysCall("sigpending");
      case 74: return this->GetSysCall("sethostname");
      case 75: return this->GetSysCall("setrlimit");
      case 76: return this->GetSysCall("getrlimit");
      case 77: return this->GetSysCall("getrusage");
        // 78: gettimeofday (see i386 specific)
      case 79: return this->GetSysCall("settimeofday");
      case 80: return this->GetSysCall("getgroups");
      case 81: return this->GetSysCall("setgroups");
      case 82: return this->GetSysCall("select");
      case 83: return this->GetSysCall("symlink");
      case 84: return this->GetSysCall("oldlstat");
      case 85: return this->GetSysCall("readlink");
      case 86: return this->GetSysCall("uselib");
      case 87: return this->GetSysCall("swapon");
      case 88: return this->GetSysCall("reboot");
      case 89: return this->GetSysCall("readdir");
      case 90: return this->GetSysCall("mmap");
      case 91: return this->GetSysCall("munmap");
      case 92: return this->GetSysCall("truncate");
      case 93: return this->GetSysCall("ftruncate");
      case 94: return this->GetSysCall("fchmod");
      case 95: return this->GetSysCall("fchown");
      case 96: return this->GetSysCall("getpriority");
      case 97: return this->GetSysCall("setpriority");
      case 98: return this->GetSysCall("profil");
      case 99: return this->GetSysCall("statfs");
      case 100: return this->GetSysCall("fstatfs");
      case 101: return this->GetSysCall("ioperm");
      case 102: return this->GetSysCall("socketcall");
      case 103: return this->GetSysCall("syslog");
      case 104: return this->GetSysCall("setitimer");
      case 105: return this->GetSysCall("getitimer");
      case 106: return this->GetSysCall("stat");
      case 107: return this->GetSysCall("lstat");
        // 108 fstat (see i386 specific)
      case 109: return this->GetSysCall("olduname");
      case 110: return this->GetSysCall("iopl");
      case 111: return this->GetSysCall("vhangup");
      case 112: return this->GetSysCall("idle");
      case 113: return this->GetSysCall("vm86old");
      case 114: return this->GetSysCall("wait4");
      case 115: return this->GetSysCall("swapoff");
      case 116: return this->GetSysCall("sysinfo");
      case 117: return this->GetSysCall("ipc");
      case 118: return this->GetSysCall("fsync");
      case 119: return this->GetSysCall("sigreturn");
      case 120: return this->GetSysCall("clone");
      case 121: return this->GetSysCall("setdomainname");
        // 122: uname (see i386 specific)
      case 123: return this->GetSysCall("modify_ldt");
      case 124: return this->GetSysCall("adjtimex");
      case 125: return this->GetSysCall("mprotect");
      case 126: return this->GetSysCall("sigprocmask");
      case 127: return this->GetSysCall("create_module");
      case 128: return this->GetSysCall("init_module");
      case 129: return this->GetSysCall("delete_module");
      case 130: return this->GetSysCall("get_kernel_syms");
      case 131: return this->GetSysCall("quotactl");
      case 132: return this->GetSysCall("getpgid");
      case 133: return this->GetSysCall("fchdir");
      case 134: return this->GetSysCall("bdflush");
      case 135: return this->GetSysCall("sysfs");
      case 136: return this->GetSysCall("personality");
      case 137: return this->GetSysCall("afs_syscall");
      case 138: return this->GetSysCall("setfsuid");
      case 139: return this->GetSysCall("setfsgid");
      case 140: return this->GetSysCall("_llseek");
      case 141: return this->GetSysCall("getdents");
      case 142: return this->GetSysCall("_newselect");
      case 143: return this->GetSysCall("flock");
      case 144: return this->GetSysCall("msync");
      case 145: return this->GetSysCall("readv");
      case 146: return this->GetSysCall("writev");
      case 147: return this->GetSysCall("getsid");
      case 148: return this->GetSysCall("fdatasync");
      case 149: return this->GetSysCall("_sysctl");
      case 150: return this->GetSysCall("mlock");
      case 151: return this->GetSysCall("munlock");
      case 152: return this->GetSysCall("mlockall");
      case 153: return this->GetSysCall("munlockall");
      case 154: return this->GetSysCall("sched_setparam");
      case 155: return this->GetSysCall("sched_getparam");
      case 156: return this->GetSysCall("sched_setscheduler");
      case 157: return this->GetSysCall("sched_getscheduler");
      case 158: return this->GetSysCall("sched_yield");
      case 159: return this->GetSysCall("sched_get_priority_max");
      case 160: return this->GetSysCall("sched_get_priority_min");
      case 161: return this->GetSysCall("sched_rr_get_interval");
      case 162: return this->GetSysCall("nanosleep");
      case 163: return this->GetSysCall("mremap");
      case 164: return this->GetSysCall("setresuid");
      case 165: return this->GetSysCall("getresuid");
      case 166: return this->GetSysCall("vm86");
      case 167: return this->GetSysCall("query_module");
      case 168: return this->GetSysCall("poll");
      case 169: return this->GetSysCall("nfsservctl");
      case 170: return this->GetSysCall("setresgid");
      case 171: return this->GetSysCall("getresgid");
      case 172: return this->GetSysCall("prctl");
      case 173: return this->GetSysCall("rt_sigreturn");
      case 174: return this->GetSysCall("rt_sigaction");
      case 175: return this->GetSysCall("rt_sigprocmask");
      case 176: return this->GetSysCall("rt_sigpending");
      case 177: return this->GetSysCall("rt_sigtimedwait");
      case 178: return this->GetSysCall("rt_sigqueueinfo");
      case 179: return this->GetSysCall("rt_sigsuspend");
      case 180: return this->GetSysCall("pread64");
      case 181: return this->GetSysCall("pwrite64");
      case 182: return this->GetSysCall("chown");
      case 183: return this->GetSysCall("getcwd");
      case 184: return this->GetSysCall("capget");
      case 185: return this->GetSysCall("capset");
      case 186: return this->GetSysCall("sigaltstack");
      case 187: return this->GetSysCall("sendfile");
      case 188: return this->GetSysCall("getpmsg");
      case 189: return this->GetSysCall("putpmsg");
      case 190: return this->GetSysCall("vfork");
      case 191: return this->GetSysCall("ugetrlimit");
      case 192: return this->GetSysCall("mmap2");
      case 193: return this->GetSysCall("truncate64");
      case 194: return this->GetSysCall("ftruncate64");
        // 195 stat64 (see i386 specific)
      case 196: return this->GetSysCall("lstat64");
        // 197 fstat64 (see i386 specific)
      case 198: return this->GetSysCall("lchown32");
      case 199: return this->GetSysCall("getuid32");
      case 200: return this->GetSysCall("getgid32");
      case 201: return this->GetSysCall("geteuid32");
      case 202: return this->GetSysCall("getegid32");
      case 203: return this->GetSysCall("setreuid32");
      case 204: return this->GetSysCall("setregid32");
      case 205: return this->GetSysCall("getgroups32");
      case 206: return this->GetSysCall("setgroups32");
      case 207: return this->GetSysCall("fchown32");
      case 208: return this->GetSysCall("setresuid32");
      case 209: return this->GetSysCall("getresuid32");
      case 210: return this->GetSysCall("setresgid32");
      case 211: return this->GetSysCall("getresgid32");
      case 212: return this->GetSysCall("chown32");
      case 213: return this->GetSysCall("setuid32");
      case 214: return this->GetSysCall("setgid32");
      case 215: return this->GetSysCall("setfsuid32");
      case 216: return this->GetSysCall("setfsgid32");
      case 217: return this->GetSysCall("pivot_root");
      case 218: return this->GetSysCall("mincore");
      case 219: return this->GetSysCall("madvise");
      case 220: return this->GetSysCall("getdents64");
      case 221: return this->GetSysCall("fcntl64");
      case 224: return this->GetSysCall("gettid");
      case 225: return this->GetSysCall("readahead");
      case 226: return this->GetSysCall("setxattr");
      case 227: return this->GetSysCall("lsetxattr");
      case 228: return this->GetSysCall("fsetxattr");
      case 229: return this->GetSysCall("getxattr");
      case 230: return this->GetSysCall("lgetxattr");
      case 231: return this->GetSysCall("fgetxattr");
      case 232: return this->GetSysCall("listxattr");
      case 233: return this->GetSysCall("llistxattr");
      case 234: return this->GetSysCall("flistxattr");
      case 235: return this->GetSysCall("removexattr");
      case 236: return this->GetSysCall("lremovexattr");
      case 237: return this->GetSysCall("fremovexattr");
      case 238: return this->GetSysCall("tkill");
      case 239: return this->GetSysCall("sendfile64");
      case 240: return this->GetSysCall("futex");
      case 241: return this->GetSysCall("sched_setaffinity");
      case 242: return this->GetSysCall("sched_getaffinity");
        // 243: set_thread_area (see i386 specific)
      case 244: return this->GetSysCall("get_thread_area");
      case 245: return this->GetSysCall("io_setup");
      case 246: return this->GetSysCall("io_destroy");
      case 247: return this->GetSysCall("io_getevents");
      case 248: return this->GetSysCall("io_submit");
      case 249: return this->GetSysCall("io_cancel");
      case 250: return this->GetSysCall("fadvise64");
      case 252: return this->GetSysCall("exit_group");
      case 253: return this->GetSysCall("lookup_dcookie");
      case 254: return this->GetSysCall("epoll_create");
      case 255: return this->GetSysCall("epoll_ctl");
      case 256: return this->GetSysCall("epoll_wait");
      case 257: return this->GetSysCall("remap_file_pages");
      case 258: return this->GetSysCall("set_tid_address");
      case 259: return this->GetSysCall("timer_create");
      case 260: return this->GetSysCall("timer_settime");
      case 261: return this->GetSysCall("timer_gettime");
      case 262: return this->GetSysCall("timer_getoverrun");
      case 263: return this->GetSysCall("timer_delete");
      case 264: return this->GetSysCall("clock_settime");
      case 265: return this->GetSysCall("clock_gettime");
      case 266: return this->GetSysCall("clock_getres");
      case 267: return this->GetSysCall("clock_nanosleep");
      case 268: return this->GetSysCall("statfs64");
      case 269: return this->GetSysCall("fstatfs64");
      case 270: return this->GetSysCall("tgkill");
      case 271: return this->GetSysCall("utimes");
      case 272: return this->GetSysCall("fadvise64_64");
      case 273: return this->GetSysCall("vserver");
      case 274: return this->GetSysCall("mbind");
      case 275: return this->GetSysCall("get_mempolicy");
      case 276: return this->GetSysCall("set_mempolicy");
      case 277: return this->GetSysCall("mq_open");
      case 278: return this->GetSysCall("mq_unlink");
      case 279: return this->GetSysCall("mq_timedsend");
      case 280: return this->GetSysCall("mq_timedreceive");
      case 281: return this->GetSysCall("mq_notify");
      case 282: return this->GetSysCall("mq_getsetattr");
      case 283: return this->GetSysCall("kexec_load");
      case 284: return this->GetSysCall("waitid");
      case 286: return this->GetSysCall("add_key");
      case 287: return this->GetSysCall("request_key");
      case 288: return this->GetSysCall("keyctl");
      case 289: return this->GetSysCall("ioprio_set");
      case 290: return this->GetSysCall("ioprio_get");
      case 291: return this->GetSysCall("inotify_init");
      case 292: return this->GetSysCall("inotify_add_watch");
      case 293: return this->GetSysCall("inotify_rm_watch");
      case 294: return this->GetSysCall("migrate_pages");
      case 295: return this->GetSysCall("openat");
      case 296: return this->GetSysCall("mkdirat");
      case 297: return this->GetSysCall("mknodat");
      case 298: return this->GetSysCall("fchownat");
      case 299: return this->GetSysCall("futimesat");
      case 300: return this->GetSysCall("fstatat64");
      case 301: return this->GetSysCall("unlinkat");
      case 302: return this->GetSysCall("renameat");
      case 303: return this->GetSysCall("linkat");
      case 304: return this->GetSysCall("symlinkat");
      case 305: return this->GetSysCall("readlinkat");
      case 306: return this->GetSysCall("fchmodat");
      case 307: return this->GetSysCall("faccessat");
      case 308: return this->GetSysCall("pselect6");
      case 309: return this->GetSysCall("ppoll");
      case 310: return this->GetSysCall("unshare");
      case 311: return this->GetSysCall("set_robust_list");
      case 312: return this->GetSysCall("get_robust_list");
      case 313: return this->GetSysCall("splice");
      case 314: return this->GetSysCall("sync_file_range");
      case 315: return this->GetSysCall("tee");
      case 316: return this->GetSysCall("vmsplice");
      case 317: return this->GetSysCall("move_pages");
      case 318: return this->GetSysCall("getcpu");
      case 319: return this->GetSysCall("epoll_pwait");
      case 320: return this->GetSysCall("utimensat");
      case 321: return this->GetSysCall("signalfd");
      case 322: return this->GetSysCall("timerfd_create");
      case 323: return this->GetSysCall("eventfd");
      case 324: return this->GetSysCall("fallocate");
      case 325: return this->GetSysCall("timerfd_settime");
      case 326: return this->GetSysCall("timerfd_gettime");
      case 327: return this->GetSysCall("signalfd4");
      case 328: return this->GetSysCall("eventfd2");
      case 329: return this->GetSysCall("epoll_create1");
      case 330: return this->GetSysCall("dup3");
      case 331: return this->GetSysCall("pipe2");
      case 332: return this->GetSysCall("inotify_init1");
      case 333: return this->GetSysCall("preadv");
      case 334: return this->GetSysCall("pwritev");
      case 335: return this->GetSysCall("rt_tgsigqueueinfo");
      case 336: return this->GetSysCall("perf_event_open");
      case 337: return this->GetSysCall("recvmmsg");
      case 338: return this->GetSysCall("fanotify_init");
      case 339: return this->GetSysCall("fanotify_mark");
      case 340: return this->GetSysCall("prlimit64");
      case 341: return this->GetSysCall("name_to_handle_at");
      case 342: return this->GetSysCall("open_by_handle_at");
      case 343: return this->GetSysCall("clock_adjtime");
      case 344: return this->GetSysCall("syncfs");
      case 345: return this->GetSysCall("sendmmsg");
      case 346: return this->GetSysCall("setns");
      case 347: return this->GetSysCall("process_vm_readv");
      case 348: return this->GetSysCall("process_vm_writev");
      case 349: return this->GetSysCall("kcmp");
      case 350: return this->GetSysCall("finit_module");
      case 351: return this->GetSysCall("sched_setattr");
      case 352: return this->GetSysCall("sched_getattr");

      case 122: /* i386 specific uname syscall */
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
  
              struct i386_utsname value;
              memset(&value, 0, sizeof(value));
              UTSName const& utsname = lin.GetUTSName();
              strncpy(&value.sysname[0],    utsname.sysname.c_str(), sizeof(value.sysname) - 1);
              strncpy(&value.nodename[0],   utsname.nodename.c_str(), sizeof(value.nodename) - 1);
              strncpy(&value.release[0],    utsname.release.c_str(), sizeof(value.release) - 1);
              strncpy(&value.version[0],    utsname.version.c_str(), sizeof(value.version) - 1);
              strncpy(&value.machine[0],    utsname.machine.c_str(), sizeof(value.machine));
              strncpy(&value.domainname[0], utsname.domainname.c_str(), sizeof(value.domainname) - 1);
              lin.WriteMemory( buf_addr, (uint8_t *)&value, sizeof(value) );
  
              SetI386SystemCallStatus(lin, (ret == -1) ? -target_errno : ret, (ret == -1));
            }
          } sc;
          return &sc;
        } break;
            
      case 197: /* ARM specific fstat64 syscall */
        {
          static struct : public SysCall {
            char const* GetName() const { return "fstat64"; }
            void Describe( LINUX& lin, std::ostream& sink ) const
            {
              int32_t fd = GetSystemCallParam(lin, 0);
              address_type buf = GetSystemCallParam(lin, 1);
              sink << "(int fd=" << std::dec << fd << ", struct stat *buf=" << std::hex << buf << ")";
            }
            void Execute( LINUX& lin, int syscall_id ) const
            {
              int ret;
              int32_t target_errno = 0;
              
              int32_t target_fd = GetSystemCallParam(lin, 0);
              address_type buf_address = GetSystemCallParam(lin, 1);
	
              int host_fd = SysCall::Target2HostFileDescriptor(lin, target_fd);
	
              if(host_fd == -1)
                {
                  target_errno = LINUX_EBADF;
                  ret = -1;
                }
              else
                {
                  struct i386_stat64 target_stat;
                  ret = Fstat64(host_fd, &target_stat, lin.GetEndianness());
                  if(ret == -1) target_errno = SysCall::HostToLinuxErrno(errno);
			
                  lin.WriteMemory( buf_address, (uint8_t *)&target_stat, sizeof(target_stat));
                }
	
              if(unlikely(lin.GetVerbose()))
                {
                  lin.DebugInfoStream()
                    << "fd = " << target_fd << ", buf_address = 0x" << std::hex << buf_address << std::dec
                    << std::endl;
                }
	
              SetI386SystemCallStatus(lin, (parameter_type) (ret == -1) ? -target_errno : ret, (ret == -1));
            }
          } sc;
          return &sc;
        } break;

      case 243: /* i386 specific set_thread_area syscall */
        {
          static struct : public SysCall {
            char const* GetName() const { return "set_thread_area"; }
            void Describe( LINUX& lin, std::ostream& sink ) const
            {
              uint32_t user_desc_ptr = GetSystemCallParam(lin, 0);
              sink << "(struct user_desc *u_info=" << std::hex << user_desc_ptr << std::dec << ")";
            }
            void Execute( LINUX& lin, int syscall_id ) const
            {
              uint32_t user_desc_ptr = GetSystemCallParam(lin, 0);
                  
              parameter_type entry_number, base_addr, limit, attributes;
              lin.ReadMemory( user_desc_ptr + 0x0, entry_number, true );
              lin.ReadMemory( user_desc_ptr + 0x4, base_addr, true );
              lin.ReadMemory( user_desc_ptr + 0x8, limit, true );
              lin.ReadMemory( user_desc_ptr + 0xc, attributes, true );
              entry_number = unisim::util::endian::Target2Host( lin.GetEndianness(), entry_number );
              base_addr    = unisim::util::endian::Target2Host( lin.GetEndianness(), base_addr );
              limit        = unisim::util::endian::Target2Host( lin.GetEndianness(), limit );
              attributes   = unisim::util::endian::Target2Host( lin.GetEndianness(), attributes );
                  
              if (entry_number != parameter_type(-1))
                throw std::logic_error("internal error in i686 segment descriptors");
                  
              set_tls_base( lin, base_addr );
                  
              entry_number = 3;
              entry_number = unisim::util::endian::Host2Target( lin.GetEndianness(), entry_number );
              lin.WriteMemory( user_desc_ptr + 0x0, (uint8_t*)&entry_number, 4 );

              SetI386SystemCallStatus( lin, 0, false );
            }
          } sc;
          return &sc;
        } break;

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

#endif // __UNISIM_UTIL_OS_LINUX_I386_HH__
